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
            document() noexcept;
            document(const std::vector<std::pair<std::string, bsoncxx>>&) noexcept;
            document(const document&);
            document(document&&);
            ~document();
            document& operator=(const document&);
            document& operator=(document&&);

            using CoreDocument = realm::bson::IndexedMap<realm::bson::Bson>;
            operator CoreDocument() const;
        private:
            std::shared_ptr<CoreDocument> m_document;
        };

        struct min_key {};
        struct max_key {};
        struct regular_expression {
            operator realm::bson::RegularExpression() const;
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
        bsoncxx(const std::chrono::time_point<std::chrono::system_clock>&) noexcept;
//        bsoncxx(realm::Timestamp) noexcept;
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
    private:
        std::shared_ptr<realm::bson::Bson> m_bson;
    };
} // namespace realm

#endif//REALMCXX_BSON_HPP
