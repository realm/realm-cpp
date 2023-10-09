#ifndef CPP_REALM_BRIDGE_OBJ_KEY_HPP
#define CPP_REALM_BRIDGE_OBJ_KEY_HPP
#include <cinttypes>
#include <memory>
#include <type_traits>
#include <cpprealm/internal/bridge/utils.hpp>

namespace realm {
    struct ObjKey;
    struct ObjLink;
}

namespace realm::internal::bridge {
    struct obj_key {
        obj_key(const ObjKey&);
        obj_key(int64_t);
        obj_key();
        obj_key(const obj_key& other) = default;
        obj_key& operator=(const obj_key& other) = default;
        obj_key(obj_key&& other) = default;
        obj_key& operator=(obj_key&& other) = default;
        ~obj_key() = default;
        operator ObjKey() const;
    private:
        int64_t m_obj_key;
    };

    bool operator==(const obj_key &, const obj_key &);

    bool operator!=(const obj_key &, const obj_key &);
}

namespace realm::internal::bridge {
    struct obj_link {
        obj_link(const ObjLink&);
        obj_link();
        obj_link(const obj_link& other) ;
        obj_link& operator=(const obj_link& other) ;
        obj_link(obj_link&& other);
        obj_link& operator=(obj_link&& other);
        ~obj_link();
        operator ObjLink() const;
        obj_key get_obj_key();
    private:
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        storage::ObjLink m_obj_link[1];
#else
        std::shared_ptr<ObjLink> m_obj_link;
#endif
    };

    bool operator==(const obj_link &, const obj_link &);

    bool operator!=(const obj_link &, const obj_link &);
}


#endif //CPP_REALM_BRIDGE_OBJ_KEY_HPP
