////////////////////////////////////////////////////////////////////////////
//
// Copyright 2024 Realm Inc.
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

#ifndef CPPREALM_PLATFORM_NETWORKING_HPP
#define CPPREALM_PLATFORM_NETWORKING_HPP

#include <cpprealm/internal/bridge/status.hpp>
#include <cpprealm/internal/generic_network_transport.hpp>

#ifndef REALMCXX_VERSION_MAJOR
#include <cpprealm/version_numbers.hpp>
#endif

namespace realm {
    namespace app {
        struct GenericNetworkTransport;
    }
    namespace sync {
        class SyncSocketProvider;
    }

    namespace util {
        class Logger;
    }
}
namespace realm::networking {

    std::shared_ptr<realm::sync::SyncSocketProvider> default_sync_socket_provider_factory(const std::shared_ptr<util::Logger>& logger,
                                                                                          const std::string& user_agent_binding_info,
                                                                                          const std::string& user_agent_application_info,
                                                                                          const std::shared_ptr<websocket_event_handler>&);

    std::shared_ptr<app::GenericNetworkTransport> default_http_client_factory(const std::optional<std::map<std::string, std::string>>& custom_http_headers = std::nullopt,
                                                                              const std::optional<internal::bridge::realm::sync_config::proxy_config>& proxy_config = std::nullopt);
}

#endif//CPPREALM_PLATFORM_NETWORKING_HPP
