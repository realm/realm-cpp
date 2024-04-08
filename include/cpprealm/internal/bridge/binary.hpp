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

#ifndef CPPREALM_BRIDGE_BINARY_HPP
#define CPPREALM_BRIDGE_BINARY_HPP

#include <vector>
#include <cpprealm/internal/bridge/utils.hpp>

namespace realm {
    class BinaryData;
    class OwnedBinaryData;
}
namespace realm::internal::bridge {
    struct binary {
        binary();
        binary(const binary& other) ;
        binary& operator=(const binary& other) ;
        binary(binary&& other);
        binary& operator=(binary&& other);
        ~binary();
        binary(const BinaryData&); //NOLINT(google-explicit-constructor)
        binary(const std::vector<uint8_t>&); //NOLINT(google-explicit-constructor)
        [[nodiscard]] const char* data() const;
        [[nodiscard]] size_t size() const;
        operator std::vector<uint8_t>() const; //NOLINT(google-explicit-constructor)
        operator OwnedBinaryData() const; //NOLINT(google-explicit-constructor)
        operator BinaryData() const; //NOLINT(google-explicit-constructor)
        char operator[](size_t i) const noexcept;
    private:
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        storage::OwnedBinaryData m_data[1];
#else
        char* m_data;
        size_t m_size = 0;
#endif
    };

    bool operator ==(const binary& lhs, const binary& rhs);
    bool operator !=(const binary& lhs, const binary& rhs);
}

#endif //CPPREALM_BRIDGE_BINARY_HPP
