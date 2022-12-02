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

namespace realm {
    template <>
    struct persisted<std::vector<uint8_t>> : public persisted_base<std::vector<uint8_t>> {
        persisted& operator=(const std::vector<uint8_t>&);
        uint8_t operator[](size_t idx);
        void push_back(uint8_t);
    };
}

#endif //REALM_PERSISTED_BINARY_HPP
