#include <cpprealm/internal/bridge/object_id.hpp>

#include <cpprealm/types.hpp>

#include <realm/object_id.hpp>
#include <realm/string_data.hpp>

namespace realm::internal::bridge {
    object_id::object_id() {
        m_object_id = ObjectId().to_bytes();
    }

    object_id::object_id(const std::string &v) {
        m_object_id = ObjectId(v).to_bytes();
    }

    object_id::object_id(const ::realm::object_id &v) {
        m_object_id = ObjectId(v.to_string()).to_bytes();
    }

    object_id::object_id(const ObjectId &v) {
        m_object_id = ObjectId(v).to_bytes();
    }

    std::string object_id::to_string() const {
        return ObjectId(m_object_id).to_string();
    }

    object_id object_id::generate() {
        return ObjectId::gen();
    }

    object_id::operator ::realm::object_id() const {
        return ::realm::object_id(*this);
    }

    object_id::operator ObjectId() const {
        return ObjectId(m_object_id);
    }

    bool operator ==(const object_id& a, const object_id& b) {
        return ObjectId(a.m_object_id) == ObjectId(b.m_object_id);
    }

    bool operator !=(const object_id& a, const object_id& b) {
        return ObjectId(a.m_object_id) != ObjectId(b.m_object_id);
    }

    bool operator >(const object_id& a, const object_id& b) {
        return ObjectId(a.m_object_id) > ObjectId(b.m_object_id);
    }

    bool operator <(const object_id& a, const object_id& b) {
        return ObjectId(a.m_object_id) < ObjectId(b.m_object_id);
    }

    bool operator >=(const object_id& a, const object_id& b) {
        return ObjectId(a.m_object_id) >= ObjectId(b.m_object_id);
    }

    bool operator <=(const object_id& a, const object_id& b) {
        return ObjectId(a.m_object_id) <= ObjectId(b.m_object_id);
    }
}