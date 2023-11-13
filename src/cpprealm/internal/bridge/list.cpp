#include <cpprealm/internal/bridge/list.hpp>

#include <cpprealm/internal/bridge/col_key.hpp>
#include <cpprealm/internal/bridge/mixed.hpp>
#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/internal/bridge/results.hpp>
#include <cpprealm/internal/bridge/table.hpp>

#include <realm/object-store/list.hpp>
#include "realm/object-store/shared_realm.hpp"
#include <realm/object-store/results.hpp>

namespace realm::internal::bridge {

    list::list() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_list) List();
#else
        m_list = std::make_shared<List>();
#endif
    }
    
    list::list(const list& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_list) List(*reinterpret_cast<const List*>(&other.m_list));
#else
        m_list = other.m_list;
#endif
    }
    
    list& list::operator=(const list& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<List*>(&m_list) = *reinterpret_cast<const List*>(&other.m_list);
        }
#else
        m_list = other.m_list;
#endif
        return *this;
    }

    list::list(list&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_list) List(std::move(*reinterpret_cast<List*>(&other.m_list)));
#else
        m_list = std::move(other.m_list);
#endif
    }

    list& list::operator=(list&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<List*>(&m_list) = std::move(*reinterpret_cast<List*>(&other.m_list));
        }
#else
        m_list = std::move(other.m_list);
#endif
        return *this;
    }

    list::~list() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<List*>(&m_list)->~List();
#else
        //m_list->~List();
#endif
    }

    list::list(const List &v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_list) List(v);
#else
        m_list = std::make_shared<List>(v);
#endif
    }

    list::list(const realm &realm,
               const obj &obj,
               const col_key& col_key) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_list) List(object(realm, obj).get_list(col_key));
#else
        m_list = std::make_shared<List>(List(object(realm, obj).get_list(col_key)));
#endif
    }

    inline List* list::get_list() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<List*>(&m_list);
#else
        return m_list.get();
#endif
    }

    inline const List* list::get_list() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const List*>(&m_list);
#else
        return m_list.get();
#endif
    }

    inline List* get_list(list& lst) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<List*>(&lst.m_list);
#else
        return lst.m_list.get();
#endif
    }

    inline const List* get_list(const list& lst) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const List*>(&lst.m_list);
#else
        return lst.m_list.get();
