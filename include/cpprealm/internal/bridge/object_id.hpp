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

#ifndef CPPREALM_BRIDGE_OBJECT_ID_HPP
#define CPPREALM_BRIDGE_OBJECT_ID_HPP

#include <cpprealm/internal/bridge/utils.hpp>
#include <array>

namespace realm {
    struct object_id;
    class ObjectId;
}

namespace realm::internal::bridge {
    struct object_id {
        object_id();
        object_id(const object_id& other) = default;
        object_id& operator=(const object_id& other) = default;
        object_id(object_id&& other) = default;
        object_id& operator=(object_id&& other) = default;
        ~object_id() = default;
        object_id(const ObjectId&); //NOLINT(google-explicit-constructor);
        explicit object_id(const std::string&);
        object_id(const struct ::realm::object_id&); //NOLINT(google-explicit-constructor);
        operator ObjectId() const; //NOLINT(google-explicit-constructor);
        operator ::realm::object_id() const; //NOLINT(google-explicit-constructor);
        [[nodiscard]] std::string to_string() const;
        [[nodiscard]] static object_id generate();
    private:
        std::array<uint8_t, 12> m_object_id;

        friend bool operator ==(const object_id&, const object_id&);
        friend bool operator !=(const object_id&, const object_id&);
        friend bool operator >(const object_id&, const object_id&);
        friend bool operator <(const object_id&, const object_id&);
        friend bool operator >=(const object_id&, const object_id&);
        friend bool operator <=(const object_id&, const object_id&);
    };

    bool operator ==(const object_id&, const object_id&);
    bool operator !=(const object_id&, const object_id&);
    bool operator >(const object_id&, const object_id&);
    bool operator <(const object_id&, const object_id&);
    bool operator >=(const object_id&, const object_id&);
    bool operator <=(const object_id&, const object_id&);
}

#endif //CPPREALM_BRIDGE_OBJECT_ID_HPP

