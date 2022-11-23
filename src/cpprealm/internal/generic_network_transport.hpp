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

#ifndef realm_cpp_generic_network_transport
#define realm_cpp_generic_network_transport

#include <realm/object-store/sync/impl/sync_client.hpp>

namespace realm {
namespace internal {

class DefaultTransport : public app::GenericNetworkTransport {
public:
    void send_request_to_server(const app::Request& request,
                                util::UniqueFunction<void(const app::Response&)>&& completion);
};

} // namespace internal
} // namespace realm

#endif //realm_cpp_generic_network_transport
