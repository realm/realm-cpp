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

#ifndef REALM_PERSISTED_LIST_HPP
#define REALM_PERSISTED_LIST_HPP

#include <cpprealm/persisted.hpp>
#include <cpprealm/notifications.hpp>

namespace realm {

template <typename T>
class persisted_container_base : public persisted_base<T> {
public:
    using value_type = typename T::value_type;
    using size_type = typename T::size_type;

    class iterator {
    public:
        using difference_type = size_type;
        using pointer = value_type*;
        using reference = value_type&;
        using iterator_category = std::input_iterator_tag;

        bool operator!=(const iterator& other) const
        {
            return !(*this == other);
        }

        bool operator==(const iterator& other) const
        {
            return (m_parent == other.m_parent) && (m_idx == other.m_idx);
        }

        reference operator*() noexcept
        {
            value = (*m_parent)[m_idx];
            return value;
        }

        pointer operator->() const noexcept
        {
            value = (*m_parent)[m_idx];
            return &value;
        }

        iterator& operator++()
        {
            m_idx++;
            return *this;
        }

        iterator& operator++(int i)
        {
            m_idx += i;
            return *this;
        }
    private:
        friend struct persisted_container_base<T>;

        iterator(size_t idx, persisted_container_base<T>* parent)
                : m_idx(idx)
                , m_parent(parent)
        {
        }

        size_t m_idx;
        persisted_container_base<T>* m_parent;
        value_type value;
    };

    iterator begin()
    {
        return iterator(0, this);
    }

    iterator end()
    {
        return iterator(size(), this);
    }

    size_t size()
    {
        if (this->m_object) {
            return m_backing_list.size();
        } else {
            return this->unmanaged.size();
        }
    }

    void pop_back();
    void erase(size_type pos);
    void clear();
    virtual typename T::value_type operator[](typename T::size_type a) = 0;
    notification_token observe(std::function<void(collection_change<T>)>);

    /// Make this container property managed
    /// @param object The parent object
    /// @param col_key The column key for this property
    /// @param realm The Realm instance managing the parent.
    void assign(const Obj& object, const ColKey& col_key, SharedRealm realm) {
        this->m_object = Object(realm, object);
        this->managed = ColKey(col_key);
        m_backing_list = List(realm, object, this->managed);
    }

private:
    List m_backing_list;
    template <typename>
    friend class persisted_primitive_container_base;
    template <typename>
    friend class persisted_object_container_base;
};

template <typename T>
class persisted_primitive_container_base : public persisted_container_base<T> {
public:
    using persisted_container_base<T>::persisted_container_base;
    using value_type = typename T::value_type;
    using size_type = typename T::size_type;
    using typename persisted_container_base<T>::iterator;
    using persisted_container_base<T>::end;
    using persisted_container_base<T>::size;
    using persisted_container_base<T>::pop_back;
    using persisted_container_base<T>::erase;
    using persisted_container_base<T>::clear;

    void push_back(const typename T::value_type& a) {
        if (this->m_object) {
            this->m_backing_list.add(type_info::persisted_type<value_type>::convert_if_required(a));
        } else {
            this->unmanaged.push_back(a);
        }
    }
    void push_back(const typename T::value_type&& a) {
        if (this->m_object) {
            this->m_backing_list.add(type_info::persisted_type<value_type>::convert_if_required(std::move(a)));
        } else {
            this->unmanaged.push_back(std::move(a));
        }
    }
    void set(size_type pos, const typename T::value_type& a) {
        if (this->m_object) {
            auto as_core_type = static_cast<typename type_info::persisted_type<typename T::value_type>::type>(a);
            this->m_backing_list.set(pos, as_core_type);
        } else {
            this->unmanaged[pos] = a;
        }
    }
    size_t find(const typename T::value_type& a) {
        if (this->m_object) {
            return this->m_backing_list.find(type_info::persisted_type<value_type>::convert_if_required(a));
        } else {
            auto it = std::find(this->unmanaged.begin(), this->unmanaged.end(), a);
            if (it != this->unmanaged.end()) {
                return std::distance(this->unmanaged.begin(), it);
            } else {
                return realm::npos;
            }
        }
    }

