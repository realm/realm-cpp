#include "obj.hpp"
#include "realm/object-store/shared_realm.hpp"
#include "realm/obj.hpp"
#include <realm/object-store/object_store.hpp>
#include <realm/list.hpp>
#include <realm/dictionary.hpp>

#include <cpprealm/internal/bridge/utils.hpp>
#include <cpprealm/internal/bridge/realm.hpp>
#include <cpprealm/internal/bridge/dictionary.hpp>
#include <cpprealm/internal/bridge/lnklst.hpp>
#include <cpprealm/internal/bridge/uuid.hpp>
#include <cpprealm/internal/bridge/timestamp.hpp>
#include <cpprealm/internal/bridge/mixed.hpp>
#include <cpprealm/object.hpp>

namespace realm::internal::bridge {
    static_assert(SizeCheck<64, sizeof(Obj)>{});
    static_assert(SizeCheck<128, sizeof(::realm::Dictionary)>{});
    static_assert(SizeCheck<304, sizeof(::realm::Dictionary::Iterator)>{});

    dict::iterator::value_type dict::iterator::operator*() const {
        return reinterpret_cast<const ::realm::Dictionary::Iterator*>(m_iterator)->operator*();
    }

    dict::iterator &dict::iterator::operator++() {
        new (&m_iterator)
                ::realm::Dictionary::Iterator(reinterpret_cast<::realm::Dictionary::Iterator*>(m_iterator)->operator++());
        return *this;
    }

    dict::iterator& dict::iterator::operator+=(ptrdiff_t adj) {
        new (&m_iterator)
                ::realm::Dictionary::Iterator(reinterpret_cast<::realm::Dictionary::Iterator*>(m_iterator)->operator+=(adj));
        return *this;
    }

    dict::iterator dict::iterator::operator+(ptrdiff_t n) const {
        reinterpret_cast<const ::realm::Dictionary::Iterator*>(m_iterator)->operator+(n);
        return *this;
    }

    size_t dict::iterator::get_position() {
        return reinterpret_cast<::realm::Dictionary::Iterator*>(m_iterator)->get_position();
    }

    dict::dict(const ::realm::Dictionary&v) {
        new (&m_dictionary) ::realm::Dictionary(v);
    }

    dict::operator ::realm::Dictionary() const {
        return *reinterpret_cast<const ::realm::Dictionary*>(m_dictionary);
    }

    std::pair<mixed, mixed> dict::get_pair(size_t ndx) const {
        return reinterpret_cast<const ::realm::Dictionary*>(m_dictionary)->get_pair(ndx);
    }

    mixed dict::get_key(size_t ndx) const {
        return reinterpret_cast<const ::realm::Dictionary*>(m_dictionary)->get_key(ndx);
    }

    size_t dict::size() const {
        return reinterpret_cast<const ::realm::Dictionary*>(m_dictionary)->size();
    }
    bool dict::is_null(size_t ndx) const {
        return reinterpret_cast<const ::realm::Dictionary*>(m_dictionary)->is_null(ndx);
    }
    mixed dict::get_any(size_t ndx) const {
        return reinterpret_cast<const ::realm::Dictionary*>(m_dictionary)->get_any(ndx);
    }
    size_t dict::find_any(mixed value) const {
        return reinterpret_cast<const ::realm::Dictionary*>(m_dictionary)->find_any(static_cast<Mixed>(value));
    }
    size_t dict::find_any_key(mixed value) const noexcept {
        return reinterpret_cast<const ::realm::Dictionary*>(m_dictionary)->find_any_key(static_cast<Mixed>(value));
    }
    std::pair<dict::iterator, bool> dict::insert(const mixed& key, const mixed& value) {
        auto pair = reinterpret_cast<::realm::Dictionary*>(m_dictionary)->insert(static_cast<Mixed>(key),
                                                                                 static_cast<Mixed>(value));
        dict::iterator i;
        new (&i.m_iterator) ::realm::Dictionary::Iterator(pair.first);
        return { i, pair.second };
    }
    std::pair<dict::iterator, bool> dict::insert(mixed key, const obj& obj) {
        auto pair = reinterpret_cast<::realm::Dictionary*>(m_dictionary)->insert(static_cast<Mixed>(key), obj);
        dict::iterator i;
        new (&i.m_iterator) ::realm::Dictionary::Iterator(pair.first);
        return { i, pair.second };
    }

