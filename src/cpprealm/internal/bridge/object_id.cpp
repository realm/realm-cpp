#include <cpprealm/internal/bridge/object_id.hpp>

#include <cpprealm/experimental/types.hpp>

#include <realm/object_id.hpp>
#include <realm/string_data.hpp>

namespace realm::internal::bridge {

    object_id::object_id() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new(&m_object_id) ObjectId();
#else
        m_object_id = ObjectId().to_bytes();
#endif
    }
    object_id::object_id(const object_id& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_object_id) ObjectId(*reinterpret_cast<const ObjectId*>(&other.m_object_id));
#else
        m_object_id = other.m_object_id;
#endif
    }

    object_id& object_id::operator=(const object_id& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<ObjectId*>(&m_object_id) = *reinterpret_cast<const ObjectId*>(&other.m_object_id);
        }
#else
        m_object_id = other.m_object_id;
#endif
        return *this;
    }

    object_id::object_id(object_id&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_object_id) ObjectId(std::move(*reinterpret_cast<ObjectId*>(&other.m_object_id)));
#else
        m_object_id = std::move(other.m_object_id);
#endif
    }

    object_id& object_id::operator=(object_id&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<ObjectId*>(&m_object_id) = std::move(*reinterpret_cast<ObjectId*>(&other.m_object_id));
        }
#else
        m_object_id = std::move(other.m_object_id);
#endif
        return *this;
    }

    object_id::~object_id() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<ObjectId*>(&m_object_id)->~ObjectId();
#endif
    }

    object_id::object_id(const std::string &v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new(&m_object_id) ObjectId(v);
#else
        m_object_id = ObjectId(v).to_bytes();
#endif
    }

    object_id::object_id(const ::realm::object_id &v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new(&m_object_id) ObjectId(v.to_string());
#else
        m_object_id = ObjectId(v.to_string()).to_bytes();
#endif
    }

    object_id::object_id(const ObjectId &v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new(&m_object_id) ObjectId(v);
#else
        m_object_id = ObjectId(v).to_bytes();
#endif
    }

    std::string object_id::to_string() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const ObjectId *>(&m_object_id)->to_string();
#else
        return ObjectId(m_object_id).to_string();
#endif
    }

    object_id object_id::generate() {
        return ObjectId::gen();
    }

    object_id::operator ::realm::object_id() const {
        return ::realm::object_id(*this);
    }

    object_id::operator ObjectId() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return *reinterpret_cast<const ObjectId *>(&m_object_id);
#else
        return ObjectId(m_object_id);
#endif
    }

    bool operator ==(const object_id& a, const object_id& b) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return *reinterpret_cast<const ObjectId*>(&a.m_object_id) == *reinterpret_cast<const ObjectId*>(&b.m_object_id);
#else
        return ObjectId(a.m_object_id) == ObjectId(b.m_object_id);
#endif
    }

    bool operator !=(const object_id& a, const object_id& b) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return *reinterpret_cast<const ObjectId*>(&a.m_object_id) != *reinterpret_cast<const ObjectId*>(&b.m_object_id);
#else
        return ObjectId(a.m_object_id) != ObjectId(b.m_object_id);
#endif
    }

    bool operator >(const object_id& a, const object_id& b) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return *reinterpret_cast<const ObjectId*>(&a.m_object_id) > *reinterpret_cast<const ObjectId*>(&b.m_object_id);
#else
        return ObjectId(a.m_object_id) > ObjectId(b.m_object_id);
#endif
    }

    bool operator <(const object_id& a, const object_id& b) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return *reinterpret_cast<const ObjectId*>(&a.m_object_id) < *reinterpret_cast<const ObjectId*>(&b.m_object_id);
#else
        return ObjectId(a.m_object_id) < ObjectId(b.m_object_id);
#endif
    }

    bool operator >=(const object_id& a, const object_id& b) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return *reinterpret_cast<const ObjectId*>(&a.m_object_id) >= *reinterpret_cast<const ObjectId*>(&b.m_object_id);
#else
        return ObjectId(a.m_object_id) >= ObjectId(b.m_object_id);
#endif
    }

    bool operator <=(const object_id& a, const object_id& b) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return *reinterpret_cast<const ObjectId*>(&a.m_object_id) <= *reinterpret_cast<const ObjectId*>(&b.m_object_id);
#else
        return ObjectId(a.m_object_id) <= ObjectId(b.m_object_id);
#endif
    }
}