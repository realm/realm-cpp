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

#ifndef REALM_ANALYTICS_HPP
#define REALM_ANALYTICS_HPP

// Asynchronously submits build information to Realm if running in
// DEBUG mode.
//
// To be clear: this does *not* run when your app is built with RELEASE mode;
// To forcefully disable analytics set the REALM_DISABLE_ANALYTICS environment variable.
//
// Why are we doing this? In short, because it helps us build a better product
// for you. None of the data personally identifies you, your employer or your
// app, but it *will* help us understand what version of c++ you use, what host
// you, etc. Having this info will help prioritizing our time,
// adding new features and deprecating old features. Collecting an anonymized
// bundle & anonymized MAC is the only way for us to count actual usage of the
// other metrics accurately. If we don’t have a way to deduplicate the info
// reported, it will be useless, as a single developer building their app
// 10 times would report 10 times more than another single developer that
// only builds once, making the data all but useless.
// No one likes sharing data unless it’s necessary, we get it, and we’ve
// debated adding this for a long long time. Since Realm is a free product
// without an email signup, we feel this is a necessary step so we can collect
// relevant data to build a better product for you. If you truly, absolutely
// feel compelled to not send this data back to Realm, then you can set the environment variable
// named REALM_DISABLE_ANALYTICS. Since Realm is free we believe
// letting these analytics run is a small price to pay for the product & support
// we give you.
//
// Currently the following information is reported:
// - What version of Realm is being used, and which version of C++ you are using.
// - What type of operating system you are using.
// - An anonymous MAC address to aggregate the other information on.
namespace realm {
    struct realm_analytics {
        static void send();
    };
} // namespace realm

#endif //REALM_ANALYTICS_HPP
