////////////////////////////////////////////////////////////////////////////
//
// Copyright 2021 Realm Inc.
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

#ifndef realm_results_hpp
#define realm_results_hpp

#include <any>

#include <cpprealm/type_info.hpp>
#include <realm/object-store/results.hpp>
#include <realm/query.hpp>
namespace realm {


template <type_info::ObjectPersistable T>
struct query : public T {
private:
    void set_managed(auto& prop, auto column_key) {
        prop.managed = column_key;
    }
public:
    query(Query& query, ObjectSchema&& schema) {
        std::apply([&](auto&&... props) {
            ((this->*props.ptr).prepare_for_query(query), ...);
            (set_managed((this->*props.ptr), schema.property_for_name(props.name)->column_key), ...);
        }, T::schema::properties);
    }
};

template <typename T>
struct results {
    class iterator {
    public:
        using difference_type = size_t;
        using value_type = T;
        using pointer = std::unique_ptr<value_type>;
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
            auto obj = m_parent->m_parent.template get<Obj>(m_idx);
            value = std::move(T::schema::create(std::move(obj), m_parent->m_parent.get_realm()));
            return value;
        }

        pointer operator->() const noexcept
        {
            auto obj = m_parent->m_parent.template get<Obj>(m_idx);
            return T::schema::create_unique(std::move(obj), m_parent->m_parent.get_realm());
        }

        iterator& operator++()
        {
            m_idx++;
            return *this;
        }

        iterator operator++(int i)
        {
            m_idx += i;
            return *this;
        }
    private:
        iterator(size_t idx, results<T>* parent)
        : m_idx(idx)
        , m_parent(parent)
        {
        }

        size_t m_idx;
        results<T>* m_parent;
        T value;

        template <typename>
        friend class results;
    };

    iterator begin()
    {
        return iterator(0, this);
    }

    iterator end()
    {
        return iterator(m_parent.size(), this);
    }

    size_t size()
    {
        return m_parent.size();
    }

    results& where(const std::string& query, std::vector<Mixed> arguments)
    {
        m_parent = realm::Results(m_parent.get_realm(), m_parent.get_table()->query(query,
                                                                                    std::move(arguments)));
        return *this;
    }
    results& where(std::function<rbool(T&)> fn)
    {
        auto builder = Query(m_parent.get_table());
        auto schema = *m_parent.get_realm()->schema().find(T::schema::name);
        auto q = query<T>(builder, std::move(schema));
        auto full_query = fn(q).q;
        m_parent = realm::Results(m_parent.get_realm(), full_query);
        return *this;
    }
private:
    template <type_info::ObjectPersistable...>
    friend struct db;
    results(realm::Results&& parent)
    : m_parent(std::move(parent))
    {
    }
    realm::Results m_parent;
};

}
#endif /* realm_results_hpp */
