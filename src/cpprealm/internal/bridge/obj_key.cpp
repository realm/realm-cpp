#include <cpprealm/internal/bridge/obj_key.hpp>
#include <cpprealm/internal/bridge/utils.hpp>

#include <realm/keys.hpp>

namespace realm::internal::bridge {
    obj_key::obj_key() {
        m_obj_key = ObjKey().value;
    }

    obj_key::obj_key(int64_t v) {
        m_obj_key = v;
    }

    obj_key::obj_key(const ObjKey& v) {
        m_obj_key = v.value;
    }

    obj_key::operator ObjKey() const {
        return ObjKey(m_obj_key);
    }

    bool operator==(obj_key const& lhs, obj_key const& rhs) {
        return lhs.operator ObjKey() == rhs.operator ObjKey();
    }

    bool operator!=(obj_key const &lhs, obj_key const &rhs) {
        return lhs.operator ObjKey() != rhs.operator ObjKey();
    }

    obj_link::obj_link() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_obj_link) ObjLink();
#else
        m_obj_link = std::make_shared<ObjLink>();
#endif
    }
    
    obj_link::obj_link(const obj_link& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_obj_link) ObjLink(*reinterpret_cast<const ObjLink*>(&other.m_obj_link));
#else
        m_obj_link = other.m_obj_link;
#endif
    }

    obj_link& obj_link::operator=(const obj_link& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<ObjLink*>(&m_obj_link) = *reinterpret_cast<const ObjLink*>(&other.m_obj_link);
        }
#else
        m_obj_link = other.m_obj_link;
#endif
        return *this;
    }

    obj_link::obj_link(obj_link&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_obj_link) ObjLink(std::move(*reinterpret_cast<ObjLink*>(&other.m_obj_link)));
#else
        m_obj_link = std::move(other.m_obj_link);
#endif
    }

    obj_link& obj_link::operator=(obj_link&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<ObjLink*>(&m_obj_link) = std::move(*reinterpret_cast<ObjLink*>(&other.m_obj_link));
        }
#else
        m_obj_link = std::move(other.m_obj_link);
#endif
        return *this;
    }

    obj_link::~obj_link() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<ObjLink*>(&m_obj_link)->~ObjLink();
#endif
    }
    
    obj_link::obj_link(const ObjLink& v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_obj_link) ObjLink(v);
#else
        m_obj_link = std::make_shared<ObjLink>(v);
#endif
    }

    obj_link::operator ObjLink() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return *reinterpret_cast<const ObjLink*>(&m_obj_link);
#else
        return *m_obj_link;
#endif
    }

    obj_key obj_link::get_obj_key() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const ObjLink*>(&m_obj_link)->get_obj_key();
#else
        return m_obj_link->get_obj_key();
#endif
    }

    bool operator==(obj_link const& lhs, obj_link const& rhs) {
        return static_cast<ObjLink>(lhs) == static_cast<ObjLink>(rhs);
    }

    bool operator!=(obj_link const &lhs, obj_link const &rhs) {
        return static_cast<ObjLink>(lhs) != static_cast<ObjLink>(rhs);
    }
}
