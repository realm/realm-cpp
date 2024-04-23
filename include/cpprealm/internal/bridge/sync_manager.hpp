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

#ifndef CPPREALM_BRIDGE_SYNC_MANAGER_HPP
#define CPPREALM_BRIDGE_SYNC_MANAGER_HPP

#include <memory>
#include <string>
#include <cpprealm/logger.hpp>
#include <cpprealm/internal/bridge/realm.hpp>

namespace realm {
    class SyncManager;
    class App;
    struct user;

    namespace internal::bridge {
        struct sync_manager {
            void set_log_level(logger::level);
        private:
            friend class ::realm::App;
            friend struct ::realm::user;
            sync_manager(const std::shared_ptr<SyncManager> &);
            std::shared_ptr<SyncManager> m_manager;
        };
    }
}

#endif //CPPREALM_BRIDGE_SYNC_MANAGER_HPP
