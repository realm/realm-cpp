#ifndef CPP_REALM_BRIDGE_OBJ_KEY_HPP
#define CPP_REALM_BRIDGE_OBJ_KEY_HPP
#include <cinttypes>
#include <type_traits>

namespace realm {
    struct ObjKey;
    struct ObjLink;
}

namespace realm::internal::bridge {
    struct obj_key {
        obj_key(const ObjKey&);
        obj_key(int64_t);
        obj_key();
        obj_key(const obj_key& other) ;
        obj_key& operator=(const obj_key& other) ;
        obj_key(obj_key&& other);
        obj_key& operator=(obj_key&& other);
        ~obj_key();
        operator ObjKey() const;
    private:
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        storage::ObjKey m_obj_key[1];
#elif __i386__
        std::aligned_storage<8, 4>::type m_obj_key[1];
#elif __x86_64__
        std::aligned_storage<8, 8>::type m_obj_key[1];
#elif __arm__
        std::aligned_storage<8, 8>::type m_obj_key[1];
#elif __aarch64__
        std::aligned_storage<8, 8>::type m_obj_key[1];
#elif _WIN32
        std::aligned_storage<8, 8>::type m_obj_key[1];
#endif
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
#elif __i386__
        std::aligned_storage<12, 4>::type m_obj_link[1];
#elif __x86_64__
        std::aligned_storage<16, 8>::type m_obj_link[1];
#elif __arm__
        std::aligned_storage<16, 8>::type m_obj_link[1];
#elif __aarch64__
        std::aligned_storage<16, 8>::type m_obj_link[1];
#elif _WIN32
        std::aligned_storage<16, 8>::type m_obj_link[1];
#endif
    };

    bool operator==(const obj_link &, const obj_link &);

    bool operator!=(const obj_link &, const obj_link &);
}


#endif //CPP_REALM_BRIDGE_OBJ_KEY_HPP
