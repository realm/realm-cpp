#ifndef CPP_REALM_BRIDGE_OBJ_KEY_HPP
#define CPP_REALM_BRIDGE_OBJ_KEY_HPP
#include <cinttypes>

namespace realm {
    class ObjKey;
}

namespace realm::internal::bridge {
    struct obj_key {
        obj_key(const ObjKey&);
        obj_key(int64_t);
        obj_key();
        operator ObjKey() const;
    private:
        unsigned char m_obj_key[8]{};
    };

    bool operator==(const obj_key &, const obj_key &);

    bool operator!=(const obj_key &, const obj_key &);
}


#endif //CPP_REALM_BRIDGE_OBJ_KEY_HPP
