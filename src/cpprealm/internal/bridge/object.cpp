#include <cpprealm/internal/bridge/obj.hpp>

#include <cpprealm/internal/bridge/dictionary.hpp>
#include <cpprealm/internal/bridge/list.hpp>
#include <cpprealm/internal/bridge/obj_key.hpp>
#include <cpprealm/internal/bridge/object.hpp>
#include <cpprealm/internal/bridge/object_schema.hpp>
#include <cpprealm/internal/bridge/realm.hpp>

#include <realm/object-store/dictionary.hpp>
#include <realm/object-store/list.hpp>
#include <realm/object-store/object.hpp>

namespace realm::internal::bridge {
    object::object() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_object) Object();
#else
        m_object = std::make_shared<Object>();
#endif
    }

    object::object(const object& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_object) Object(* reinterpret_cast<const Object*>(&other.m_object));
#else
        m_object = other.m_object;
#endif
    }

    object& object::operator=(const object& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<Object*>(&m_object) = *reinterpret_cast<const Object*>(&other.m_object);
        }
#else
        m_object = other.m_object;
#endif
        return *this;
    }

    object::object(object&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_object) Object(std::move(*reinterpret_cast<Object*>(&other.m_object)));
#else
        m_object = std::move(other.m_object);
#endif
    }

    object& object::operator=(object&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<Object*>(&m_object) = std::move(*reinterpret_cast<Object*>(&other.m_object));
        }
#else
        m_object = std::move(other.m_object);
#endif
        return *this;
    }

    object::~object() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<Object*>(&m_object)->~Object();
#endif
    }

    object::object(const Object &v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_object) Object(v);
#else
        m_object = std::make_shared<Object>(v);
#endif
    }
    object::object(const realm &realm, const struct obj &obj) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_object) Object(realm, obj);
#else
        m_object = std::make_shared<Object>(realm, obj);
#endif
    }
    object::object(const realm &realm, const struct obj_link& link) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_object) Object(realm, link);
#else
        m_object = std::make_shared<Object>(realm, link);
#endif
    }
    obj object::get_obj() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const Object*>(&m_object)->get_obj();
#else
        return m_object->get_obj();
#endif
    }
    list object::get_list(const col_key& col_key) const {
        return List(get_realm(), get_obj(), col_key);
    }
    dictionary object::get_dictionary(const col_key &v) const {
        return Dictionary(get_realm(), get_obj(), v);
    }
    bool object::is_valid() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const Object*>(&m_object)->is_valid();
#else
        return m_object->is_valid();
#endif
    }
    realm object::get_realm() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const Object*>(&m_object)->get_realm();
#else
        return m_object->get_realm();
#endif
    }
    object_schema object::get_object_schema() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const Object*>(&m_object)->get_object_schema();
#else
        return m_object->get_object_schema();
#endif
    }
    object::operator Object() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return *reinterpret_cast<const Object*>(&m_object);
#else
        return *m_object;
#endif
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
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<Object*>(&m_object)->add_notification_callback(ccb);
#else
        return m_object->add_notification_callback(ccb);
#endif
    }

    bool index_set::empty() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const IndexSet*>(&m_idx_set)->empty();
#else
        return m_idx_set->empty();
#endif
    }
    index_set::index_set() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_idx_set) IndexSet();
#else
        m_idx_set = std::make_shared<IndexSet>();
#endif
    }
    index_set::index_set(const index_set& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_idx_set) IndexSet(*reinterpret_cast<const IndexSet*>(&other.m_idx_set));
#else
        m_idx_set = other.m_idx_set;
#endif
    }

    index_set& index_set::operator=(const index_set& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<IndexSet*>(&m_idx_set) = *reinterpret_cast<const IndexSet*>(&other.m_idx_set);
        }
#else
        m_idx_set = other.m_idx_set;
#endif
        return *this;
    }

    index_set::index_set(index_set&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_idx_set) IndexSet(std::move(*reinterpret_cast<IndexSet*>(&other.m_idx_set)));
#else
        m_idx_set = std::move(other.m_idx_set);
#endif
    }

    index_set& index_set::operator=(index_set&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<IndexSet*>(&m_idx_set) = std::move(*reinterpret_cast<IndexSet*>(&other.m_idx_set));
        }
#else
        m_idx_set = std::move(other.m_idx_set);
#endif
        return *this;
    }

    index_set::~index_set() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<IndexSet*>(&m_idx_set)->~IndexSet();
#endif
    }
    index_set::index_set(const IndexSet& v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_idx_set) IndexSet(v);
#else
        m_idx_set = std::make_shared<IndexSet>(v);
#endif
    }
    bool collection_change_set::empty() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const CollectionChangeSet *>(&m_change_set)->empty();
#else
        return m_change_set->empty();
#endif
    }

    collection_change_set::operator CollectionChangeSet() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return *reinterpret_cast<const CollectionChangeSet *>(&m_change_set);
#else
        return *m_change_set;
#endif
    }

    index_set collection_change_set::modifications() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const CollectionChangeSet *>(&m_change_set)->modifications;
#else
        return m_change_set->modifications;
