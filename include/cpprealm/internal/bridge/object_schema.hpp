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

#ifndef CPPREALM_BRIDGE_OBJECT_SCHEMA_HPP
#define CPPREALM_BRIDGE_OBJECT_SCHEMA_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <cpprealm/internal/bridge/utils.hpp>

namespace realm {
    class ObjectSchema;
}
namespace realm::internal::bridge {
    struct property;

    struct object_schema {
        enum class object_type : uint8_t { TopLevel = 0, Embedded = 0x1, TopLevelAsymmetric = 0x2 };

        object_schema();

        object_schema(const object_schema& other) ;
        object_schema& operator=(const object_schema& other) ;
        object_schema(object_schema&& other);
        object_schema& operator=(object_schema&& other);
        ~object_schema();


        object_schema(const std::string& name,
                      const std::vector<property>& properties,
                      const std::string& primary_key,
                      object_type type);
        object_schema(const ObjectSchema&);
        operator ObjectSchema() const;
        uint32_t table_key();
        void add_property(const property&);

        void set_name(const std::string& name);
        std::string get_name() const;
        void set_primary_key(const std::string& primary_key);
        void set_object_type(object_type);
        property property_for_name(const std::string&);
        bool operator==(const object_schema& rhs);
    private:
        ObjectSchema*  get_object_schema();
        const ObjectSchema*  get_object_schema() const;
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        storage::ObjectSchema m_schema[1];
#else
        std::shared_ptr<ObjectSchema> m_schema;
#endif
    };
}

#endif //CPPREALM_BRIDGE_OBJECT_SCHEMA_HPP
