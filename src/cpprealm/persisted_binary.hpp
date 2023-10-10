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


#ifndef REALM_PERSISTED_BINARY_HPP
#define REALM_PERSISTED_BINARY_HPP

#include <cpprealm/persisted.hpp>
#include <sstream>

namespace realm {
    namespace internal::bridge {
        struct binary;
    }

    template <>
    struct persisted<std::vector<uint8_t>> : public persisted_primitive_base<std::vector<uint8_t>> {
        persisted()=default;
        persisted(const std::vector<uint8_t>& value) {
            new(&this->unmanaged) std::vector<uint8_t>(value);
        }
        persisted(std::vector<uint8_t>&& value) {
            new(&this->unmanaged) std::vector<uint8_t>(std::move(value));
        }

        /// Returns a reference to the element at specified location pos. No bounds checking is performed.
        uint8_t operator[](size_t idx);
        /// Appends the given element value to the end of the container.
        void push_back(uint8_t);
        /// Returns the number of elements in the container.
        [[nodiscard]] size_t size() const;

        static internal::bridge::binary serialize(const std::vector<uint8_t>& v, const std::optional<internal::bridge::realm>& = std::nullopt);
        static std::vector<uint8_t> deserialize(const internal::bridge::binary &v);
        __cpp_realm_friends
    };

    __cpp_realm_generate_operator(std::vector<uint8_t>, ==, equal)
    __cpp_realm_generate_operator(std::vector<uint8_t>, !=, not_equal)

    inline std::ostream& operator<< (std::ostream& stream, const persisted<std::vector<uint8_t>>& value)
    {
        stream << std::string("{ ");
        for (auto& c : *value) {
            stream << c << std::string(", ");
        }
        stream.seekp(-2);
        stream << std::string(" }");
        return stream;
    }
}

#endif //REALM_PERSISTED_BINARY_HPP
