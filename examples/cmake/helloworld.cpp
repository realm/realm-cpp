#include "utils.hpp"

#include <cpprealm/sdk.hpp>

struct FooObject: realm::object {

    realm::persisted<int> _id;
    realm::persisted<bool> bool_col;
    realm::persisted<std::string> str_col;
    realm::persisted<std::chrono::time_point<std::chrono::system_clock>> date_col;
    realm::persisted<realm::uuid> uuid_col;
    realm::persisted<std::vector<std::uint8_t>> binary_col;

    static constexpr auto schema = realm::schema(
        "AllTypesObject",
        realm::property<&FooObject::_id, true>("_id"),
        realm::property<&FooObject::bool_col>("bool_col"),
        realm::property<&FooObject::str_col>("str_col"),
        realm::property<&FooObject::date_col>("date_col"),
        realm::property<&FooObject::uuid_col>("uuid_col"),
        realm::property<&FooObject::binary_col>("binary_col"));
};

void run_realm() {
    auto app = realm::App("cpp-sdk-2-pgjiz");
    auto user = app.login(realm::App::Credentials::anonymous()).get_future().get();

    auto flx_sync_config = user.flexible_sync_configuration();
    std::cout << flx_sync_config.path << std::endl;

    auto tsr = realm::async_open<FooObject>(flx_sync_config).get_future().get();
    auto synced_realm = tsr.resolve();

    auto update_success = synced_realm.subscriptions().update([](realm::MutableSyncSubscriptionSet& subs) {
        subs.clear();
    }).get_future().get();

    update_success = synced_realm.subscriptions().update([](realm::MutableSyncSubscriptionSet& subs) {
        subs.add<FooObject>("foo-strings", [](auto& obj) {
            return obj.str_col != "alex"; // sync all objects where name does not equal 'alex'
        });
    }).get_future().get();

    auto person = FooObject();
    person._id = 1;
    person.str_col = "john";

    synced_realm.write([&synced_realm, &person]() {
        synced_realm.add(person);
    });

    auto token = person.observe<FooObject>([](auto&& change) {
        if (change.error) {
            std::cout << "An error occurred: " << change.error << std::endl;
        } else if (change.is_deleted) {
            std::cout << "The object was deleted." << std::endl;
        } else {
            for (auto &&property : change.property_changes) {
                std::cout << "Property " << property.name << " changed"
                    << " from " << property.old_value
                    << " to " << property.new_value
                    << std::endl;
            }
        }
    });

    wait_for_sync(user);

    synced_realm.write([&synced_realm, &person]() {
        person.str_col = "sarah";
    });

    wait_for_sync(user);

    synced_realm.write([&synced_realm, &person]() {
        person.str_col = "bob";
    });

    wait_for_sync(user);
}

int main() {
    run_realm();
    return 0;
}
