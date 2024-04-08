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

#ifndef CPPREALM_BRIDGE_COL_KEY_HPP
#define CPPREALM_BRIDGE_COL_KEY_HPP

#include <cinttypes>

namespace realm {
    struct ColKey;
}
namespace realm::internal::bridge {
    struct col_key {
        col_key() noexcept;
        col_key(const col_key& other) ;
        col_key& operator=(const col_key& other) ;
        col_key(col_key&& other);
        col_key& operator=(col_key&& other);
        ~col_key() = default;
        col_key(int64_t); //NOLINT(google-explicit-constructor)
        col_key(const ColKey&); //NOLINT(google-explicit-constructor)
        operator ColKey() const; //NOLINT(google-explicit-constructor)
        operator bool() const;
        [[nodiscard]] int64_t value() const;
    private:
        uint64_t m_col_key;
    };
}

#endif //CPPREALM_BRIDGE_COL_KEY_HPP
