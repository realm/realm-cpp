#include <cpprealm/internal/bridge/col_key.hpp>
#include <cpprealm/internal/bridge/list.hpp>
#include <cpprealm/internal/bridge/binary.hpp>
#include <cpprealm/internal/bridge/mixed.hpp>
#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/internal/bridge/realm.hpp>
#include <cpprealm/internal/bridge/utils.hpp>
#include <cpprealm/internal/bridge/uuid.hpp>
#include <cpprealm/internal/bridge/object_id.hpp>
#include <cpprealm/internal/bridge/table.hpp>
#include <cpprealm/internal/type_info.hpp>

#include <realm/object-store/list.hpp>

namespace realm::internal::bridge {
    list::list() {
        m_list = std::make_shared<List>(List());
    }

    list::list(const List &v) {
        m_list = std::make_shared<List>(v);
    }

    list::list(const realm &realm,
               const obj &obj,
               const col_key& col_key) {
        m_list = std::make_shared<List>(List(realm, obj, col_key));
    }

    list::operator List() const {
        return *m_list;
    }

    table list::get_table() const {
        return m_list->get_table();
    }
    size_t list::size() const {
        return m_list->size();
    }
    void list::remove(size_t idx) {
        m_list->remove(idx);
    }
    void list::remove_all() {
        m_list->remove_all();
    }

    realm list::get_realm() const {
        return m_list->get_realm();
    }

    void list::add(const std::string &v) {
        m_list->add(StringData(v));
    }
    void list::add(const int64_t &v) {
        m_list->add(v);
    }
    void list::add(const double &v) {
        m_list->add(v);
    }
    void list::add(const binary &v) {
        m_list->add(static_cast<BinaryData>(v));
    }
    void list::add(const uuid &v) {
        m_list->add(static_cast<UUID>(v));
    }
    void list::add(const object_id &v) {
        m_list->add(static_cast<ObjectId>(v));
    }
    void list::add(const mixed &v) {
        m_list->add(static_cast<Mixed>(v));
    }
    void list::add(const obj_key &v) {
        m_list->add(static_cast<ObjKey>(v));
    }
    void list::add(const timestamp &v) {
        m_list->add(static_cast<Timestamp>(v));
    }
    void list::add(const bool &v) {
        m_list->add(v);
    }
    obj list::add_embedded() {
        return m_list->add_embedded();
    }

    template <>
    std::string get(const list& lst, size_t idx) {
        return lst.m_list->get<StringData>(idx);
    }
    template <>
    int64_t get(const list& lst, size_t idx) {
        return lst.m_list->get<Int>(idx);
    }
    template <>
    double get(const list& lst, size_t idx) {
        return lst.m_list->get<Double>(idx);
    }
    template <>
    binary get(const list& lst, size_t idx) {
        return lst.m_list->get<BinaryData>(idx);
    }
    template <>
    uuid get(const list& lst, size_t idx) {
        return lst.m_list->get<UUID>(idx);
    }
    template <>
    object_id get(const list& lst, size_t idx) {
        return lst.m_list->get<ObjectId>(idx);
    }
    template <>
    mixed get(const list& lst, size_t idx) {
        return lst.m_list->get<Mixed>(idx);
    }
    template <>
    obj get(const list& lst, size_t idx) {
        return lst.m_list->get<Obj>(idx);
    }
    template <>
    bool get(const list& lst, size_t idx) {
        return lst.m_list->get<Bool>(idx);
    }
    template <>
    timestamp get(const list& lst, size_t idx) {
        return lst.m_list->get<Timestamp>(idx);
    }

    void list::set(size_t pos, const int64_t &v) { m_list->set(pos, v); }
    void list::set(size_t pos, const bool &v) { m_list->set(pos, v); }
    void list::set(size_t pos, const std::string &v) { m_list->set(pos, StringData(v)); }
    void list::set(size_t pos, const double &v) { m_list->set(pos, v); }
    void list::set(size_t pos, const uuid &v) { m_list->set(pos, static_cast<UUID>(v)); }
    void list::set(size_t pos, const object_id &v) { m_list->set(pos, static_cast<ObjectId>(v)); }
    void list::set(size_t pos, const mixed &v) { m_list->set(pos, static_cast<Mixed>(v)); }
    void list::set(size_t pos, const binary &v) { m_list->set(pos, static_cast<BinaryData>(v)); }
    void list::set(size_t pos, const timestamp &v) { m_list->set(pos, static_cast<Timestamp>(v)); }

    size_t list::find(const int64_t &v) { return m_list->find(v); }
    size_t list::find(const bool &v) { return m_list->find(v); }
    size_t list::find(const double &v) { return m_list->find(v); }
    size_t list::find(const std::string &v) { return m_list->find(StringData(v)); }
    size_t list::find(const uuid &v) { return m_list->find(static_cast<UUID>(v)); }
    size_t list::find(const object_id &v) { return m_list->find(static_cast<ObjectId>(v)); }
    size_t list::find(const mixed &v) { return m_list->find(static_cast<Mixed>(v)); }
    size_t list::find(const timestamp &v) { return m_list->find(static_cast<Timestamp>(v)); }
    size_t list::find(const binary& v) {
        auto v_actual = v.operator BinaryData();
        auto val_actual = m_list->get<BinaryData>(1);
        auto val = m_list->find(static_cast<BinaryData>(v));
        return val;
    }
    size_t list::find(const obj_key& v) { return m_list->find(static_cast<ObjKey>(v)); }

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
        return m_list->add_notification_callback(ccb);
    }
}
