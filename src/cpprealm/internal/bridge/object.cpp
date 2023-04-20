#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/internal/bridge/obj_key.hpp>
#include <cpprealm/internal/bridge/object.hpp>
#include <cpprealm/internal/bridge/object_schema.hpp>
#include <cpprealm/internal/bridge/realm.hpp>
#include <cpprealm/internal/bridge/utils.hpp>
#include <cpprealm/internal/bridge/list.hpp>
#include <cpprealm/internal/bridge/dictionary.hpp>

#include <realm/object-store/object.hpp>
#include <realm/object-store/list.hpp>
#include <realm/object-store/dictionary.hpp>

#include <memory>

namespace realm::internal::bridge {
#ifdef __i386__
    static_assert(SizeCheck<64, sizeof(Object)>{});
    static_assert(SizeCheck<4, alignof(Object)>{});
    static_assert(SizeCheck<12, sizeof(IndexSet)>{});
    static_assert(SizeCheck<4, alignof(IndexSet)>{});
    static_assert(SizeCheck<84, sizeof(CollectionChangeSet)>{});
    static_assert(SizeCheck<4, alignof(CollectionChangeSet)>{});
    static_assert(SizeCheck<16, sizeof(IndexSet::IndexIterator)>{});
    static_assert(SizeCheck<4, alignof(IndexSet::IndexIterator)>{});
    static_assert(SizeCheck<4, sizeof(IndexSet::IndexIteratableAdaptor)>{});
    static_assert(SizeCheck<4, alignof(IndexSet::IndexIteratableAdaptor)>{});
    static_assert(SizeCheck<16, sizeof(NotificationToken)>{});
    static_assert(SizeCheck<4, alignof(NotificationToken)>{});
#elif __x86_64__
    static_assert(SizeCheck<104, sizeof(Object)>{});
    static_assert(SizeCheck<8, alignof(Object)>{});
    static_assert(SizeCheck<24, sizeof(IndexSet)>{});
    static_assert(SizeCheck<8, alignof(IndexSet)>{});
    #if defined(__clang__)
    static_assert(SizeCheck<168, sizeof(CollectionChangeSet)>{});
    #elif defined(__GNUC__) || defined(__GNUG__)
    static_assert(SizeCheck<184, sizeof(CollectionChangeSet)>{});
    #endif
    static_assert(SizeCheck<8, alignof(CollectionChangeSet)>{});
    static_assert(SizeCheck<32, sizeof(IndexSet::IndexIterator)>{});
    static_assert(SizeCheck<8, alignof(IndexSet::IndexIterator)>{});
    static_assert(SizeCheck<8, sizeof(IndexSet::IndexIteratableAdaptor)>{});
    static_assert(SizeCheck<8, alignof(IndexSet::IndexIteratableAdaptor)>{});
    static_assert(SizeCheck<24, sizeof(NotificationToken)>{});
    static_assert(SizeCheck<8, alignof(NotificationToken)>{});
#elif __arm__
    static_assert(SizeCheck<80, sizeof(Object)>{});
    static_assert(SizeCheck<8, alignof(Object)>{});
    static_assert(SizeCheck<12, sizeof(IndexSet)>{});
    static_assert(SizeCheck<4, alignof(IndexSet)>{});
    static_assert(SizeCheck<84, sizeof(CollectionChangeSet)>{});
    static_assert(SizeCheck<4, alignof(CollectionChangeSet)>{});
    static_assert(SizeCheck<16, sizeof(IndexSet::IndexIterator)>{});
    static_assert(SizeCheck<4, alignof(IndexSet::IndexIterator)>{});
    static_assert(SizeCheck<4, sizeof(IndexSet::IndexIteratableAdaptor)>{});
    static_assert(SizeCheck<4, alignof(IndexSet::IndexIteratableAdaptor)>{});
    static_assert(SizeCheck<16, sizeof(NotificationToken)>{});
    static_assert(SizeCheck<8, alignof(NotificationToken)>{});
#elif __aarch64__
    static_assert(SizeCheck<104, sizeof(Object)>{});
    static_assert(SizeCheck<8, alignof(Object)>{});
    static_assert(SizeCheck<24, sizeof(IndexSet)>{});
    static_assert(SizeCheck<8, alignof(IndexSet)>{});
#if defined(__clang__)
    static_assert(SizeCheck<168, sizeof(CollectionChangeSet)>{});
#elif defined(__GNUC__) || defined(__GNUG__)
    static_assert(SizeCheck<184, sizeof(CollectionChangeSet)>{});
#endif
    static_assert(SizeCheck<8, alignof(CollectionChangeSet)>{});
    static_assert(SizeCheck<32, sizeof(IndexSet::IndexIterator)>{});
    static_assert(SizeCheck<8, alignof(IndexSet::IndexIterator)>{});
    static_assert(SizeCheck<8, sizeof(IndexSet::IndexIteratableAdaptor)>{});
    static_assert(SizeCheck<8, alignof(IndexSet::IndexIteratableAdaptor)>{});
    static_assert(SizeCheck<24, sizeof(NotificationToken)>{});
    static_assert(SizeCheck<8, alignof(NotificationToken)>{});
#endif