    obj dict::create_and_insert_linked_object(mixed key) {
        return reinterpret_cast<::realm::Dictionary*>(m_dictionary)->create_and_insert_linked_object(static_cast<Mixed>(key));
    }

    // throws std::out_of_range if key is not found
    mixed dict::get(mixed key) const {
        return reinterpret_cast<const ::realm::Dictionary*>(m_dictionary)->get(static_cast<Mixed>(key));
    }
    // Noexcept version
    std::optional<mixed> dict::try_get(mixed key) const noexcept {
        return reinterpret_cast<const ::realm::Dictionary*>(m_dictionary)->try_get(static_cast<Mixed>(key));
    }
    // adds entry if key is not found
    const mixed dict::operator[](mixed key) {
        return reinterpret_cast<::realm::Dictionary*>(m_dictionary)->operator[](static_cast<Mixed>(key));
    }

    obj dict::get_object(StringData key) {
        return reinterpret_cast<::realm::Dictionary*>(m_dictionary)->get_object(key);
    }

    group::group(realm& val)
    : m_realm(val)
    {
    }

    group realm::read_group() {
        return *this;
    }

    obj::obj(const Obj& v) {
        new (&m_obj) Obj(v);
    }

    obj::operator Obj() const {
        return *reinterpret_cast<const Obj*>(m_obj);
    }

    dict obj::get_dictionary(const col_key &col_key) {
        return reinterpret_cast<const Obj*>(m_obj)->get_dictionary(col_key);
    }
    obj_key obj::get_key() const {
        return reinterpret_cast<const Obj*>(m_obj)->get_key();
    }
    table obj::get_table() const noexcept {
        return reinterpret_cast<const Obj*>(m_obj)->get_table();
    }
    table obj::get_target_table(col_key key) const noexcept {
        return reinterpret_cast<const Obj*>(m_obj)->get_target_table(key);
    }
    obj obj::get_linked_object(const col_key &col_key) {
        return reinterpret_cast<Obj*>(m_obj)->get_linked_object(col_key);
    }
    bool obj::is_null(const col_key &col_key) const {
        return reinterpret_cast<const Obj*>(m_obj)->is_null(col_key);
    }
    template <>
    std::string obj::get(const col_key& col_key) const {
        return reinterpret_cast<const Obj*>(m_obj)->get<StringData>(col_key);
    }
    template <>
    int64_t obj::get(const col_key& col_key) const {
        return reinterpret_cast<const Obj*>(m_obj)->get<Int>(col_key);
    }
    template <>
    bool obj::get(const col_key& col_key) const {
        return reinterpret_cast<const Obj*>(m_obj)->get<bool>(col_key);
    }
    template <>
    binary obj::get(const col_key& col_key) const {
        return reinterpret_cast<const Obj*>(m_obj)->get<BinaryData>(col_key);
    }
    template <>
    uuid obj::get(const col_key& col_key) const {
        return reinterpret_cast<const Obj*>(m_obj)->get<UUID>(col_key);
    }
    template <>
    mixed obj::get(const col_key& col_key) const {
        return reinterpret_cast<const Obj*>(m_obj)->get_any(col_key);
    }
    template <>
    timestamp obj::get(const col_key& col_key) const {
        return reinterpret_cast<const Obj*>(m_obj)->get<Timestamp>(col_key);
    }
    void obj::set(const col_key &col_key, const std::string &value) {
        reinterpret_cast<Obj*>(m_obj)->set<StringData>(col_key, value);
    }
    void obj::set(const col_key &col_key, const binary &value) {
        reinterpret_cast<Obj*>(m_obj)->set<BinaryData>(col_key, value);
    }
    void obj::set(const col_key &col_key, const int64_t &value) {
        reinterpret_cast<Obj*>(m_obj)->set<Int>(col_key, value);
    }
    void obj::set(const col_key &col_key, const bool &value) {
        reinterpret_cast<Obj*>(m_obj)->set<Bool>(col_key, value);
    }
    void obj::set(const col_key &col_key, const double &value) {
        reinterpret_cast<Obj*>(m_obj)->set<Double>(col_key, value);
    }
    void obj::set(const col_key &col_key, const internal::bridge::uuid &value) {
        reinterpret_cast<Obj*>(m_obj)->set<UUID>(col_key, value);
    }
    void obj::set(const col_key &col_key, const obj_key &value) {
        reinterpret_cast<Obj*>(m_obj)->set<ObjKey>(col_key, value);
    }
    void obj::set(const col_key &col_key, const mixed &value) {
        reinterpret_cast<Obj*>(m_obj)->set_any(col_key, static_cast<Mixed>(value));
    }
    void obj::set(const col_key &col_key, const timestamp &value) {
        reinterpret_cast<Obj*>(m_obj)->set<Timestamp>(col_key, value);
    }
    lnklst obj::get_linklist(const col_key &col_key) {
        return reinterpret_cast<Obj*>(m_obj)->get_linklist(col_key);
    }

