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

#ifndef CPPREALM_RBOOL_HPP
#define CPPREALM_RBOOL_HPP

#include <cpprealm/internal/bridge/query.hpp>
#include <cpprealm/internal/bridge/realm.hpp>
#include <cpprealm/internal/bridge/table.hpp>
#include <cpprealm/internal/bridge/utils.hpp>
#include <cpprealm/types.hpp>

namespace realm {
    template<typename>
    struct box_base;
}

namespace realm {
    // MARK: rbool

    class rbool {
    public:
        struct dictionary_context {
            internal::bridge::col_key origin_col_key;
            std::string m_key;
        };

        rbool& add_link_chain(const internal::bridge::col_key& col_key) {
            if (m_link_chain) {
                m_link_chain->link(col_key);
            } else {
                m_link_chain = m_table.get_link(col_key);
            }
            return *this;
        }

#define define_query(type, comparison) \
    rbool& comparison(const internal::bridge::col_key& col_key, const std::optional<type>& rhs) { \
        if (auto lc = m_link_chain) { \
            q = lc->column<type>(col_key).comparison(rhs); \
            m_link_chain = std::nullopt; \
        } else if (m_dictionary_ctx) { \
            q = q.dictionary_link_subexpr(m_dictionary_ctx->origin_col_key, col_key, m_dictionary_ctx->m_key).comparison(rhs); \
            m_dictionary_ctx = std::nullopt; \
        } else {                       \
            if (rhs) {                 \
                q = internal::bridge::query(q.get_table()).comparison(col_key, *rhs); \
            } else {                   \
                q = internal::bridge::query(q.get_table()).comparison(col_key, std::nullopt); \
            }                          \
        } \
        return *this; \
    }                                  \
    rbool& comparison(const internal::bridge::col_key& col_key, const type& rhs) { \
        if (auto lc = m_link_chain) { \
            q = lc->column<type>(col_key).comparison(std::optional<type>(rhs)); \
            m_link_chain = std::nullopt; \
        } else if (m_dictionary_ctx) { \
            q = q.dictionary_link_subexpr(m_dictionary_ctx->origin_col_key, col_key, m_dictionary_ctx->m_key).comparison(std::optional<type>(rhs)); \
            m_dictionary_ctx = std::nullopt; \
        } else {                       \
            q = internal::bridge::query(q.get_table()).comparison(col_key, rhs); \
        } \
        return *this; \
    }

#define define_numeric_query(type, comparison) \
    rbool& comparison(const internal::bridge::col_key& col_key, const type& rhs) { \
        if (auto lc = m_link_chain) { \
            q = lc->column<type>(col_key).comparison(::realm::serialize(std::optional<type>(rhs))); \
            m_link_chain = std::nullopt; \
        } else if (m_dictionary_ctx) { \
            q = q.dictionary_link_subexpr(m_dictionary_ctx->origin_col_key, col_key, m_dictionary_ctx->m_key).comparison(std::optional<type>(rhs)); \
            m_dictionary_ctx = std::nullopt; \
        } else {                       \
            q = internal::bridge::query(q.get_table()).comparison(col_key, rhs); \
        } \
        return *this; \
    }

        define_query(std::string, equal)
        define_query(std::string, not_equal)

        rbool& contains(const internal::bridge::col_key& col_key, const std::string& rhs, bool case_sensitive = true) {
            if (auto lc = m_link_chain) {
                q = lc->column<std::string>(col_key).contains(::realm::serialize(std::optional<std::string>(rhs)), case_sensitive);
                m_link_chain = std::nullopt;
            } else {
                q = internal::bridge::query(q.get_table()).contains(col_key, rhs, case_sensitive);
            }
            return *this;
        }

        define_query(int64_t, equal)
        define_query(int64_t, not_equal)
        define_numeric_query(int64_t, greater)
        define_numeric_query(int64_t, less)
        define_numeric_query(int64_t, greater_equal)
        define_numeric_query(int64_t, less_equal)