    object::object() {
        new (&m_object) Object();
    }
    object::object(const Object &v) {
        new (&m_object) Object(v);
    }
    object::object(const realm &realm, const struct obj &obj) {
        new (&m_object) Object(realm, obj);
    }
    object::object(const realm &realm, const struct obj_link& link) {
        new (&m_object) Object(realm, link);
    }
    obj object::get_obj() const {
        return reinterpret_cast<const Object*>(m_object)->obj();
    }
    list object::get_list(const col_key& col_key) const {
        return List(get_realm(), get_obj(), col_key);
    }
    dictionary object::get_dictionary(const col_key &v) const {
        return Dictionary(get_realm(), get_obj(), v);
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
    notification_token object::add_notification_callback(std::shared_ptr<collection_change_callback>&& cb) {
        struct wrapper : CollectionChangeCallback {
            std::shared_ptr<collection_change_callback> m_cb;
            explicit wrapper(std::shared_ptr<collection_change_callback>&& cb)
            : m_cb(std::move(cb)) {}
            void before(const CollectionChangeSet& v) const {
                m_cb->before(v);
            }
            void after(const CollectionChangeSet& v) const {
                m_cb->after(v);
            }
        } ccb(std::move(cb));
        return reinterpret_cast<Object*>(m_object)->add_notification_callback(ccb);
    }
    bool collection_change_set::empty() const {
        return m_change_set->empty();
    }

    collection_change_set::operator CollectionChangeSet() const {
        return *m_change_set;
    }

    static std::vector<uint64_t> to_vector(const IndexSet& index_set) {
        auto vector = std::vector<uint64_t>();
        for (auto index : index_set.as_indexes()) {
            vector.push_back(index);
        }
        return vector;
    };

    index_set collection_change_set::insertions() const {
        return to_vector(m_change_set->insertions);
    }

    index_set collection_change_set::modifications() const {
        return to_vector(m_change_set->modifications);
    }

    std::unordered_map<int64_t, index_set> collection_change_set::columns() const {
        auto& columns = m_change_set->columns;
        std::unordered_map<int64_t, index_set> map;
        for (const auto &[k, v]: columns) {
            map[k] = to_vector(v);
        }
        return map;
    }

    index_set collection_change_set::deletions() const {
        return to_vector(m_change_set->deletions);
    }

    notification_token::notification_token() {
        new (&m_token) NotificationToken();
    }

    notification_token::notification_token(NotificationToken &&v) {
        new (&m_token) NotificationToken(std::move(v));
    }
    void notification_token::unregister() {
        reinterpret_cast<NotificationToken *>(m_token)->unregister();
    }

    collection_change_set::collection_change_set(const CollectionChangeSet &v) {
        m_change_set = std::make_unique<CollectionChangeSet>(v);
    }
    bool collection_change_set::collection_root_was_deleted() const {
        return m_change_set->collection_root_was_deleted;
    }
}
