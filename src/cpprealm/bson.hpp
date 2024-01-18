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

#ifndef REALMCXX_BSON_HPP
#define REALMCXX_BSON_HPP

#include <cpprealm/types.hpp>

namespace realm {
    namespace bson {
        class Bson;
        template <typename>
        class IndexedMap;
        class RegularExpression;
    }

    struct bsoncxx {

        using array = std::vector<bsoncxx>;

        enum class type {
            b_null,
            b_int32,
            b_int64,
            b_bool,
            b_double,
            b_string,
            b_binary,
            b_timestamp,
            b_datetime,
            b_objectId,
            b_decimal128,
            b_regular_expression,
            b_max_key,
            b_min_key,
            b_document,
            b_array,
            b_uuid
        };

        struct document {
            using CoreDocument = realm::bson::IndexedMap<realm::bson::Bson>;

            struct value {
                value& operator=(const bsoncxx& v);
                operator bsoncxx() const;
                const std::string key;
            private:
                friend struct bsoncxx;
                explicit value(std::shared_ptr<CoreDocument>& d, const std::string& k)
                    : m_document(d), key(k) {};
                std::shared_ptr<CoreDocument> m_document;
            };

            struct iterator {
                std::pair<std::string, bsoncxx> operator*();
                iterator& operator++();
                iterator& operator--();
                iterator& operator++(int);
                iterator& operator--(int);
                bool operator!=(const iterator& rhs) const noexcept;
                bool operator==(const iterator& rhs) const noexcept;
            private:
                friend struct document;
                iterator(const std::shared_ptr<CoreDocument>& d, size_t i) : m_document(d), m_idx(i) {};
                size_t m_idx = 0;
                std::shared_ptr<CoreDocument> m_document;
            };
            iterator begin();
            iterator end();

            document() noexcept;
            document(const std::vector<std::pair<std::string, bsoncxx>>&) noexcept;
            document(const document&);
            document(document&&);
            ~document();
            document& operator=(const document&);
            document& operator=(document&&);
            void insert(const std::string& key, const bsoncxx& value);
            void pop_back();
            bool empty() const;
            size_t size() const;
            value operator[](const std::string&);
            operator CoreDocument() const;
            document(CoreDocument&) noexcept;
        private:
            friend struct bsoncxx;
            std::shared_ptr<CoreDocument> m_document;
        };

        struct min_key {};
        struct max_key {};
        struct regular_expression {
            enum class option : uint8_t {
                none,
                ignore_case = 1,
                multiline = 2,
                dotall = 4,
                extended = 8
            };
            regular_expression(const std::string& pattern, const std::string& options);
            regular_expression(const std::string& pattern, option options);
            regular_expression();
            option get_options() const;
            std::string get_pattern() const;
            operator realm::bson::RegularExpression() const;
        private:
            friend struct bsoncxx;
            regular_expression(const realm::bson::RegularExpression&);
            std::shared_ptr<realm::bson::RegularExpression> m_reg_expr;
        };

        struct timestamp {
            explicit timestamp(uint32_t seconds, uint32_t increment) : m_seconds(seconds), m_increment(increment) {}
            uint32_t get_seconds() const noexcept { return m_seconds; }
            uint32_t get_increment() const noexcept { return m_increment; }
        private:
            friend struct bsoncxx;
            const uint32_t m_seconds;
            const uint32_t m_increment;
        };
        struct date {
            explicit date(const std::chrono::time_point<std::chrono::system_clock>& d) : m_date(d) {}
            std::chrono::time_point<std::chrono::system_clock> get_date() const noexcept { return m_date; }
        private:
            friend struct bsoncxx;
            const std::chrono::time_point<std::chrono::system_clock> m_date;
        };

        bsoncxx() noexcept;
        ~bsoncxx() noexcept;
        bsoncxx(const bsoncxx&) noexcept;
        bsoncxx(bsoncxx&&) noexcept;
        bsoncxx& operator=(const bsoncxx&) noexcept;
        bsoncxx& operator=(bsoncxx&&) noexcept;

        bsoncxx(int32_t) noexcept;
        bsoncxx(int64_t) noexcept;
        bsoncxx(bool) noexcept;
        bsoncxx(double) noexcept;
        bsoncxx(min_key) noexcept;
        bsoncxx(max_key) noexcept;
        bsoncxx(const timestamp&) noexcept;
        bsoncxx(const date&) noexcept;
        bsoncxx(const decimal128&) noexcept;
        bsoncxx(const object_id&) noexcept;
        bsoncxx(const uuid&) noexcept;
        bsoncxx(const regular_expression&) noexcept;
        bsoncxx(const std::vector<uint8_t>&) noexcept;
        bsoncxx(const std::string&) noexcept;
        bsoncxx(const char*) noexcept;
        bsoncxx(const document&) noexcept;
        bsoncxx(const std::vector<bsoncxx>&) noexcept;
        type get_type() const;
        operator realm::bson::Bson() const;

        operator std::nullopt_t() const;
        operator int32_t() const;
        operator int64_t() const;
        operator bool() const;
        operator double() const;
        operator min_key() const;
        operator max_key() const;
        operator timestamp() const;
        operator date() const;
        operator decimal128() const;
        operator object_id() const;
        operator uuid() const;
        operator regular_expression() const;
        operator std::vector<uint8_t>() const;
        operator std::string() const;
        operator document() const;
        operator array() const;

        std::string to_string() const;
        std::string to_json() const;
        bsoncxx(realm::bson::Bson&) noexcept;
    private:
        std::shared_ptr<realm::bson::Bson> m_bson;
    };

    bool operator==(const bsoncxx& lhs, const bsoncxx& rhs);
    bool operator!=(const bsoncxx& lhs, const bsoncxx& rhs);
    bool operator==(const bsoncxx::regular_expression& lhs, const bsoncxx::regular_expression& rhs) noexcept;
    bool operator!=(const bsoncxx::regular_expression& lhs, const bsoncxx::regular_expression& rhs) noexcept;
    bsoncxx::regular_expression::option operator|(const bsoncxx::regular_expression::option& lhs,
                                                  const bsoncxx::regular_expression::option& rhs) noexcept;
    bsoncxx::regular_expression::option operator&(const bsoncxx::regular_expression::option& lhs,
                                                  const bsoncxx::regular_expression::option& rhs) noexcept;
} // namespace realm

#endif//REALMCXX_BSON_HPP