        define_query(bool, equal)
        define_query(bool, not_equal)

        define_query(double, equal)
        define_query(double, not_equal)
        define_numeric_query(double, greater)
        define_numeric_query(double, less)
        define_numeric_query(double, greater_equal)
        define_numeric_query(double, less_equal)

        define_query(std::vector<uint8_t>, equal)
        define_query(std::vector<uint8_t>, not_equal)

        define_query(std::chrono::time_point<std::chrono::system_clock>, equal)
        define_query(std::chrono::time_point<std::chrono::system_clock>, not_equal)
        define_numeric_query(std::chrono::time_point<std::chrono::system_clock>, greater)
        define_numeric_query(std::chrono::time_point<std::chrono::system_clock>, less)
        define_numeric_query(std::chrono::time_point<std::chrono::system_clock>, greater_equal)
        define_numeric_query(std::chrono::time_point<std::chrono::system_clock>, less_equal)

        define_query(uuid, equal)
        define_query(uuid, not_equal)

        define_query(object_id, equal)
        define_query(object_id, not_equal)

        define_query(decimal128, equal)
        define_query(decimal128, not_equal)
        define_numeric_query(decimal128, greater)
        define_numeric_query(decimal128, less)
        define_numeric_query(decimal128, greater_equal)
        define_numeric_query(decimal128, less_equal)

        rbool& mixed_equal(const internal::bridge::col_key& col_key, const internal::bridge::mixed& rhs) {
            if (auto lc = m_link_chain) {
                q = lc->column<std::string>(col_key).mixed_equal(rhs);
                m_link_chain = std::nullopt;
            } else {
                q.equal(col_key, rhs);
            }
            return *this;
        }

        rbool& mixed_not_equal(const internal::bridge::col_key& col_key, const internal::bridge::mixed& rhs) {
            if (auto lc = m_link_chain) {
                q = lc->column_mixed(col_key).mixed_not_equal(rhs);
                m_link_chain = std::nullopt;
            } else {
                q.not_equal(col_key, rhs);
            }
            return *this;
        }

        rbool& link_equal(const internal::bridge::col_key& col_key, const std::optional<internal::bridge::obj>& rhs) {
            if (auto lc = m_link_chain) {
                q = lc->column<internal::bridge::obj>(col_key).equal(rhs);
                m_link_chain = std::nullopt;
            } else {
                if (rhs) {
                    q.links_to(col_key, *rhs);
                } else {
                    q.links_to(col_key, internal::bridge::obj());
                }
            }
            return *this;
        }

        rbool& link_not_equal(const internal::bridge::col_key& col_key, const std::optional<internal::bridge::obj>& rhs) {
            if (auto lc = m_link_chain) {
                q = lc->column<internal::bridge::obj>(col_key).not_equal(rhs);
                m_link_chain = std::nullopt;
            } else {
                if (rhs) {
                    q.not_links_to(col_key, *rhs);
                } else {
                    q.not_links_to(col_key, internal::bridge::obj());
                }
            }
            return *this;
        }

        // Dictionary

        rbool& dictionary_has_value_for_key_equals(internal::bridge::col_key column_key, const std::string& key, const internal::bridge::mixed& value) {
            q = internal::bridge::query(q.get_table()).dictionary_has_value_for_key_equals(column_key, key, value);
            return *this;
        }

        rbool& dictionary_has_value_for_key_not_equals(internal::bridge::col_key column_key, const std::string& key, const internal::bridge::mixed& value) {
            q = internal::bridge::query(q.get_table()).dictionary_has_value_for_key_not_equals(column_key, key, value);
            return *this;
        }

        rbool& dictionary_has_value_for_key_greater_than(internal::bridge::col_key column_key, const std::string& key, const internal::bridge::mixed& value) {
            q = internal::bridge::query(q.get_table()).dictionary_has_value_for_key_greater_than(column_key, key, value);
            return *this;
        }

