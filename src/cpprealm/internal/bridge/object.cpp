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

    object::object() {
        new (&m_object) Object();
    }
    object::object(const Object &v) {
        new (&m_object) Object(v);
    }
    object::object(const realm &realm, const struct obj &obj) {
        new (&m_object) Object(realm, obj);
    }
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
    object::operator Object() const {
        return *reinterpret_cast<const Object*>(m_object);
    }
    notification_token object::add_notification_callback(collection_change_callback &&v) {
        return reinterpret_cast<Object*>(m_object)->add_notification_callback(v);
    }
    bool index_set::empty() const {
        return reinterpret_cast<const IndexSet*>(m_idx_set)->empty();
    }
    index_set::index_set() {
        new (&m_idx_set) IndexSet();
    }
    index_set::index_set(const IndexSet& v) {
        new (&m_idx_set) IndexSet(v);
    }
    bool collection_change_set::empty() const {
        return reinterpret_cast<const CollectionChangeSet *>(m_change_set)->empty();
    }

    index_set collection_change_set::modifications() const {
        return reinterpret_cast<const CollectionChangeSet *>(m_change_set)->modifications;
    }

    std::unordered_map<int64_t, index_set> collection_change_set::columns() const {
        auto& columns = reinterpret_cast<const CollectionChangeSet *>(m_change_set)->columns;
        std::unordered_map<int64_t, index_set> map;
        for (const auto &[k, v]: columns) {
            map[k] = v;
        }
        return map;
    }

    index_set collection_change_set::deletions() const {
        return reinterpret_cast<const CollectionChangeSet *>(m_change_set)->deletions;
    }
    collection_change_callback::operator CollectionChangeCallback() const {
        return *reinterpret_cast<const CollectionChangeCallback*>(this);
    }
    notification_token::notification_token() {
        new (&m_token) NotificationToken();
    }
}