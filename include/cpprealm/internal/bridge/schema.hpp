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

#ifndef CPPREALM_BRIDGE_SCHEMA_HPP
#define CPPREALM_BRIDGE_SCHEMA_HPP

#include <string>
#include <vector>
#include <cpprealm/internal/bridge/utils.hpp>

namespace realm {
    class Schema;
}
namespace realm::internal::bridge {
    struct object_schema;

    struct schema {
        schema();
        schema(const schema& other) ;
        schema& operator=(const schema& other) ;
        schema(schema&& other);
        schema& operator=(schema&& other);
        ~schema();
        schema(const std::vector<object_schema>&); //NOLINT(google-explicit-constructor)
        schema(const Schema&); //NOLINT(google-explicit-constructor)
        operator Schema() const; //NOLINT(google-explicit-constructor)
        object_schema find(const std::string &name);
    private:
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        storage::Schema m_schema[1];
#else
        std::shared_ptr<Schema> m_schema;
#endif
    };
}

#endif //CPPREALM_BRIDGE_SCHEMA_HPP
