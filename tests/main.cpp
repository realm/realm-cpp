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

#include <catch2/catch_all.hpp>
#include <realm/util/features.h>

int main(int argc, char* argv[]) {
    Catch::ConfigData config;

    if (getenv("REALM_CI")) {
        config.showDurations = Catch::ShowDurations::Always; // this is to help debug hangs
        config.reporterSpecifications.push_back(Catch::ReporterSpec{"console", {}, {}, {}});
        config.reporterSpecifications.push_back(Catch::ReporterSpec{"junit", {"TestResults.xml"}, {}, {}});
    } else if (getenv("REALM_LOCAL_ENDPOINT") == nullptr) {
#if REALM_MOBILE || REALM_WINDOWS
        // skip app and sync tests on Mobile and Windows platforms if we're not running on CI or have an explicit BAAS endpoint set
        config.testsOrTags.push_back("~[app]");
        config.testsOrTags.push_back("~[sync]");
#endif
    }

    Catch::Session session;
    session.useConfigData(config);
    int result = session.run(argc, argv);
    return result < 0xff ? result : 0xff;
}
