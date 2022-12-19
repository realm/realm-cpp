#include <cpprealm/internal/bridge/col_key.hpp>
#include <cpprealm/internal/bridge/list.hpp>
#include <cpprealm/internal/bridge/binary.hpp>
#include <cpprealm/internal/bridge/mixed.hpp>
#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/internal/bridge/realm.hpp>
#include <cpprealm/internal/bridge/utils.hpp>
#include <cpprealm/internal/bridge/uuid.hpp>
#include <cpprealm/internal/bridge/table.hpp>
#include <cpprealm/internal/type_info.hpp>

#include <realm/object-store/list.hpp>

namespace realm::internal::bridge {
    static_assert(SizeCheck<80, sizeof(List)>{});

    list::list() {
        new (&m_list) List();
    }

    list::list(const List &v) {
        new (&m_list) List(v);
    }

    list::list(const realm &realm,
               const obj &obj,
               const col_key& col_key) {
        new (&m_list) List(realm, obj, col_key);
    }

    list::operator List() const {
        return *reinterpret_cast<const List*>(m_list);
    }

    table list::get_table() const {
        return reinterpret_cast<const List *>(m_list)->get_table();
    }
    size_t list::size() const {
        return reinterpret_cast<const List *>(m_list)->size();
    }
    void list::remove(size_t idx) {
        reinterpret_cast<List *>(m_list)->remove(idx);
    }
    void list::remove_all() {
        reinterpret_cast<List *>(m_list)->remove_all();
    }

    realm list::get_realm() const {
        return reinterpret_cast<const List *>(m_list)->get_realm();
    }

    void list::add(const std::string &v) {
        reinterpret_cast<List *>(m_list)->add(StringData(v));
    }
    void list::add(const int64_t &v) {
        reinterpret_cast<List *>(m_list)->add(v);
    }
    void list::add(const binary &v) {
        reinterpret_cast<List *>(m_list)->add(static_cast<BinaryData>(v));
    }
    void list::add(const uuid &v) {
        reinterpret_cast<List *>(m_list)->add(static_cast<UUID>(v));
    }
    void list::add(const mixed &v) {
        reinterpret_cast<List *>(m_list)->add(static_cast<Mixed>(v));
    }
    void list::add(const obj_key &v) {
        reinterpret_cast<List *>(m_list)->add(static_cast<ObjKey>(v));
    }
    void list::add(const timestamp &v) {
        reinterpret_cast<List *>(m_list)->add(static_cast<Timestamp>(v));
    }
    void list::add(const bool &v) {
        reinterpret_cast<List *>(m_list)->add(v);
    }
    obj list::add_embedded() {
        return reinterpret_cast<List *>(m_list)->add_embedded();
    }

    template <>
    std::string list::get(size_t idx) const {
        return reinterpret_cast<const List *>(m_list)->get<StringData>(idx);
    }
    template <>
    int64_t list::get(size_t idx) const {
        return reinterpret_cast<const List *>(m_list)->get<Int>(idx);
    }
    template <>
    binary list::get(size_t idx) const {
        return reinterpret_cast<const List *>(m_list)->get<BinaryData>(idx);
    }
    template <>
    uuid list::get(size_t idx) const {
        return reinterpret_cast<const List *>(m_list)->get<UUID>(idx);
    }
    template <>
    mixed list::get(size_t idx) const {
        return reinterpret_cast<const List *>(m_list)->get<Mixed>(idx);
    }
    template <>
    obj list::get(size_t idx) const {
        return reinterpret_cast<const List *>(m_list)->get<Obj>(idx);
    }
    template <>
    bool list::get(size_t idx) const {
        return reinterpret_cast<const List *>(m_list)->get<Bool>(idx);
    }
    template <>
    timestamp list::get(size_t idx) const {
        return reinterpret_cast<const List *>(m_list)->get<Timestamp>(idx);
    }

    void list::set(size_t pos, const int64_t &v) { reinterpret_cast<List *>(m_list)->set(pos, v); }
    void list::set(size_t pos, const bool &v) { reinterpret_cast<List *>(m_list)->set(pos, v); }
    void list::set(size_t pos, const std::string &v) { reinterpret_cast<List *>(m_list)->set(pos, StringData(v)); }
    void list::set(size_t pos, const double &v) { reinterpret_cast<List *>(m_list)->set(pos, v); }
    void list::set(size_t pos, const uuid &v) { reinterpret_cast<List *>(m_list)->set(pos, static_cast<UUID>(v)); }
    void list::set(size_t pos, const mixed &v) { reinterpret_cast<List *>(m_list)->set(pos, static_cast<Mixed>(v)); }
    void list::set(size_t pos, const binary &v) { reinterpret_cast<List *>(m_list)->set(pos, static_cast<BinaryData>(v)); }
    void list::set(size_t pos, const timestamp &v) { reinterpret_cast<List *>(m_list)->set(pos, static_cast<Timestamp>(v)); }

    size_t list::find(const int64_t &v) { return reinterpret_cast<List *>(m_list)->find(v); }
    size_t list::find(const bool &v) { return reinterpret_cast<List *>(m_list)->find(v); }
    size_t list::find(const double &v) { return reinterpret_cast<List *>(m_list)->find(v); }
    size_t list::find(const std::string &v) { return reinterpret_cast<List *>(m_list)->find(StringData(v)); }
    size_t list::find(const uuid &v) { return reinterpret_cast<List *>(m_list)->find(static_cast<UUID>(v)); }
    size_t list::find(const mixed &v) { return reinterpret_cast<List *>(m_list)->find(static_cast<Mixed>(v)); }
    size_t list::find(const timestamp &v) { return reinterpret_cast<List *>(m_list)->find(static_cast<Timestamp>(v)); }
    size_t list::find(const binary& v) {
        auto v_actual = v.operator BinaryData();
        auto val_actual = reinterpret_cast<List *>(m_list)->get<BinaryData>(1);
        auto val = reinterpret_cast<List *>(m_list)->find(static_cast<BinaryData>(v));
        return val;
    }
    size_t list::find(const obj_key& v) { return reinterpret_cast<List *>(m_list)->find(static_cast<ObjKey>(v)); }

    notification_token list::add_notification_callback(std::shared_ptr<collection_change_callback> cb) {
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
        return reinterpret_cast<List*>(m_list)->add_notification_callback(ccb);
    }
}
