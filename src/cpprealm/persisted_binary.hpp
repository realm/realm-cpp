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
    template<typename T>
    struct persisted<T, type_info::BinaryPersistable<T>> : public persisted_noncontainer_base<T> {
        using persisted_noncontainer_base<T>::persisted_noncontainer_base;
        using persisted_noncontainer_base<T>::operator*;
        using persisted_noncontainer_base<T>::operator=;
        typename T::value_type operator[](typename T::size_type idx) {
            if (this->m_object) {
                realm::BinaryData binary = this->m_object->obj().template get<BinaryData>(this->managed);
                return binary[idx];
            } else {
                return this->unmanaged[idx];
            }
        }
        void push_back(const typename T::value_type& a) {
            if (this->m_object) {
                std::string data = this->m_object->obj().template get<BinaryData>(this->managed).data();
                data = data + reinterpret_cast<char&>(a);
                auto binary = BinaryData(data);
                this->m_object->obj()
                        .template set<BinaryData>(this->managed, binary);
            } else {
                this->unmanaged.push_back(a);
            }
        }
        void push_back(typename T::value_type&& a) {
            if (this->m_object) {
                std::string data = this->m_object->obj().template get<BinaryData>(this->managed).data();
                data = data + reinterpret_cast<char&>(a);
                auto binary = BinaryData(data);
                this->m_object->obj()
                        .template set<BinaryData>(this->managed, binary);
            } else {
                this->unmanaged.push_back(a);
            }
        }
    };
}

#endif //REALM_PERSISTED_BINARY_HPP
