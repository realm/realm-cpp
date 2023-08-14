#include <cpprealm/internal/bridge/set.hpp>

#include <cpprealm/internal/bridge/col_key.hpp>
#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/internal/bridge/mixed.hpp>
#include <cpprealm/internal/bridge/realm.hpp>

#include <cpprealm/internal/bridge/table.hpp>
#include <cpprealm/internal/type_info.hpp>

#include <realm/object-store/set.hpp>

namespace realm::internal::bridge {
#ifdef __i386__
    static_assert(SizeCheck<40, sizeof(object_store::Set)>{});
    static_assert(SizeCheck<4, alignof(object_store::Set)>{});
#elif __x86_64__
    static_assert(SizeCheck<80, sizeof(object_store::Set)>{});
    static_assert(SizeCheck<8, alignof(object_store::Set)>{});
#elif __arm__
    static_assert(SizeCheck<40, sizeof(object_store::Set)>{});
    static_assert(SizeCheck<4, alignof(object_store::Set)>{});
#elif __aarch64__
    static_assert(SizeCheck<80, sizeof(object_store::Set)>{});
    static_assert(SizeCheck<8, alignof(object_store::Set)>{});
#elif _WIN32
    static_assert(SizeCheck<80, sizeof(object_store::Set)>{});
    static_assert(SizeCheck<8, alignof(object_store::Set)>{});
#endif

    set::set() {
        new (&m_set) object_store::Set();
    }

    set::set(const set& other) {
        new (&m_set) object_store::Set(*reinterpret_cast<const object_store::Set*>(&other.m_set));
    }

    set& set::operator=(const set& other) {
        if (this != &other) {
            *reinterpret_cast<object_store::Set*>(&m_set) = *reinterpret_cast<const object_store::Set*>(&other.m_set);
        }
        return *this;
    }

    set::set(set&& other) {
        new (&m_set) object_store::Set(std::move(*reinterpret_cast<object_store::Set*>(&other.m_set)));
    }

    set& set::operator=(set&& other) {
        if (this != &other) {
            *reinterpret_cast<object_store::Set*>(&m_set) = std::move(*reinterpret_cast<object_store::Set*>(&other.m_set));
        }
        return *this;
    }

    set::~set() {
        reinterpret_cast<object_store::Set*>(&m_set)->~Set();
    }

    set::set(const object_store::Set &v) {
        new (&m_set) object_store::Set(v);
    }

    set::set(const realm &realm,
               const obj &obj,
               const col_key& col_key) {
        new (&m_set) object_store::Set(realm.operator std::shared_ptr<Realm>(), obj, col_key);
    }

    mixed set::get_any(const size_t& i) const {
        return reinterpret_cast<const object_store::Set*>(&m_set)->get_any(i);
    }

    obj set::get_obj(const size_t& i) const {
        return reinterpret_cast<const object_store::Set*>(&m_set)->get<Obj>(i);
    }

    set::operator object_store::Set() const {
        return *reinterpret_cast<const object_store::Set*>(&m_set);
    }

    table set::get_table() const {
        return reinterpret_cast<const object_store::Set *>(&m_set)->get_table();
    }
    size_t set::size() const {
        return reinterpret_cast<const object_store::Set *>(&m_set)->size();
    }
    void set::remove_all() {
        reinterpret_cast<object_store::Set *>(&m_set)->remove_all();
    }

