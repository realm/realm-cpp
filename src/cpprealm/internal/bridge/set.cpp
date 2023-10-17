#include <cpprealm/internal/bridge/set.hpp>

#include <cpprealm/internal/bridge/col_key.hpp>
#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/internal/bridge/mixed.hpp>
#include <cpprealm/internal/bridge/realm.hpp>

#include <cpprealm/internal/bridge/table.hpp>
#include <realm/object-store/set.hpp>

#include <realm/array_mixed.hpp>
#include <realm/array_typed_link.hpp>

namespace realm::internal::bridge {
    set::set() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_set) object_store::Set();
#else
        m_set = std::make_shared<object_store::Set>();
#endif
    }

    set::set(const set& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_set) object_store::Set(*reinterpret_cast<const object_store::Set*>(&other.m_set));
#else
        m_set = other.m_set;
#endif
    }

    set& set::operator=(const set& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<object_store::Set*>(&m_set) = *reinterpret_cast<const object_store::Set*>(&other.m_set);
        }
#else
        m_set = other.m_set;
#endif
        return *this;
    }

    set::set(set&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_set) object_store::Set(std::move(*reinterpret_cast<object_store::Set*>(&other.m_set)));
#else
        m_set = std::move(other.m_set);
#endif
    }

    set& set::operator=(set&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<object_store::Set*>(&m_set) = std::move(*reinterpret_cast<object_store::Set*>(&other.m_set));
        }
#else
        m_set = std::move(other.m_set);
#endif
        return *this;
    }

    set::~set() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<object_store::Set*>(&m_set)->~Set();
#endif
    }

    set::set(const object_store::Set &v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_set) object_store::Set(v);
#else
        m_set = std::make_shared<object_store::Set>(v);
#endif
    }

    set::set(const realm &realm,
               const obj &obj,
               const col_key& col_key) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_set) object_store::Set(realm.operator std::shared_ptr<Realm>(), obj, col_key);
#else
        m_set = std::make_shared<object_store::Set>(realm.operator std::shared_ptr<Realm>(), obj, col_key);
#endif
    }

    const object_store::Set* set::get_set() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const object_store::Set*>(&m_set);
#else
        return m_set.get();
#endif
    }
    object_store::Set* set::get_set() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<object_store::Set*>(&m_set);
#else
        return m_set.get();
#endif
    }

    mixed set::get_any(const size_t& i) const {
        return get_set()->get_any(i);
    }

    obj set::get_obj(const size_t& i) const {
        return get_set()->get<Obj>(i);
    }

    set::operator object_store::Set() const {
        return *get_set();
    }

    table set::get_table() const {
        return get_set()->get_table();
    }
    size_t set::size() const {
        return get_set()->size();
    }
    void set::remove_all() {
        get_set()->remove_all();
    }

    std::pair<size_t, bool> set::insert(const std::string &v) {
        return get_set()->insert(StringData(v));
    }
    std::pair<size_t, bool> set::insert(const int64_t &v) {
        return get_set()->insert(v);
    }
    std::pair<size_t, bool> set::insert(const double &v) {
        return get_set()->insert(v);
    }
    std::pair<size_t, bool> set::insert(const binary &v) {
        return get_set()->insert(v.operator BinaryData());
    }
    std::pair<size_t, bool> set::insert(const uuid &v) {
        return get_set()->insert(v.operator UUID());
    }
    std::pair<size_t, bool> set::insert(const object_id &v) {
        return get_set()->insert(v.operator ObjectId());
    }
    std::pair<size_t, bool> set::insert(const mixed &v) {
        return get_set()->insert(v.operator ::realm::Mixed());
    }
    std::pair<size_t, bool> set::insert(const obj_key &v) {
        return get_set()->insert(v.operator ObjKey());
    }
    std::pair<size_t, bool> set::insert(const timestamp &v) {
        return get_set()->insert(v.operator Timestamp());
    }
    std::pair<size_t, bool> set::insert(const bool &v) {
        return get_set()->insert(v);
    }

    void set::remove(const int64_t &v) { get_set()->remove(v); }
    void set::remove(const bool &v) { get_set()->remove(v); }
    void set::remove(const double &v) { get_set()->remove(v); }
    void set::remove(const std::string &v) { get_set()->remove(StringData(v)); }
    void set::remove(const uuid &v) { get_set()->remove(v.operator UUID()); }
    void set::remove(const object_id &v) { get_set()->remove(v.operator ObjectId()); }
    void set::remove(const mixed &v) { get_set()->remove(v.operator ::realm::Mixed()); }
    void set::remove(const timestamp &v) { get_set()->remove(v.operator Timestamp()); }
    void set::remove(const binary& v) { get_set()->remove(v.operator BinaryData()); }
    void set::remove(const obj_key& v) { get_set()->remove(v.operator ObjKey()); }

    size_t set::find(const int64_t &v) { return get_set()->find(v); }
    size_t set::find(const bool &v) { return get_set()->find(v); }
    size_t set::find(const double &v) { return get_set()->find(v); }
    size_t set::find(const std::string &v) { return get_set()->find(StringData(v)); }
    size_t set::find(const uuid &v) { return get_set()->find(v.operator UUID()); }
    size_t set::find(const object_id &v) { return get_set()->find(v.operator ObjectId()); }
    size_t set::find(const mixed &v) { return get_set()->find(v.operator ::realm::Mixed()); }
    size_t set::find(const timestamp &v) { return get_set()->find(v.operator Timestamp()); }
    size_t set::find(const binary& v) { return get_set()->find(v.operator BinaryData()); }
    size_t set::find(const obj_key& v) { return get_set()->find(v.operator ObjKey()); }

    notification_token set::add_notification_callback(std::shared_ptr<collection_change_callback> cb) {
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
        return get_set()->add_notification_callback(ccb);
    }
}
