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


#ifndef REALM_PERSISTED_DOUBLE_HPP
#define REALM_PERSISTED_DOUBLE_HPP

#include <cpprealm/persisted.hpp>

namespace realm {
    template<typename T>
    struct persisted<T, type_info::DoublePersistable<T>> : public persisted_add_assignable_base<T> {
        using persisted_add_assignable_base<T>::persisted_add_assignable_base;
        using persisted_add_assignable_base<T>::operator*;
        using persisted_add_assignable_base<T>::operator=;
        using persisted_add_assignable_base<T>::operator+;
        using persisted_add_assignable_base<T>::operator++;
        using persisted_add_assignable_base<T>::operator+=;
        using persisted_add_assignable_base<T>::operator-;
        using persisted_add_assignable_base<T>::operator-=;
        using persisted_add_assignable_base<T>::operator--;
    };
}

#endif //REALM_PERSISTED_DOUBLE_HPP