    typename T::value_type operator[](typename T::size_type a) {
        if (this->m_object) {
            if constexpr (realm::type_info::BinaryPersistableConcept<typename T::value_type>::value) {
                auto binary_data = this->m_backing_list.template get<BinaryData>(a);
                return std::vector<uint8_t>(binary_data.data(), binary_data.data()+binary_data.size());
            } else if constexpr (type_info::MixedPersistableConcept<typename T::value_type>::value) {
                return type_info::mixed_to_variant<typename T::value_type>(this->m_backing_list.template get<typename type_info::persisted_type<typename T::value_type>::type>(a));
            } else {
                return static_cast<typename T::value_type>(
                        this->m_backing_list.template get<typename type_info::persisted_type<typename T::value_type>::type>(a));
            }
        } else {
            return this->unmanaged[a];
        }
    }
};
template <typename T>
class persisted_object_container_base : public persisted_container_base<T> {
public:
    using persisted_container_base<T>::persisted_container_base;
    using value_type = typename T::value_type;
    using size_type = typename T::size_type;
    using typename persisted_container_base<T>::iterator;
    using persisted_container_base<T>::end;
    using persisted_container_base<T>::size;
    using persisted_container_base<T>::pop_back;
    using persisted_container_base<T>::erase;
    using persisted_container_base<T>::clear;


    void push_back(typename T::value_type& a) {
        if (this->m_object) {
            if (!a.m_object) {
                if (this->m_object->obj().get_table()->get_link_target(this->managed)->is_embedded()) {
                    a.m_object = Object(this->m_backing_list.get_realm(),
                                        this->m_backing_list.add_embedded());
                    T::value_type::schema.set(a);
                } else {
                    T::value_type::schema.add(a, this->m_object->obj().get_table()->get_link_target(this->managed),
                                               this->m_backing_list.get_realm());
                }
            }
            if (!this->m_object->obj().get_table()->get_link_target(this->managed)->is_embedded()) {
                this->m_backing_list.add(a.m_object->obj().get_key());
            }
        } else {
            this->unmanaged.push_back(a);
        }
    }

    void push_back(typename T::value_type&& a) {
        if (this->m_object) {
            push_back(a);
        } else {
            this->unmanaged.push_back(std::move(a));
        }
    }

    void set(size_type pos, typename T::value_type& a) {
        if (this->m_obj) {
            if (!a.m_obj) {
                T::value_type::schema::add(a, this->m_obj->get_table()->get_link_target(this->managed), nullptr);
            }
            this->m_backing_list.set(pos, a.m_obj->get_key());
        } else {
            this->unmanaged[pos] = a;
        }
    }

    void set(size_type pos, typename T::value_type&& a) {
        if (this->m_obj) {
            set(pos, a);
        } else {
            this->unmanaged[pos] = std::move(a);
        }
    }

