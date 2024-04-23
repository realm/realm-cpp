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
#include <cpprealm/internal/bridge/utils.hpp>
#include <any>

namespace realm {
    namespace bson {
        class Bson;
        class BsonDocument;
        struct RegularExpression;
    }


    /**
     A struct representing a BSON value. BSON is a computer data interchange format.
     The name "BSON" is based on the term JSON and stands for "Binary JSON".

     The following types conform to BSON:

       int32_t
       int64_t
       bool
       double
       bsoncxx::min_key
       bsoncxx::max_key
       bsoncxx::timestamp
       bsoncxx::date
       decimal128
       object_id
       uuid
       bsoncxx::regular_expression
       std::vector<uint8_t>
       std::string
       char*;
       bsoncxx::document;
       std::vector<bsoncxx>

     @see bsonspec.org
     */
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
            using CoreDocument = realm::bson::BsonDocument;

            struct value {
                value& operator=(const bsoncxx& v);
                operator bsoncxx() const;
                const std::string key;
            private:
                friend struct bsoncxx;
                explicit value(CoreDocument* d, const std::string& k)
                    : key(k), m_document(d) {}
                CoreDocument* m_document;
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
                iterator(std::any&& i) : m_iterator(i) { }
                std::any m_iterator;
            };
            iterator begin();
            iterator end();

            document() noexcept;
            document(const std::initializer_list<std::pair<std::string, bsoncxx>>&) noexcept;
            document(const document&);
            document(document&&);
            ~document();
            document& operator=(const document&);
            document& operator=(document&&);
            void insert(const std::string& key, const bsoncxx& value);
            bool empty();
            size_t size() const;
            value operator[](const std::string&);
            operator CoreDocument() const;
            document(CoreDocument&) noexcept;
        private:
            friend struct bsoncxx;
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
            internal::bridge::storage::BsonIndexedMap m_document[1];
#else
            std::shared_ptr<CoreDocument> m_document;
#endif
        };

        /// MinKey will always be the smallest value when comparing to other BSON types
        struct min_key {};
        /// MaxKey will always be the greatest value when comparing to other BSON types
        struct max_key {};

        /// Provides regular expression capabilities for pattern matching strings in queries.
        /// MongoDB uses Perl compatible regular expressions (i.e. "PCRE") version 8.42 with UTF-8 support.
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
            option m_options;
            std::string m_pattern;
        };

        /// A type representing a BSON timestamp
        struct timestamp {
            explicit timestamp(uint32_t seconds, uint32_t increment) : m_seconds(seconds), m_increment(increment) {}
            uint32_t get_seconds() const noexcept { return m_seconds; }
            uint32_t get_increment() const noexcept { return m_increment; }
        private:
            friend struct bsoncxx;
            const uint32_t m_seconds;
            const uint32_t m_increment;
        };

        /// A type representing a BSON datetime
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

        /**
         Allowed BSON types.
        */
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
        bsoncxx(const array&) noexcept;
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
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        internal::bridge::storage::Bson m_bson[1];
#else
        std::shared_ptr<realm::bson::Bson> m_bson;
#endif
    };

    bool operator==(const bsoncxx& lhs, const bsoncxx& rhs);
    bool operator!=(const bsoncxx& lhs, const bsoncxx& rhs);
    bool operator==(const bsoncxx::document& lhs, const bsoncxx::document& rhs);
    bool operator!=(const bsoncxx::document& lhs, const bsoncxx::document& rhs);
    bool operator==(const bsoncxx::regular_expression& lhs, const bsoncxx::regular_expression& rhs) noexcept;
    bool operator!=(const bsoncxx::regular_expression& lhs, const bsoncxx::regular_expression& rhs) noexcept;
    bsoncxx::regular_expression::option operator|(const bsoncxx::regular_expression::option& lhs,
                                                  const bsoncxx::regular_expression::option& rhs) noexcept;
    bsoncxx::regular_expression::option operator&(const bsoncxx::regular_expression::option& lhs,
                                                  const bsoncxx::regular_expression::option& rhs) noexcept;
} // namespace realm

#endif//REALMCXX_BSON_HPP