#endif
    }

    std::unordered_map<int64_t, index_set> collection_change_set::columns() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        auto& columns = reinterpret_cast<const CollectionChangeSet *>(&m_change_set)->columns;
#else
        auto& columns = m_change_set->columns;
#endif
        std::unordered_map<int64_t, index_set> map;
        for (const auto &[k, v]: columns) {
            map[k] = v;
        }
        return map;
    }

    index_set collection_change_set::deletions() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const CollectionChangeSet *>(&m_change_set)->deletions;
#else
        return m_change_set->deletions;
#endif
    }

    notification_token::notification_token() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_token) NotificationToken();
#else
        m_token = std::make_shared<NotificationToken>();
#endif
    }

    notification_token::notification_token(notification_token&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_token) NotificationToken(std::move(*reinterpret_cast<NotificationToken *>(&other.m_token)));
#else
        m_token = std::move(other.m_token);
#endif
    }
    notification_token& notification_token::operator=(notification_token&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<NotificationToken *>(&m_token) = std::move(*reinterpret_cast<NotificationToken *>(&other.m_token));
        }
#else
        m_token = std::move(other.m_token);
#endif
        return *this;
    }

    notification_token::notification_token(NotificationToken &&v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_token) NotificationToken(std::move(v));
#else
        m_token = std::make_shared<NotificationToken>(std::move(v));
#endif
    }
    void notification_token::unregister() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<NotificationToken *>(&m_token)->unregister();
#else
        m_token->unregister();
#endif
    }

    notification_token::~notification_token() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<NotificationToken*>(&m_token)->~NotificationToken();
#endif
    }

    collection_change_set::collection_change_set() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_change_set) CollectionChangeSet();
#else
        m_change_set = std::make_shared<CollectionChangeSet>();
#endif
    }

    collection_change_set::collection_change_set(const collection_change_set& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_change_set) CollectionChangeSet(* reinterpret_cast<const CollectionChangeSet*>(&other.m_change_set));

#else
        m_change_set = std::make_shared<CollectionChangeSet>(other);
#endif
    }

    collection_change_set& collection_change_set::operator=(const collection_change_set& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<CollectionChangeSet*>(&m_change_set) = *reinterpret_cast<const CollectionChangeSet*>(&other.m_change_set);
        }
#else
        m_change_set = std::make_shared<CollectionChangeSet>(other);
#endif
        return *this;
    }

    collection_change_set::collection_change_set(collection_change_set&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_change_set) CollectionChangeSet(std::move(*reinterpret_cast<CollectionChangeSet*>(&other.m_change_set)));
#else
        m_change_set = std::make_shared<CollectionChangeSet>(std::move(other));
#endif
    }

    collection_change_set& collection_change_set::operator=(collection_change_set&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<CollectionChangeSet*>(&m_change_set) = std::move(*reinterpret_cast<CollectionChangeSet*>(&other.m_change_set));
        }
#else
        m_change_set = std::make_shared<CollectionChangeSet>(std::move(other));
#endif
        return *this;
    }

    collection_change_set::~collection_change_set() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<CollectionChangeSet*>(&m_change_set)->~CollectionChangeSet();
#endif
    }

    collection_change_set::collection_change_set(const CollectionChangeSet &v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_change_set) CollectionChangeSet(v);
#else
        m_change_set = std::make_shared<CollectionChangeSet>(v);
#endif
    }

    index_set::index_iterator::index_iterator(const index_set::index_iterator& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_iterator) IndexSet::IndexIterator(*reinterpret_cast<const IndexSet::IndexIterator*>(&other.m_iterator));
#else
        m_iterator = std::make_shared<std::any>(other.m_iterator);
#endif
    }

    index_set::index_iterator& index_set::index_iterator::operator=(const index_set::index_iterator& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<IndexSet::IndexIterator*>(&m_iterator) = *reinterpret_cast<const IndexSet::IndexIterator*>(&other.m_iterator);
        }
#else
        m_iterator = std::make_shared<std::any>(other.m_iterator);
#endif
        return *this;
    }

    index_set::index_iterator::index_iterator(index_set::index_iterator&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_iterator) IndexSet::IndexIterator(std::move(*reinterpret_cast<IndexSet::IndexIterator *>(&other.m_iterator)));
#else
        m_iterator = std::make_shared<std::any>(std::move(other.m_iterator));
#endif
    }

    index_set::index_iterator& index_set::index_iterator::operator=(index_set::index_iterator&& other) {

#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<IndexSet::IndexIterator*>(&m_iterator) = std::move(*reinterpret_cast<IndexSet::IndexIterator*>(&other.m_iterator));
        }
#else
        m_iterator = std::make_shared<std::any>(std::move(other.m_iterator));
#endif
        return *this;
    }

    index_set::index_iterator::~index_iterator() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<IndexSet::IndexIterator*>(&m_iterator)->~IndexIterator();
