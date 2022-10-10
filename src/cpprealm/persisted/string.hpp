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

#ifndef REALM_PERSISTED_STRING_HPP
#define REALM_PERSISTED_STRING_HPP

#include <cpprealm/persisted.hpp>

namespace realm {
    template<typename T>
    struct persisted<T, type_info::StringPersistable<T>> : public persisted_noncontainer_base<T> {
        using persisted_noncontainer_base<T>::persisted_noncontainer_base;
        using persisted_noncontainer_base<T>::operator*;
        using persisted_noncontainer_base<T>::operator=;

        persisted(const char* value) {
            new (&this->unmanaged) std::string(value);
        }
        persisted& operator=(const char* o) {
            if (auto obj = persisted_noncontainer_base<T>::m_object) {
                std::string convert = type_info::persisted_type<T>::convert_if_required(T(o));
                obj->obj().template set<typename persisted_noncontainer_base<T>::type>(persisted_noncontainer_base<T>::managed, convert);
            } else {
                persisted_noncontainer_base<T>::unmanaged = o;
            }
            return *this;
        }

        rbool contains(const char *str) {
            if (this->should_detect_usage_for_queries) {
                auto query = Query(this->query->get_table());
                query.contains(this->managed, StringData(str));
                return {std::move(query)};
            } else if (auto m_obj = this->m_object) {
                StringData actual = m_obj->obj().template get<StringData>(this->managed);
                return actual.contains(str);
            } else {
                return type_info::persisted_type<T>::convert_if_required(this->unmanaged).contains(str);
            }
        }
    };
}

#endif //REALM_PERSISTED_STRING_HPP
