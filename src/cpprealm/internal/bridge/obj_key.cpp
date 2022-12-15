#include <cpprealm/internal/bridge/obj_key.hpp>
#include <cpprealm/internal/bridge/utils.hpp>

#include <realm/obj.hpp>

namespace realm::internal::bridge {
    static_assert(SizeCheck<8, sizeof(ObjKey)>{});

    obj_key::obj_key() {
        new (&m_obj_key) ObjKey();
    }

    obj_key::obj_key(int64_t v) {
        new (&m_obj_key) ObjKey(v);
    }

    obj_key::obj_key(const ObjKey& v) {
        new (&m_obj_key) ObjKey(v);
    }

    obj_key::operator ObjKey() const {
        return *reinterpret_cast<const ObjKey*>(m_obj_key);
    }

    bool operator==(obj_key const& lhs, obj_key const& rhs) {
        return static_cast<ObjKey>(lhs) == static_cast<ObjKey>(rhs);
    }
}