        rbool& dictionary_has_value_for_key_less_than(internal::bridge::col_key column_key, const std::string& key, const internal::bridge::mixed& value) {
            q = internal::bridge::query(q.get_table()).dictionary_has_value_for_key_less_than(column_key, key, value);
            return *this;
        }

        rbool& dictionary_has_value_for_key_greater_than_equals(internal::bridge::col_key column_key, const std::string& key, const internal::bridge::mixed& value) {
            q = internal::bridge::query(q.get_table()).dictionary_has_value_for_key_greater_than_equals(column_key, key, value);
            return *this;
        }

        rbool& dictionary_has_value_for_key_less_than_equals(internal::bridge::col_key column_key, const std::string& key, const internal::bridge::mixed& value) {
            q = internal::bridge::query(q.get_table()).dictionary_has_value_for_key_less_than_equals(column_key, key, value);
            return *this;
        }

        rbool& dictionary_contains_string_for_key(internal::bridge::col_key column_key, const std::string& key, const std::string& value) {
            q = internal::bridge::query(q.get_table()).dictionary_contains_string_for_key(column_key, key, value);
            return *this;
        }

        rbool& add_dictionary_link_chain(dictionary_context&& ctx) {
            m_dictionary_ctx = ctx;
            return *this;
        }

        rbool& dictionary_has_key(internal::bridge::col_key column_key, const std::string& key) {
            q = internal::bridge::query(q.get_table()).dictionary_contains_key(column_key, key);
            return *this;
        }

        ~rbool() {
            if (is_for_queries)
                q.~query();
        }
        operator bool() const {
            return b;
        }
        rbool operator!() const {
            if (is_for_queries) {
                q.negate();
                return *this;
            }
            return !b;
        }
        union {
            bool b;
            mutable internal::bridge::query q;
        };

        rbool(internal::bridge::query &&q) : q(q), is_for_queries(true) {
            m_table = q.get_table();
        }
        rbool(bool b) : b(b) {}
        rbool(const rbool &r) {
            if (r.is_for_queries) {
                new (&q) internal::bridge::query(r.q);
                is_for_queries = true;
            } else
                b = r.b;
        }

    private:
        bool is_for_queries = false;
        bool is_dictionary_link = false;
        std::optional<internal::bridge::link_chain> m_link_chain;
        internal::bridge::table m_table;
        std::optional<dictionary_context> m_dictionary_ctx;

        template<typename T>
        friend struct results;
        friend rbool operator&&(const rbool &lhs, const rbool &rhs);
        friend rbool operator||(const rbool &lhs, const rbool &rhs);
    };

    inline rbool operator &&(const rbool& lhs, const rbool& rhs) {
        if (lhs.is_for_queries) {
            lhs.q.and_query(rhs.q);
            return lhs;
        }
        return lhs.b && rhs.b;
    }
    inline rbool operator ||(const rbool& lhs, const rbool& rhs) {
        if (lhs.is_for_queries) {
            lhs.q = lhs.q || rhs.q;
            return lhs;
        }
        return lhs.b && rhs.b;
    }

    /// Return all objects from a collection.
    template<typename T>
    inline rbool truepredicate(const T& o) {
        // An empty query returns all results and one way to indicate this
        // is to serialise TRUEPREDICATE which is functionally equivalent
        rbool* rb = internal::get_rbool(o);
        if (rb == nullptr)
            throw std::runtime_error("Managed object is not used in a query context");
        auto table = rb->q.get_table();
        return rbool(table);
    }

    /// Return no objects from a collection.
    template<typename T>
    inline rbool falsepredicate(const T& o) {
        rbool* rb = internal::get_rbool(o);
        if (rb == nullptr)
            throw std::runtime_error("Managed object is not used in a query context");
        auto table = rb->q.get_table();
        auto q = internal::bridge::query(table).and_query(internal::bridge::query(table).falsepredicate());
        return rbool(std::move(q));
    }
}

#endif //CPPREALM_RBOOL_HPP
