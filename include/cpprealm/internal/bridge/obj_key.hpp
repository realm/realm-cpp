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

#ifndef CPPREALM_BRIDGE_OBJ_KEY_HPP
#define CPPREALM_BRIDGE_OBJ_KEY_HPP

#include <cinttypes>
#include <memory>
#include <type_traits>
#include <cpprealm/internal/bridge/utils.hpp>

namespace realm {
    struct ObjKey;
    struct ObjLink;
}

namespace realm::internal::bridge {
    struct obj_key {
        obj_key(const ObjKey&);
        obj_key(int64_t);
        obj_key();
        obj_key(const obj_key& other) = default;
        obj_key& operator=(const obj_key& other) = default;
        obj_key(obj_key&& other) = default;
        obj_key& operator=(obj_key&& other) = default;
        ~obj_key() = default;
        operator ObjKey() const;
    private:
        int64_t m_obj_key;
    };

    bool operator==(const obj_key &, const obj_key &);

    bool operator!=(const obj_key &, const obj_key &);
}

namespace realm::internal::bridge {
    struct obj_link {
        obj_link(const ObjLink&);
        obj_link();
        obj_link(const obj_link& other);
        obj_link(uint32_t table_key, obj_key obj_key);
        obj_link& operator=(const obj_link& other);
        obj_link(obj_link&& other);
        obj_link& operator=(obj_link&& other);
        ~obj_link();
        operator ObjLink() const;
        obj_key get_obj_key();
        uint32_t get_table_key();
    private:
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        storage::ObjLink m_obj_link[1];
#else
        std::shared_ptr<ObjLink> m_obj_link;
#endif
    };

    bool operator==(const obj_link &, const obj_link &);

    bool operator!=(const obj_link &, const obj_link &);
}


#endif //CPPREALM_BRIDGE_OBJ_KEY_HPP
