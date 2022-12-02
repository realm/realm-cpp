#include "obj.hpp"
#include "realm/table_ref.hpp"
#include "realm/table.hpp"
#include "realm/object-store/shared_realm.hpp"
#include "realm/obj.hpp"
#include "realm/group.hpp"
#include <cpprealm/internal/bridge/utils.hpp>
#include <cpprealm/internal/bridge/realm.hpp>

namespace realm::internal::bridge {

    group::group(realm& val)
    : m_realm(val)
    {
    }

    group realm::read_group() {
        return *this;
    }

    obj::obj(Obj&& v) {
        new (&m_obj) Obj(v);
    }

    template <>
    std::string obj::get(const col_key& col_key) const {
        return reinterpret_cast<const Obj*>(m_obj)->get<StringData>(*reinterpret_cast<const ColKey*>(&col_key));
    }

    template <>
    void obj::set(const col_key &col_key, const std::string &value) {
        reinterpret_cast<Obj*>(m_obj)->set(*reinterpret_cast<const ColKey*>(&col_key), value);
    }
}
//namespace realm::internal::obj {
//    std::any get_timestamp(const std::any& obj, int64_t col_key)
//    {
//        return std::any_cast<const realm::Obj&>(obj).get<Timestamp>(static_cast<ColKey>(col_key));
//    }
//    void set_string(std::any&& obj, int64_t col_key, const std::string& string)
//    {
//        std::any_cast<Obj&>(obj).set(static_cast<ColKey>(col_key), string);
//    }
//}
//
//namespace realm::internal::query {
//
//}
//namespace realm::internal::timestamp {
//    auto time_since_epoch(const std::any& ts) {
//        return std::any_cast<const Timestamp&>(ts).get_time_point().time_since_epoch();
//    }
//}