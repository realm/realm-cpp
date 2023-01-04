#include <cpprealm/sdk.hpp>

struct Foo: realm::object<Foo> {

    realm::persisted<int> _id;
    realm::persisted<bool> bool_col;
    realm::persisted<std::string> str_col;
    realm::persisted<std::chrono::time_point<std::chrono::system_clock>> date_col;
    realm::persisted<realm::uuid> uuid_col;
    realm::persisted<std::vector<std::uint8_t>> binary_col;

    static constexpr auto schema = realm::schema(
        "AllTypesObject",
        realm::property<&Foo::_id, true>("_id"),
        realm::property<&Foo::bool_col>("bool_col"),
        realm::property<&Foo::str_col>("str_col"),
        realm::property<&Foo::date_col>("date_col"),
        realm::property<&Foo::uuid_col>("uuid_col"),
        realm::property<&Foo::binary_col>("binary_col"));
};

void run_realm() {
    // Create a new Device Sync App and turn development mode on under sync settings.
    auto app = realm::App("MY_APP_ID");
    auto user = app.login(realm::App::credentials::anonymous()).get_future().get();

    auto flx_sync_config = user.flexible_sync_configuration();
    auto tsr = realm::async_open<Foo>(flx_sync_config).get_future().get();
    auto synced_realm = tsr.resolve();

    auto update_success = synced_realm.subscriptions().update([](realm::MutableSyncSubscriptionSet& subs) {
        subs.clear();
    }).get_future().get();

    update_success = synced_realm.subscriptions().update([](realm::MutableSyncSubscriptionSet& subs) {
        subs.add<Foo>("foo-strings", [](auto& obj) {
            return obj.str_col != "alex"; // sync all objects where name does not equal 'alex'
        });
    }).get_future().get();

    auto person = Foo();
    person._id = 1;
    person.str_col = "john";

    synced_realm.write([&synced_realm, &person]() {
        synced_realm.add(person);
    });

    auto token = person.observe([](auto&& change) {
        std::cout << "property changed" << std::endl;
    });

    synced_realm.write([&synced_realm, &person]() {
        person.str_col = "sarah";
    });

    synced_realm.write([&synced_realm, &person]() {
        person.str_col = "bob";
    });
}

int main() {
    run_realm();
    return 0;
}
