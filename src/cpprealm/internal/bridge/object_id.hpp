#ifndef CPP_REALM_BRIDGE_OBJECT_ID_HPP
#define CPP_REALM_BRIDGE_OBJECT_ID_HPP

#include <cpprealm/internal/bridge/utils.hpp>

namespace realm {
    struct object_id;
    class ObjectId;
}

namespace realm::internal::bridge {
    struct object_id : core_binding<ObjectId> {
        object_id();
        object_id(const object_id& other) ;
        object_id& operator=(const object_id& other) ;
        object_id(object_id&& other);
        object_id& operator=(object_id&& other);
        ~object_id();
        object_id(const ObjectId&); //NOLINT(google-explicit-constructor);
        explicit object_id(const std::string&);
        object_id(const struct ::realm::object_id&); //NOLINT(google-explicit-constructor);
        operator ObjectId() const final; //NOLINT(google-explicit-constructor);
        operator ::realm::object_id() const; //NOLINT(google-explicit-constructor);
        [[nodiscard]] std::string to_string() const;
        [[nodiscard]] static object_id generate();
    private:
#ifdef __i386__
        std::aligned_storage<12, 1>::type m_object_id[1];
#elif __x86_64__
        std::aligned_storage<12, 1>::type m_object_id[1];
#elif __arm__
        std::aligned_storage<12, 1>::type m_object_id[1];
#elif __aarch64__
        std::aligned_storage<12, 1>::type m_object_id[1];
#elif _WIN32
        std::aligned_storage<1, 1>::type m_object_id[1];
#endif
        friend bool operator ==(const object_id&, const object_id&);
        friend bool operator !=(const object_id&, const object_id&);
        friend bool operator >(const object_id&, const object_id&);
        friend bool operator <(const object_id&, const object_id&);
        friend bool operator >=(const object_id&, const object_id&);
        friend bool operator <=(const object_id&, const object_id&);
    };

    bool operator ==(const object_id&, const object_id&);
    bool operator !=(const object_id&, const object_id&);
    bool operator >(const object_id&, const object_id&);
    bool operator <(const object_id&, const object_id&);
    bool operator >=(const object_id&, const object_id&);
    bool operator <=(const object_id&, const object_id&);
}

#endif //CPP_REALM_BRIDGE_OBJECT_ID_HPP

