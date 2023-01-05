#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/internal/bridge/object.hpp>
#include <cpprealm/internal/bridge/object_schema.hpp>
#include <cpprealm/internal/bridge/realm.hpp>
#include <cpprealm/internal/bridge/utils.hpp>
#include <cpprealm/internal/bridge/list.hpp>
#include <cpprealm/internal/bridge/dictionary.hpp>

#include <realm/object-store/object.hpp>
#include <realm/object-store/list.hpp>
#include <realm/object-store/dictionary.hpp>

namespace realm::internal::bridge {
    static_assert(SizeCheck<104, sizeof(Object)>{});
    static_assert(SizeCheck<24, sizeof(IndexSet)>{});
    static_assert(SizeCheck<168, sizeof(CollectionChangeSet)>{});
    static_assert(SizeCheck<32, sizeof(IndexSet::IndexIterator)>{});
    static_assert(SizeCheck<8, sizeof(IndexSet::IndexIteratableAdaptor)>{});
    static_assert(SizeCheck<24, sizeof(NotificationToken)>{});
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

    list object::get_list(const col_key& col_key) const {
        return List(get_realm(), obj(), col_key);
    }
    dictionary object::get_dictionary(const col_key &v) const {
        return Dictionary(get_realm(), obj(), v);
    }
    object object::freeze(internal::bridge::realm v) const {
        return reinterpret_cast<const Object*>(m_object)->freeze(v);
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
    notification_token object::add_notification_callback(const std::shared_ptr<collection_change_callback>& cb) {
        struct wrapper : CollectionChangeCallback {
            const std::shared_ptr<collection_change_callback> m_cb;
            explicit wrapper(const std::shared_ptr<collection_change_callback>& cb)
                    : m_cb(cb) {}
            void before(const CollectionChangeSet& v) const {
                m_cb->before(v);
            }
            void after(const CollectionChangeSet& v) const {
                m_cb->after(v);
            }
        } ccb(cb);
        return reinterpret_cast<Object*>(m_object)->add_notification_callback(ccb);
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

    collection_change_set::operator CollectionChangeSet() const {
        return *reinterpret_cast<const CollectionChangeSet *>(m_change_set);
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

    notification_token::notification_token() {
        new (&m_token) NotificationToken();
    }

    notification_token::notification_token(NotificationToken &&v) {
        new (&m_token) NotificationToken(std::move(v));
    }

    collection_change_set::collection_change_set(const CollectionChangeSet &v) {
        new (&m_change_set) CollectionChangeSet(v);
    }

    size_t index_set::index_iterator::operator*() const noexcept {
        return reinterpret_cast<const IndexSet::IndexIterator*>(m_iterator)->operator*();
    }

    bool index_set::index_iterator::operator==(const index_set::index_iterator &it) const noexcept {
        return reinterpret_cast<const IndexSet::IndexIterator*>(m_iterator)->operator==(
                *reinterpret_cast<const IndexSet::IndexIterator*>(it.m_iterator));
    }

    index_set::index_iterator &index_set::index_iterator::operator++() noexcept {
        new (&m_iterator) IndexSet::IndexIterator(reinterpret_cast<IndexSet::IndexIterator*>(m_iterator)->operator++());
        return *this;
    }

    index_set collection_change_set::insertions() const {
        return reinterpret_cast<const CollectionChangeSet*>(m_change_set)->insertions;
    }

    bool collection_change_set::collection_root_was_deleted() const {
        return reinterpret_cast<const CollectionChangeSet*>(m_change_set)->collection_root_was_deleted;
    }

    bool index_set::index_iterator::operator!=(const index_set::index_iterator &it) const noexcept {
        return reinterpret_cast<const IndexSet::IndexIterator*>(m_iterator)->operator!=(
                *reinterpret_cast<const IndexSet::IndexIterator*>(it.m_iterator));
    }

    index_set::index_iterator index_set::index_iterable_adaptor::begin() const noexcept {
        index_iterator iter;
        new (&iter.m_iterator) IndexSet::IndexIterator(
                reinterpret_cast<const IndexSet::IndexIteratableAdaptor*>(index_iterable_adaptor)->begin());
        return iter;
    }
    index_set::index_iterator index_set::index_iterable_adaptor::end() const noexcept {
        index_iterator iter;
        new (&iter.m_iterator) IndexSet::IndexIterator(
                reinterpret_cast<const IndexSet::IndexIteratableAdaptor*>(index_iterable_adaptor)->end());
        return iter;
    }

    index_set::index_iterable_adaptor index_set::as_indexes() const {
        index_iterable_adaptor iter;
        new (&iter.index_iterable_adaptor) IndexSet::IndexIteratableAdaptor(
                reinterpret_cast<const IndexSet*>(m_idx_set)->as_indexes());
        return iter;
    }
}