#endif
    }

    list::operator List() const {
        return *get_list();
    }

    results list::sort(const std::vector<sort_descriptor>& descriptors) {
        std::vector<std::pair<std::string, bool>> results_descriptors;
        results_descriptors.resize(descriptors.size());
        std::transform(descriptors.begin(), descriptors.end(), results_descriptors.begin(),
                       [](const sort_descriptor& sd) -> std::pair<std::string, bool>{
                           return sd.operator std::pair<std::string, bool>();
                       });
        return get_list()->sort(results_descriptors);
    }

    table list::get_table() const {
        return get_list()->get_table();
    }
    size_t list::size() const {
        return get_list()->size();
    }
    void list::remove(size_t idx) {
        get_list()->remove(idx);
    }
    void list::remove_all() {
        get_list()->remove_all();
    }

    void list::add(const std::string &v) {
        get_list()->add(StringData(v));
    }
    void list::add(const int64_t &v) {
        get_list()->add(v);
    }
    void list::add(const double &v) {
        get_list()->add(v);
    }
    void list::add(const binary &v) {
        get_list()->add(static_cast<BinaryData>(v));
    }
    void list::add(const uuid &v) {
        get_list()->add(static_cast<UUID>(v));
    }
    void list::add(const object_id &v) {
        get_list()->add(static_cast<ObjectId>(v));
    }
    void list::add(const decimal128 &v) {
        get_list()->add(static_cast<Decimal128>(v));
    }
    void list::add(const mixed &v) {
        get_list()->add(v.operator ::realm::Mixed());
    }
    void list::add(const obj_key &v) {
        get_list()->add(static_cast<ObjKey>(v));
    }
    void list::add(const timestamp &v) {
        get_list()->add(v.operator Timestamp());
    }
    void list::add(const bool &v) {
        get_list()->add(v);
    }
    obj list::add_embedded() {
        return get_list()->add_embedded();
    }

    template <>
    std::string get(const list& lst, size_t idx) {
        return get_list(lst)->get<StringData>(idx);
    }
    template <>
    int64_t get(const list& lst, size_t idx) {
        return get_list(lst)->get<Int>(idx);
    }
    template <>
    double get(const list& lst, size_t idx) {
        return get_list(lst)->get<Double>(idx);
    }
    template <>
    binary get(const list& lst, size_t idx) {
        return get_list(lst)->get<BinaryData>(idx);
    }
    template <>
    uuid get(const list& lst, size_t idx) {
        return get_list(lst)->get<UUID>(idx);
    }
    template <>
    object_id get(const list& lst, size_t idx) {
        return get_list(lst)->get<ObjectId>(idx);
    }
    template <>
    decimal128 get(const list& lst, size_t idx) {
        return get_list(lst)->get<Decimal128>(idx);
    }
    template <>
    mixed get(const list& lst, size_t idx) {
        return get_list(lst)->get<Mixed>(idx);
    }
    template <>
    obj get(const list& lst, size_t idx) {
        return get_list(lst)->get<Obj>(idx);
    }
    template <>
    bool get(const list& lst, size_t idx) {
        return get_list(lst)->get<Bool>(idx);
    }
    template <>
    timestamp get(const list& lst, size_t idx) {
        return get_list(lst)->get<Timestamp>(idx);
    }

    template <>
    std::optional<int64_t> get(const list& lst, size_t idx) {
        return get_list(lst)->get<std::optional<Int>>(idx);
    }
    template <>
    std::optional<double> get(const list& lst, size_t idx) {
        return get_list(lst)->get<std::optional<Double>>(idx);
    }
    template <>
    std::optional<bool> get(const list& lst, size_t idx) {
        return get_list(lst)->get<std::optional<Bool>>(idx);
    }
    template <>
    std::optional<uuid> get(const list& lst, size_t idx) {
        return get_list(lst)->get<std::optional<UUID>>(idx);
    }
    template <>
    std::optional<object_id> get(const list& lst, size_t idx) {
        return get_list(lst)->get<std::optional<ObjectId>>(idx);
    }
    template <>
    std::optional<decimal128> get(const list& lst, size_t idx) {
        auto v = get_list(lst)->get<Decimal128>(idx);
        if (v.is_null()) {
            return std::nullopt;
        } else {
            return v;
        }
    }
    template <>
    std::optional<timestamp> get(const list& lst, size_t idx) {
        auto v = get_list(lst)->get<Timestamp>(idx);
        if (v.is_null()) {
            return std::nullopt;
        } else {
            return v;
        }
    }
    template <>
    std::optional<std::string> get(const list& lst, size_t idx) {
        auto v = get_list(lst)->get<StringData>(idx);
        if (v.is_null()) {
            return std::nullopt;
        } else {
            return v;
        }
    }
    template <>
    std::optional<binary> get(const list& lst, size_t idx) {
        auto v = get_list(lst)->get<BinaryData>(idx);
        if (v.is_null()) {
            return std::nullopt;
        } else {
            return v;
        }
    }

    void list::set(size_t pos, const int64_t &v) { get_list()->set(pos, v); }
    void list::set(size_t pos, const bool &v) { get_list()->set(pos, v); }
    void list::set(size_t pos, const std::string &v) { get_list()->set(pos, StringData(v)); }
    void list::set(size_t pos, const double &v) { get_list()->set(pos, v); }
    void list::set(size_t pos, const uuid &v) { get_list()->set(pos, static_cast<UUID>(v)); }
    void list::set(size_t pos, const object_id &v) { get_list()->set(pos, static_cast<ObjectId>(v)); }
    void list::set(size_t pos, const decimal128 &v) { get_list()->set(pos, static_cast<Decimal128>(v)); }
    void list::set(size_t pos, const mixed &v) { get_list()->set(pos, v.operator ::realm::Mixed()); }
    void list::set(size_t pos, const binary &v) { get_list()->set(pos, static_cast<BinaryData>(v)); }
    void list::set(size_t pos, const timestamp &v) { get_list()->set(pos, v.operator Timestamp()); }

    size_t list::find(const int64_t &v) { return get_list()->find(v); }
    size_t list::find(const bool &v) { return get_list()->find(v); }
    size_t list::find(const double &v) { return get_list()->find(v); }
    size_t list::find(const std::string &v) { return get_list()->find(StringData(v)); }
    size_t list::find(const uuid &v) { return get_list()->find(static_cast<UUID>(v)); }
    size_t list::find(const object_id &v) { return get_list()->find(static_cast<ObjectId>(v)); }
    size_t list::find(const decimal128 &v) { return get_list()->find(static_cast<Decimal128>(v)); }
    size_t list::find(const mixed &v) { return get_list()->find(v.operator ::realm::Mixed()); }
    size_t list::find(const timestamp &v) { return get_list()->find(v.operator Timestamp()); }
    size_t list::find(const binary& v) {
        auto v_actual = v.operator BinaryData();
        auto val_actual = get_list()->get<BinaryData>(1);
        auto val = get_list()->find(static_cast<BinaryData>(v));
        return val;
    }
    size_t list::find(const obj_key& v) { return get_list()->find(static_cast<ObjKey>(v)); }

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
        return get_list()->add_notification_callback(ccb);
    }
}