    size_t find(const typename T::value_type& a) {
            if (this->m_object && this->m_object->is_valid() && a.m_object && a.m_object->is_valid()) {
                return this->m_backing_list.find(a.m_object->obj().get_key());
            } else {
                // unmanaged objects in vectors aren't equatable.
                return realm::npos;
            }
    }
    typename T::value_type operator[](typename T::size_type idx) {
        if (this->m_object) {
            return T::value_type::schema.create(this->m_backing_list.get(idx), this->m_backing_list.get_realm());
        } else {
            return this->unmanaged[idx];
        }
    }
};


template <typename T>
void persisted_container_base<T>::pop_back() {
    if (this->m_object) {
        if (auto size = m_backing_list.size()) {
            m_backing_list.remove(size - 1);
        }
    } else {
        this->unmanaged.pop_back();
    }
}

template <typename T>
void persisted_container_base<T>::erase(size_type pos) {
    if (this->m_object) {
        m_backing_list.remove(pos);
    } else {
        this->unmanaged.erase(this->unmanaged.begin() + pos);
    }
}

template <typename T>
void persisted_container_base<T>::clear() {
    if (this->m_object) {
        m_backing_list.remove_all();
    } else {
        this->unmanaged.clear();
    }
}

template <typename T>
notification_token persisted_container_base<T>::observe(std::function<void(collection_change<T>)> handler)
{
    if (!this->m_object) {
        throw std::runtime_error("Only collections which are managed by a Realm support change notifications");
    }

    if (!this->m_object->is_valid()) {
        throw std::runtime_error("Only collections which are managed by a Realm support change notifications");
    }

    notification_token token;
    token.m_token = m_backing_list.add_notification_callback(collection_callback_wrapper<T> { std::move(handler), *static_cast<persisted<T>*>(this), false });
    return token;
}

    template <typename T>
    struct persisted<T, type_info::ListPrimitivePersistable<T>> : public persisted_primitive_container_base<T> {
        using persisted_primitive_container_base<T>::persisted_primitive_container_base;
        using persisted_primitive_container_base<T>::operator=;
        using value_type = typename T::value_type;
        using size_type = typename T::size_type;
        using typename persisted_primitive_container_base<T>::iterator;
        using persisted_primitive_container_base<T>::end;
        using persisted_primitive_container_base<T>::size;
        using persisted_primitive_container_base<T>::pop_back;
        using persisted_primitive_container_base<T>::erase;
        using persisted_primitive_container_base<T>::clear;
        using persisted_primitive_container_base<T>::push_back;
        using persisted_primitive_container_base<T>::set;
        using persisted_primitive_container_base<T>::find;
        using persisted_primitive_container_base<T>::operator[];
        using persisted_primitive_container_base<T>::observe;
    };

    template <typename T>
    struct persisted<T, type_info::ListObjectPersistable<T>> : public persisted_object_container_base<T> {
        using persisted_object_container_base<T>::persisted_object_container_base;
        using persisted_object_container_base<T>::operator=;
        using value_type = typename T::value_type;
        using size_type = typename T::size_type;
        using typename persisted_object_container_base<T>::iterator;
        using persisted_object_container_base<T>::end;
        using persisted_object_container_base<T>::size;
        using persisted_object_container_base<T>::pop_back;
        using persisted_object_container_base<T>::erase;
        using persisted_object_container_base<T>::clear;
        using persisted_object_container_base<T>::push_back;
        using persisted_object_container_base<T>::set;
        using persisted_object_container_base<T>::find;
        using persisted_object_container_base<T>::operator[];
        using persisted_object_container_base<T>::observe;
    };
    template <typename T>
    struct persisted<T, std::enable_if_t<
            std::is_same_v<std::vector<typename T::value_type>, T> &&
            type_info::MixedPersistableConcept<typename T::value_type>::value
    >> : public persisted_primitive_container_base<T> {
        using persisted_primitive_container_base<T>::persisted_primitive_container_base;
        using persisted_primitive_container_base<T>::operator=;
        using value_type = typename T::value_type;
        using size_type = typename T::size_type;
        using typename persisted_primitive_container_base<T>::iterator;
        using persisted_primitive_container_base<T>::end;
        using persisted_primitive_container_base<T>::size;
        using persisted_primitive_container_base<T>::pop_back;
        using persisted_primitive_container_base<T>::erase;
        using persisted_primitive_container_base<T>::clear;
        using persisted_primitive_container_base<T>::push_back;
        using persisted_primitive_container_base<T>::set;
        using persisted_primitive_container_base<T>::find;
        using persisted_primitive_container_base<T>::operator[];
        using persisted_primitive_container_base<T>::observe;
    };
}

#endif //REALM_PERSISTED_LIST_HPP
