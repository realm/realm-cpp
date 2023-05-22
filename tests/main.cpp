////////////////////////////////////////////////////////////////////////////
//
// Copyright 2022 Realm Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
////////////////////////////////////////////////////////////////////////////

#define CATCH_CONFIG_RUNNER

#include "main.hpp"
#include <catch2/catch_all.hpp>
#include <realm/util/features.h>
#include <filesystem>
//#include "admin_utils.hpp"

#include <cpprealm/internal/bridge/realm.hpp>




#include <cpprealm/sdk.hpp>

#include <cpprealm/experimental/db.hpp>
#include <cpprealm/experimental/observation.hpp>

using namespace realm;


namespace realm::experimental {
    struct Dog {
        std::string name;
        std::string name2;
        std::string name3;
        int64_t foo2 = 0;
        std::string name4;

        int64_t age = 0;
    };
    REALM_SCHEMA(Dog, name)
    //#define DECLARE_MANAGED_PROPERTY(cls, p) &realm::experimental::managed<cls>::p,l
    //#define DECLARE_MANAGED_PROPERTY_NAME(cls, p) #p,

        /*
    template<>
    struct managed<Dog> {
        static constexpr bool is_object = true;
        //FOR_EACH(DECLARE_PERSISTED, cls, __VA_ARGS__)
        managed<decltype(Dog::name)> name;

        static constexpr auto schema = realm::schema("Dog", std::tuple{realm::property<&Dog::name>("name")});
        static constexpr auto managed_pointers() {
            return std::tuple{&realm::experimental::managed<Dog>::name};
        }

        static constexpr auto managed_pointers_names = std::tuple{"name"};
        internal::bridge::obj m_obj;
        internal::bridge::realm m_realm;
        explicit managed(internal::bridge::obj &&obj,
                         internal::bridge::realm realm)
            : m_obj(std::move(obj)), m_realm(std::move(realm)) {

            std::apply([&](auto &&...ptr) {
                std::apply([&](auto &&..._name) {
                    ((*this.*ptr).assign(&m_obj, &m_realm, m_obj.get_table().get_column_key(_name)), ...);
                },
                           managed_pointers_names);
            }, managed_pointers());
        }
        managed(const managed &other) : m_obj(other.m_obj) {
            m_realm = other.m_realm;

            std::apply([this, &other](auto &&...ptr) {
                ((*this.*ptr).assign(&m_obj, &m_realm, ((other.*ptr)).m_key), ...);
            }, managed_pointers());
        }
        typename decltype(schema)::variant_t property_value_for_name(std::string_view _name) const {
            FOR_EACH(DECLARE_COND_PROPERTY_VALUE_FOR_NAME, cls, __VA_ARGS__)
            return {};
        }
    };
    struct meta_schema_Dog {
        meta_schema_Dog() {
            realm::experimental::db::schemas.push_back(managed<Dog>::schema.to_core_schema());
        }
    };
    meta_schema_Dog _meta_schema_Dog{};
    */
}// namespace realm::experimental

int main(int argc, char* argv[]) {

    auto dog = realm::experimental::Dog();


    realm_path path;
    experimental::db db = experimental::open(path);

    auto managed_dog = db.write([&]() {
        return db.add(realm::experimental::Dog());
    });

    auto x = managed_dog.name;

    
    Catch::ConfigData config;
    if (getenv("REALM_CI")) {
        config.showDurations = Catch::ShowDurations::Always; // this is to help debug hangs
        config.reporterSpecifications.push_back(Catch::ReporterSpec{"console", {}, {}, {}});
        config.reporterSpecifications.push_back(Catch::ReporterSpec{"junit", {"TestResults.xml"}, {}, {}});
       // Admin::shared().cache_app_id(Admin::shared().create_app({"str_col", "_id"}));
    } else if (getenv("REALM_LOCAL_ENDPOINT") == nullptr) {
#if REALM_MOBILE || REALM_WINDOWS
        // skip app and sync tests on Mobile and Windows platforms if we're not running on CI or have an explicit BAAS endpoint set
        config.testsOrTags.push_back("~[app]");
        config.testsOrTags.push_back("~[sync]");
#else
        config.showDurations = Catch::ShowDurations::Always; // this is to help debug hangs
//        config.testsOrTags.emplace_back("~[performance]");
        //Admin::shared().cache_app_id(Admin::shared().create_app({"str_col", "_id"}));
#endif
    }

    Catch::Session session;
    session.useConfigData(config);
    int result = session.run(argc, argv);
    return result < 0xff ? result : 0xff;
    
    return 0;
}
