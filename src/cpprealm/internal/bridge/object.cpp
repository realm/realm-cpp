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
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
    static_assert(LayoutCheck<storage::Object, Object>{});
    static_assert(LayoutCheck<storage::IndexSet, IndexSet>{});
    static_assert(LayoutCheck<storage::CollectionChangeSet, CollectionChangeSet>{});
    static_assert(LayoutCheck<storage::IndexSet_IndexIterator, IndexSet::IndexIterator>{});
    static_assert(LayoutCheck<storage::IndexSet_IndexIteratableAdaptor, IndexSet::IndexIteratableAdaptor>{});
    static_assert(LayoutCheck<storage::NotificationToken, NotificationToken>{});
#elif __i386__
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
#elif _WIN32
    #if _DEBUG
    static_assert(SizeCheck<32, sizeof(IndexSet)>{});
    static_assert(SizeCheck<248, sizeof(CollectionChangeSet)>{});
    static_assert(SizeCheck<64, sizeof(IndexSet::IndexIterator)>{});
    #else
    static_assert(SizeCheck<24, sizeof(IndexSet)>{});
    static_assert(SizeCheck<192, sizeof(CollectionChangeSet)>{});
    static_assert(SizeCheck<32, sizeof(IndexSet::IndexIterator)>{});
    #endif

    static_assert(SizeCheck<104, sizeof(Object)>{});
    static_assert(SizeCheck<8, alignof(Object)>{});
    static_assert(SizeCheck<8, alignof(IndexSet)>{});
    static_assert(SizeCheck<8, alignof(CollectionChangeSet)>{});
    static_assert(SizeCheck<8, alignof(IndexSet::IndexIterator)>{});
    static_assert(SizeCheck<8, sizeof(IndexSet::IndexIteratableAdaptor)>{});
    static_assert(SizeCheck<8, alignof(IndexSet::IndexIteratableAdaptor)>{});
    static_assert(SizeCheck<24, sizeof(NotificationToken)>{});
    static_assert(SizeCheck<8, alignof(NotificationToken)>{});