    void obj::set_list_values(const col_key &col_key, const std::vector<obj_key> &values) {
        std::vector<ObjKey> v;
        for (auto& v2 : values) {
            v.push_back(v2);
        }
        reinterpret_cast<Obj*>(m_obj)->set_list_values(col_key, v);
    }
    void obj::set_list_values(const col_key &col_key, const std::vector<bool> &values) {
        reinterpret_cast<Obj*>(m_obj)->set_list_values(col_key, values);
    }
    void obj::set_list_values(const col_key &col_key, const std::vector<int64_t> &values) {
        reinterpret_cast<Obj*>(m_obj)->set_list_values(col_key, values);
    }
    void obj::set_list_values(const col_key &col_key, const std::vector<internal::bridge::uuid> &values) {
        std::vector<UUID> v;
        for (auto& v2 : values) {
            v.push_back(v2);
        }
        reinterpret_cast<Obj*>(m_obj)->set_list_values(col_key, v);
    }
    void obj::set_list_values(const col_key &col_key, const std::vector<binary> &values) {
        std::vector<BinaryData> v;
        for (auto& v2 : values) {
            v.push_back(v2);
        }
        reinterpret_cast<Obj*>(m_obj)->set_list_values(col_key, v);
    }
    void obj::set_list_values(const col_key &col_key, const std::vector<std::string> &values) {
        std::vector<StringData> v;
        for (auto& v2 : values) {
            v.emplace_back(v2);
        }
        reinterpret_cast<Obj*>(m_obj)->set_list_values(col_key, v);
    }
    void obj::set_list_values(const col_key &col_key, const std::vector<timestamp> &values) {
        std::vector<Timestamp> v;
        for (auto& v2 : values) {
            v.emplace_back(v2);
        }
        reinterpret_cast<Obj*>(m_obj)->set_list_values(col_key, v);
    }
    void obj::set_list_values(const col_key &col_key, const std::vector<mixed> &values) {
        std::vector<Mixed> v;
        for (auto& v2 : values) {
            v.emplace_back(v2);
        }
        reinterpret_cast<Obj*>(m_obj)->set_list_values(col_key, v);
    }

    void obj::set_null(const col_key &v) {
        reinterpret_cast<Obj*>(m_obj)->set_null(v);
    }

    table group::get_table(const std::string &table_key) {
        return static_cast<SharedRealm>(m_realm.get())->read_group().get_table(table_name_for_object_type(table_key));
    }
    table group::get_table(int64_t table_key) {
        return static_cast<SharedRealm>(m_realm.get())->read_group().get_table(TableKey(table_key));
    }

    obj obj::create_and_set_linked_object(const col_key &v) {
        return reinterpret_cast<Obj*>(m_obj)->create_and_set_linked_object(v);
    }

    obj::obj() {
        new (&m_obj) Obj();
    }
}

std::string realm::internal::bridge::table_name_for_object_type(const std::string &v) {
    return ObjectStore::table_name_for_object_type(v);
}
