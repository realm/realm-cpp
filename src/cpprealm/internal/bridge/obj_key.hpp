#ifndef CPP_REALM_BRIDGE_OBJ_KEY_HPP
#define CPP_REALM_BRIDGE_OBJ_KEY_HPP
#include <cinttypes>
#include <type_traits>

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
#ifdef __i386__
        std::aligned_storage<8, 4>::type m_obj_key[1];
#elif __x86_64__
        std::aligned_storage<8, 8>::type m_obj_key[1];
#elif __arm__
        std::aligned_storage<8, 8>::type m_obj_key[1];
#elif __aarch64__
        std::aligned_storage<8, 8>::type m_obj_key[1];
#else
        std::aligned_storage<8, 4>::type m_obj_key[1];
#endif
    };

    bool operator==(const obj_key &, const obj_key &);

    bool operator!=(const obj_key &, const obj_key &);
}


#endif //CPP_REALM_BRIDGE_OBJ_KEY_HPP