#endif
    }

    size_t index_set::index_iterator::operator*() const noexcept {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const IndexSet::IndexIterator*>(&m_iterator)->operator*();
#else
        return std::any_cast<IndexSet::IndexIterator>(*m_iterator).operator*();
#endif
    }

    bool index_set::index_iterator::operator==(const index_set::index_iterator &it) const noexcept {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const IndexSet::IndexIterator*>(&m_iterator)->operator==(
                *reinterpret_cast<const IndexSet::IndexIterator*>(it.m_iterator));
#else
        return std::any_cast<IndexSet::IndexIterator>(*m_iterator).operator==(std::any_cast<IndexSet::IndexIterator>(*it.m_iterator));
#endif
    }

    index_set::index_iterator &index_set::index_iterator::operator++() noexcept {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<IndexSet::IndexIterator*>(&m_iterator)->operator++();
#else
        std::any_cast<IndexSet::IndexIterator>(m_iterator.get())->operator++();
#endif
        return *this;
    }

    index_set collection_change_set::insertions() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const CollectionChangeSet*>(&m_change_set)->insertions;
#else
        return m_change_set->insertions;
#endif
    }

    bool collection_change_set::collection_root_was_deleted() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const CollectionChangeSet*>(&m_change_set)->collection_root_was_deleted;
#else
        return m_change_set->collection_root_was_deleted;
#endif
    }

    bool index_set::index_iterator::operator!=(const index_set::index_iterator &it) const noexcept {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const IndexSet::IndexIterator*>(&m_iterator)->operator!=(
                *reinterpret_cast<const IndexSet::IndexIterator*>(it.m_iterator));
#else
        return std::any_cast<IndexSet::IndexIterator>(*m_iterator).operator!=(std::any_cast<IndexSet::IndexIterator>(*it.m_iterator));
#endif

    }

    index_set::index_iterable_adaptor::index_iterable_adaptor(const index_set::index_iterable_adaptor& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_index_iterable_adaptor) IndexSet::IndexIteratableAdaptor(*reinterpret_cast<const IndexSet::IndexIteratableAdaptor *>(&other.m_index_iterable_adaptor));
#else
        m_index_iterable_adaptor = other.m_index_iterable_adaptor;
#endif
    }

    index_set::index_iterable_adaptor& index_set::index_iterable_adaptor::operator=(const index_set::index_iterable_adaptor& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_index_iterable_adaptor) IndexSet::IndexIteratableAdaptor(*reinterpret_cast<const IndexSet::IndexIteratableAdaptor *>(&other.m_index_iterable_adaptor));
#else
        m_index_iterable_adaptor = other.m_index_iterable_adaptor;
#endif
        return *this;
    }

    index_set::index_iterable_adaptor::index_iterable_adaptor(index_set::index_iterable_adaptor&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_index_iterable_adaptor) IndexSet::IndexIteratableAdaptor(std::move(*reinterpret_cast<IndexSet::IndexIteratableAdaptor *>(&other.m_index_iterable_adaptor)));
#else
        m_index_iterable_adaptor = std::move(other.m_index_iterable_adaptor);
#endif
    }

    index_set::index_iterable_adaptor& index_set::index_iterable_adaptor::operator=(index_set::index_iterable_adaptor&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_index_iterable_adaptor) IndexSet::IndexIteratableAdaptor(std::move(*reinterpret_cast<IndexSet::IndexIteratableAdaptor *>(&other.m_index_iterable_adaptor)));
#else
        m_index_iterable_adaptor = std::move(other.m_index_iterable_adaptor);
#endif
        return *this;
    }

    index_set::index_iterable_adaptor::~index_iterable_adaptor() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<IndexSet::IndexIteratableAdaptor*>(&m_index_iterable_adaptor)->~IndexIteratableAdaptor();
#endif
    }

    index_set::index_iterator index_set::index_iterable_adaptor::begin() const noexcept {
        index_iterator iter;
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&iter.m_iterator) IndexSet::IndexIterator(reinterpret_cast<const IndexSet::IndexIteratableAdaptor *>(&m_index_iterable_adaptor)->begin());
#else
        auto adapter = std::any_cast<IndexSet::IndexIteratableAdaptor>(*m_index_iterable_adaptor);
        iter.m_iterator = std::make_shared<std::any>(adapter.begin());
#endif
        return iter;
    }
    index_set::index_iterator index_set::index_iterable_adaptor::end() const noexcept {
        index_iterator iter;
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&iter.m_iterator) IndexSet::IndexIterator(reinterpret_cast<const IndexSet::IndexIteratableAdaptor *>(&m_index_iterable_adaptor)->end());
#else
        auto adapter = std::any_cast<IndexSet::IndexIteratableAdaptor>(*m_index_iterable_adaptor);
        iter.m_iterator = std::make_shared<std::any>(adapter.end());
#endif
        return iter;
    }

    index_set::index_iterable_adaptor index_set::as_indexes() const {
        index_iterable_adaptor iter;
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&iter.m_index_iterable_adaptor) IndexSet::IndexIteratableAdaptor(
                reinterpret_cast<const IndexSet*>(&m_idx_set)->as_indexes());
#else
        iter.m_index_iterable_adaptor = std::make_shared<std::any>(m_idx_set->as_indexes());
#endif
        return iter;
    }
}
