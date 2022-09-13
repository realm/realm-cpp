#ifndef REALM_SYNC_TEST_UTILS_HPP
#define REALM_SYNC_TEST_UTILS_HPP

#include <cpprealm/sdk.hpp>

#include <realm/object-store/sync/sync_session.hpp>

#include <sync/sync_test_utils.hpp>
#include <baas_admin_api.hpp>
#include <test_file.hpp>

namespace test {
    realm::task<void> wait_for_sync_uploads(const realm::User& user) {
        auto sync_sessions = user.m_user->sync_manager()->get_all_sessions();
        CHECK_EQUALS(sync_sessions.size(), 1);
        auto session = sync_sessions[0];
        co_await realm::make_awaitable<std::error_code>([&] (auto cb) {
            session->wait_for_upload_completion(cb);
        });
        co_return;
    }

    realm::task<void> wait_for_sync_downloads(const realm::User& user) {
        auto sync_sessions = user.m_user->sync_manager()->get_all_sessions();
        CHECK_EQUALS(sync_sessions.size(), 1);
        auto session = sync_sessions[0];
        co_await realm::make_awaitable<std::error_code>([&] (auto cb) {
            session->wait_for_download_completion(cb);
        });
        co_return;
    }


struct SyncTestHarness {

    realm::AppSession get_runtime_app_session(std::string base_url)
    {
        const auto dog_schema =
        realm::ObjectSchema("Dog", {
            realm::Property("_id", realm::PropertyType::Int, true),
            realm::Property("age", realm::PropertyType::Int),
            realm::Property("name", realm::PropertyType::String),
            realm::Property("realm_id", realm::PropertyType::String | realm::PropertyType::Nullable)});

        const auto person_schema =
        realm::ObjectSchema("Person", {
            realm::Property("_id", realm::PropertyType::Int, true),
            realm::Property("age", realm::PropertyType::Int),
            realm::Property("dog", realm::PropertyType::Object | realm::PropertyType::Nullable, "Dog"),
            realm::Property("name", realm::PropertyType::String),
            realm::Property("realm_id", realm::PropertyType::String | realm::PropertyType::Nullable)});

        /*
         struct Dog: realm::object {
             realm::persisted<std::string> name;
             realm::persisted<int> age;

             using schema = realm::schema<"Dog",
                     realm::property<"name", &Dog::name>,
                     realm::property<"age", &Dog::age>>;
         };

         struct Person: realm::object {
             realm::persisted<std::string> name;
             realm::persisted<int> age;
             realm::persisted<std::optional<Dog>> dog;

             using schema = realm::schema<"Person",
                     realm::property<"name", &Person::name>,
                     realm::property<"age", &Person::age>,
                     realm::property<"dog", &Person::dog>>;
         };
         */

        realm::Schema default_schema({dog_schema, person_schema});

        static const realm::AppSession cached_app_session = [&] {
            auto config = realm::minimal_app_config(base_url, "test", default_schema);
            realm::AppCreateConfig::FLXSyncConfig flx_sync_config {
                .queryable_fields = {"_id", "name"}
            };
            config.flx_sync_config = flx_sync_config;
            auto cached_app_session = create_app(config);
            return cached_app_session;
        }();
        return cached_app_session;
    }

    std::string setup()
    {
        auto session = TestAppSession(get_runtime_app_session("http://localhost:9090"), nullptr, DeleteApp{false});
        //TestAppSession(get_runtime_app_session(get_base_url()), nullptr, DeleteApp{false})

        auto app = session.app();
        return app->config().app_id;
    }
};
}

#endif //REALM_SYNC_TEST_UTILS_HPP
