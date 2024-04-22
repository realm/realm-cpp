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

#ifndef CPPREALM_BRIDGE_UUID_HPP
#define CPPREALM_BRIDGE_UUID_HPP

#include <cpprealm/internal/bridge/utils.hpp>
#include <string_view>
#include <array>

namespace realm {
    struct uuid;
    class UUID;
}

namespace realm::internal::bridge {
    struct uuid {
        uuid();
        uuid(const uuid& other) = default;
        uuid& operator=(const uuid& other) = default;
        uuid(uuid&& other) = default;
        uuid& operator=(uuid&& other) = default;
        ~uuid() = default;
        uuid(const UUID&); //NOLINT(google-explicit-constructor);
        explicit uuid(const std::string&);
        uuid(const std::array<uint8_t, 16>&);
        uuid(const struct ::realm::uuid&); //NOLINT(google-explicit-constructor);
        operator UUID() const; //NOLINT(google-explicit-constructor);
        operator ::realm::uuid() const; //NOLINT(google-explicit-constructor);
        [[nodiscard]] std::string to_string() const;
        [[nodiscard]] std::string to_base64() const;
        [[nodiscard]] std::array<uint8_t, 16> to_bytes() const;
    private:
        std::array<uint8_t, 16> m_uuid;
        friend bool operator ==(const uuid&, const uuid&);
        friend bool operator !=(const uuid&, const uuid&);
        friend bool operator >(const uuid&, const uuid&);
        friend bool operator <(const uuid&, const uuid&);
        friend bool operator >=(const uuid&, const uuid&);
        friend bool operator <=(const uuid&, const uuid&);
    };

    bool operator ==(const uuid&, const uuid&);
    bool operator !=(const uuid&, const uuid&);
    bool operator >(const uuid&, const uuid&);
    bool operator <(const uuid&, const uuid&);
    bool operator >=(const uuid&, const uuid&);
    bool operator <=(const uuid&, const uuid&);
}

#endif //CPPREALM_BRIDGE_UUID_HPP