#endif

    object::object() {
        new (&m_object) Object();
    }

    object::object(const object& other) {
        new (&m_object) Object(* reinterpret_cast<const Object*>(&other.m_object));
    }

    object& object::operator=(const object& other) {
        if (this != &other) {
            *reinterpret_cast<Object*>(&m_object) = *reinterpret_cast<const Object*>(&other.m_object);
        }
        return *this;
    }

    object::object(object&& other) {
        new (&m_object) Object(std::move(*reinterpret_cast<Object*>(&other.m_object)));
    }

    object& object::operator=(object&& other) {
        if (this != &other) {
            *reinterpret_cast<Object*>(&m_object) = std::move(*reinterpret_cast<Object*>(&other.m_object));
        }
        return *this;
    }

    object::~object() {
        reinterpret_cast<Object*>(&m_object)->~Object();
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
        return reinterpret_cast<const Object*>(&m_object)->get_obj();
    }
    list object::get_list(const col_key& col_key) const {
        return List(get_realm(), get_obj(), col_key);
    }
    dictionary object::get_dictionary(const col_key &v) const {
        return Dictionary(get_realm(), get_obj(), v);
    }
    bool object::is_valid() const {
        return reinterpret_cast<const Object*>(&m_object)->is_valid();
    }
    realm object::get_realm() const {
        return reinterpret_cast<const Object*>(&m_object)->get_realm();
    }
    object_schema object::get_object_schema() const {
        return reinterpret_cast<const Object*>(&m_object)->get_object_schema();
    }
    object::operator Object() const {
        return *reinterpret_cast<const Object*>(&m_object);
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
        return reinterpret_cast<Object*>(&m_object)->add_notification_callback(ccb);
    }

    bool index_set::empty() const {
        return reinterpret_cast<const IndexSet*>(&m_idx_set)->empty();
    }
    index_set::index_set() {
        new (&m_idx_set) IndexSet();
    }
    index_set::index_set(const index_set& other) {
        new (&m_idx_set) IndexSet(*reinterpret_cast<const IndexSet*>(&other.m_idx_set));
    }

    index_set& index_set::operator=(const index_set& other) {
        if (this != &other) {
            *reinterpret_cast<IndexSet*>(&m_idx_set) = *reinterpret_cast<const IndexSet*>(&other.m_idx_set);
        }
        return *this;
    }

    index_set::index_set(index_set&& other) {
        new (&m_idx_set) IndexSet(std::move(*reinterpret_cast<IndexSet*>(&other.m_idx_set)));
    }

    index_set& index_set::operator=(index_set&& other) {
        if (this != &other) {
            *reinterpret_cast<IndexSet*>(&m_idx_set) = std::move(*reinterpret_cast<IndexSet*>(&other.m_idx_set));
        }
        return *this;
    }

    index_set::~index_set() {
        reinterpret_cast<IndexSet*>(&m_idx_set)->~IndexSet();
    }
    index_set::index_set(const IndexSet& v) {
        new (&m_idx_set) IndexSet(v);
    }
    bool collection_change_set::empty() const {
        return reinterpret_cast<const CollectionChangeSet *>(&m_change_set)->empty();
    }

    collection_change_set::operator CollectionChangeSet() const {
        return *reinterpret_cast<const CollectionChangeSet *>(&m_change_set);
    }

    index_set collection_change_set::modifications() const {
        return reinterpret_cast<const CollectionChangeSet *>(&m_change_set)->modifications;
    }

    std::unordered_map<int64_t, index_set> collection_change_set::columns() const {
        auto& columns = reinterpret_cast<const CollectionChangeSet *>(&m_change_set)->columns;
        std::unordered_map<int64_t, index_set> map;
        for (const auto &[k, v]: columns) {
            map[k] = v;
        }
        return map;
    }

    index_set collection_change_set::deletions() const {
        return reinterpret_cast<const CollectionChangeSet *>(&m_change_set)->deletions;
    }

    notification_token::notification_token() {
        new (&m_token) NotificationToken();
    }

    notification_token::notification_token(notification_token&& other) {
        new (&m_token) NotificationToken(std::move(*reinterpret_cast<NotificationToken *>(&other.m_token)));
    }
    notification_token& notification_token::operator=(notification_token&& other) {
        if (this != &other) {
            *reinterpret_cast<NotificationToken *>(&m_token) = std::move(*reinterpret_cast<NotificationToken *>(&other.m_token));
        }
        return *this;
    }

    notification_token::notification_token(NotificationToken &&v) {
        new (&m_token) NotificationToken(std::move(v));
    }
    void notification_token::unregister() {
        reinterpret_cast<NotificationToken *>(&m_token)->unregister();
    }

    notification_token::~notification_token() {
        reinterpret_cast<NotificationToken*>(&m_token)->~NotificationToken();
    }

    collection_change_set::collection_change_set() {
        new (&m_change_set) CollectionChangeSet();
    }

    collection_change_set::collection_change_set(const collection_change_set& other) {
        new (&m_change_set) CollectionChangeSet(* reinterpret_cast<const CollectionChangeSet*>(&other.m_change_set));
    }

    collection_change_set& collection_change_set::operator=(const collection_change_set& other) {
        if (this != &other) {
            *reinterpret_cast<CollectionChangeSet*>(&m_change_set) = *reinterpret_cast<const CollectionChangeSet*>(&other.m_change_set);
        }
        return *this;
    }

    collection_change_set::collection_change_set(collection_change_set&& other) {
        new (&m_change_set) CollectionChangeSet(std::move(*reinterpret_cast<CollectionChangeSet*>(&other.m_change_set)));
    }

    collection_change_set& collection_change_set::operator=(collection_change_set&& other) {
        if (this != &other) {
            *reinterpret_cast<CollectionChangeSet*>(&m_change_set) = std::move(*reinterpret_cast<CollectionChangeSet*>(&other.m_change_set));
        }
        return *this;
    }

    collection_change_set::~collection_change_set() {
        reinterpret_cast<CollectionChangeSet*>(&m_change_set)->~CollectionChangeSet();
    }

    collection_change_set::collection_change_set(const CollectionChangeSet &v) {
        new (&m_change_set) CollectionChangeSet(v);
    }

    index_set::index_iterator::index_iterator(const index_set::index_iterator& other) {
        new (&m_iterator) IndexSet::IndexIterator(*reinterpret_cast<const IndexSet::IndexIterator*>(&other.m_iterator));
    }

    index_set::index_iterator& index_set::index_iterator::operator=(const index_set::index_iterator& other) {
        if (this != &other) {
            *reinterpret_cast<IndexSet::IndexIterator*>(&m_iterator) = *reinterpret_cast<const IndexSet::IndexIterator*>(&other.m_iterator);
        }
        return *this;
    }

    index_set::index_iterator::index_iterator(index_set::index_iterator&& other) {
        new (&m_iterator) IndexSet::IndexIterator(std::move(*reinterpret_cast<IndexSet::IndexIterator *>(&other.m_iterator)));
    }

    index_set::index_iterator& index_set::index_iterator::operator=(index_set::index_iterator&& other) {
        if (this != &other) {
            *reinterpret_cast<IndexSet::IndexIterator*>(&m_iterator) = std::move(*reinterpret_cast<IndexSet::IndexIterator*>(&other.m_iterator));
        }
        return *this;
    }

    index_set::index_iterator::~index_iterator() {
        reinterpret_cast<IndexSet::IndexIterator*>(&m_iterator)->~IndexIterator();
    }

    size_t index_set::index_iterator::operator*() const noexcept {
        return reinterpret_cast<const IndexSet::IndexIterator*>(&m_iterator)->operator*();
    }

    bool index_set::index_iterator::operator==(const index_set::index_iterator &it) const noexcept {
        return reinterpret_cast<const IndexSet::IndexIterator*>(&m_iterator)->operator==(
                *reinterpret_cast<const IndexSet::IndexIterator*>(it.m_iterator));
    }

    index_set::index_iterator &index_set::index_iterator::operator++() noexcept {
        reinterpret_cast<IndexSet::IndexIterator*>(&m_iterator)->operator++();
        return *this;
    }

    index_set collection_change_set::insertions() const {
        return reinterpret_cast<const CollectionChangeSet*>(&m_change_set)->insertions;
    }

    bool collection_change_set::collection_root_was_deleted() const {
        return reinterpret_cast<const CollectionChangeSet*>(&m_change_set)->collection_root_was_deleted;
    }

    bool index_set::index_iterator::operator!=(const index_set::index_iterator &it) const noexcept {
        return reinterpret_cast<const IndexSet::IndexIterator*>(&m_iterator)->operator!=(
                *reinterpret_cast<const IndexSet::IndexIterator*>(it.m_iterator));
    }

    index_set::index_iterable_adaptor::index_iterable_adaptor(const index_set::index_iterable_adaptor& other) {
        new (&m_index_iterable_adaptor) IndexSet::IndexIteratableAdaptor(*reinterpret_cast<const IndexSet::IndexIteratableAdaptor *>(&other.m_index_iterable_adaptor));
    }

    index_set::index_iterable_adaptor& index_set::index_iterable_adaptor::operator=(const index_set::index_iterable_adaptor& other) {
        new (&m_index_iterable_adaptor) IndexSet::IndexIteratableAdaptor(*reinterpret_cast<const IndexSet::IndexIteratableAdaptor *>(&other.m_index_iterable_adaptor));
        return *this;
    }

    index_set::index_iterable_adaptor::index_iterable_adaptor(index_set::index_iterable_adaptor&& other) {
        new (&m_index_iterable_adaptor) IndexSet::IndexIteratableAdaptor(std::move(*reinterpret_cast<IndexSet::IndexIteratableAdaptor *>(&other.m_index_iterable_adaptor)));
    }

    index_set::index_iterable_adaptor& index_set::index_iterable_adaptor::operator=(index_set::index_iterable_adaptor&& other) {
        new (&m_index_iterable_adaptor) IndexSet::IndexIteratableAdaptor(std::move(*reinterpret_cast<IndexSet::IndexIteratableAdaptor *>(&other.m_index_iterable_adaptor)));
        return *this;
    }

    index_set::index_iterable_adaptor::~index_iterable_adaptor() {
        reinterpret_cast<IndexSet::IndexIteratableAdaptor*>(&m_index_iterable_adaptor)->~IndexIteratableAdaptor();
    }

    index_set::index_iterator index_set::index_iterable_adaptor::begin() const noexcept {
        index_iterator iter;
        new (&iter.m_iterator) IndexSet::IndexIterator(reinterpret_cast<const IndexSet::IndexIteratableAdaptor *>(&m_index_iterable_adaptor)->begin());
        return iter;
    }
    index_set::index_iterator index_set::index_iterable_adaptor::end() const noexcept {
        index_iterator iter;
        new (&iter.m_iterator) IndexSet::IndexIterator(reinterpret_cast<const IndexSet::IndexIteratableAdaptor *>(&m_index_iterable_adaptor)->end());
        return iter;
    }

    index_set::index_iterable_adaptor index_set::as_indexes() const {
        index_iterable_adaptor iter;
        new (&iter.m_index_iterable_adaptor) IndexSet::IndexIteratableAdaptor(
                reinterpret_cast<const IndexSet*>(&m_idx_set)->as_indexes());
        return iter;
    }
}