    std::pair<size_t, bool> set::insert(const std::string &v) {
        return reinterpret_cast<object_store::Set *>(&m_set)->insert(StringData(v));
    }
    std::pair<size_t, bool> set::insert(const int64_t &v) {
        return reinterpret_cast<object_store::Set *>(&m_set)->insert(v);
    }
    std::pair<size_t, bool> set::insert(const double &v) {
        return reinterpret_cast<object_store::Set *>(&m_set)->insert(v);
    }
    std::pair<size_t, bool> set::insert(const binary &v) {
        return reinterpret_cast<object_store::Set *>(&m_set)->insert(static_cast<BinaryData>(v));
    }
    std::pair<size_t, bool> set::insert(const uuid &v) {
        return reinterpret_cast<object_store::Set *>(&m_set)->insert(static_cast<UUID>(v));
    }
    std::pair<size_t, bool> set::insert(const object_id &v) {
        return reinterpret_cast<object_store::Set *>(&m_set)->insert(static_cast<ObjectId>(v));
    }
    std::pair<size_t, bool> set::insert(const mixed &v) {
        return reinterpret_cast<object_store::Set *>(&m_set)->insert(v.operator ::realm::Mixed());
    }
    std::pair<size_t, bool> set::insert(const obj_key &v) {
        return reinterpret_cast<object_store::Set *>(&m_set)->insert(static_cast<ObjKey>(v));
    }
    std::pair<size_t, bool> set::insert(const timestamp &v) {
        return reinterpret_cast<object_store::Set *>(&m_set)->insert(static_cast<Timestamp>(v));
    }
    std::pair<size_t, bool> set::insert(const bool &v) {
        return reinterpret_cast<object_store::Set *>(&m_set)->insert(v);
    }

    void set::remove(const int64_t &v) { reinterpret_cast<object_store::Set *>(&m_set)->remove(v); }
    void set::remove(const bool &v) { reinterpret_cast<object_store::Set *>(&m_set)->remove(v); }
    void set::remove(const double &v) { reinterpret_cast<object_store::Set *>(&m_set)->remove(v); }
    void set::remove(const std::string &v) { reinterpret_cast<object_store::Set *>(&m_set)->remove(StringData(v)); }
    void set::remove(const uuid &v) { reinterpret_cast<object_store::Set *>(&m_set)->remove(static_cast<UUID>(v)); }
    void set::remove(const object_id &v) { reinterpret_cast<object_store::Set *>(&m_set)->remove(static_cast<ObjectId>(v)); }
    void set::remove(const mixed &v) { reinterpret_cast<object_store::Set *>(&m_set)->remove(v.operator ::realm::Mixed()); }
    void set::remove(const timestamp &v) { reinterpret_cast<object_store::Set *>(&m_set)->remove(static_cast<Timestamp>(v)); }
    void set::remove(const binary& v) { reinterpret_cast<object_store::Set *>(&m_set)->remove(static_cast<BinaryData>(v)); }
    void set::remove(const obj_key& v) { reinterpret_cast<object_store::Set *>(&m_set)->remove(static_cast<ObjKey>(v)); }

    size_t set::find(const int64_t &v) { return reinterpret_cast<object_store::Set *>(&m_set)->find(v); }
    size_t set::find(const bool &v) { return reinterpret_cast<object_store::Set *>(&m_set)->find(v); }
    size_t set::find(const double &v) { return reinterpret_cast<object_store::Set *>(&m_set)->find(v); }
    size_t set::find(const std::string &v) { return reinterpret_cast<object_store::Set *>(&m_set)->find(StringData(v)); }
    size_t set::find(const uuid &v) { return reinterpret_cast<object_store::Set *>(&m_set)->find(static_cast<UUID>(v)); }
    size_t set::find(const object_id &v) { return reinterpret_cast<object_store::Set *>(&m_set)->find(static_cast<ObjectId>(v)); }
    size_t set::find(const mixed &v) { return reinterpret_cast<object_store::Set *>(&m_set)->find(v.operator ::realm::Mixed()); }
    size_t set::find(const timestamp &v) { return reinterpret_cast<object_store::Set *>(&m_set)->find(static_cast<Timestamp>(v)); }
    size_t set::find(const binary& v) { return reinterpret_cast<object_store::Set *>(&m_set)->find(static_cast<BinaryData>(v)); }
    size_t set::find(const obj_key& v) { return reinterpret_cast<object_store::Set *>(&m_set)->find(static_cast<ObjKey>(v)); }

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
        return reinterpret_cast<object_store::Set*>(&m_set)->add_notification_callback(ccb);
    }
}
