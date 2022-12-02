#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/internal/bridge/object.hpp>
#include <cpprealm/internal/bridge/object_schema.hpp>
#include <cpprealm/internal/bridge/realm.hpp>
#include <cpprealm/internal/bridge/utils.hpp>

#include <realm/object-store/object.hpp>

namespace realm::internal::bridge {
    static_assert(SizeCheck<104, sizeof(Object)>{});
    static_assert(SizeCheck<24, sizeof(IndexSet)>{});
    static_assert(SizeCheck<168, sizeof(CollectionChangeSet)>{});

    obj object::obj() const {
        return reinterpret_cast<const Object*>(m_object)->obj();
    }

    bool object::is_valid() const {
        return reinterpret_cast<const Object*>(m_object)->is_valid();
    }

    realm object::get_realm() const {
        return reinterpret_cast<const Object*>(m_object)->get_realm();
    }

    object_schema object::get_object_schema() const {
        return reinterpret_cast<const Object*>(m_object)->get_object_schema();
    }

    bool index_set::empty() const {
        return reinterpret_cast<const IndexSet*>(m_idx_set)->empty();
    }
}