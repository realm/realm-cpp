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

#include "admin_utils.hpp"
#include <catch2/catch_all.hpp>
#include <filesystem>
#include <realm/util/features.h>

int main(int argc, char *argv[]) {
    Catch::ConfigData config;

#ifdef REALM_CPP_PERFORMANCE_TESTS
    config.sectionsToRun.push_back("~[performance]");
#else
    config.testsOrTags.push_back("~[performance]");
#endif

    if (getenv("REALM_CI")) {
        config.showDurations = Catch::ShowDurations::Always;// this is to help debug hangs
        config.reporterSpecifications.push_back(Catch::ReporterSpec{"console", {}, {}, {}});
        config.reporterSpecifications.push_back(Catch::ReporterSpec{"junit", {"TestResults.xml"}, {}, {}});
    }

#ifdef CPPREALM_ENABLE_SYNC_TESTS
    Admin::shared().cache_app_id(Admin::shared().create_app({"str_col", "_id"}));
#endif

    Catch::Session session;
    session.useConfigData(config);
    int result = session.run(argc, argv);
    return result < 0xff ? result : 0xff;
}