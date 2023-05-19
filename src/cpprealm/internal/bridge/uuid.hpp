#ifndef CPP_REALM_BRIDGE_UUID_HPP
#define CPP_REALM_BRIDGE_UUID_HPP

#include <string_view>
#include <cpprealm/internal/bridge/utils.hpp>

namespace realm {
    struct uuid;
    class UUID;
}

namespace realm::internal::bridge {
    struct uuid : core_binding<UUID> {
        uuid();
        uuid(const uuid& other) ;
        uuid& operator=(const uuid& other) ;
        uuid(uuid&& other);
        uuid& operator=(uuid&& other);
        ~uuid();
        uuid(const UUID&); //NOLINT(google-explicit-constructor);
        explicit uuid(const std::string&);
        uuid(const struct ::realm::uuid&); //NOLINT(google-explicit-constructor);
        operator UUID() const final; //NOLINT(google-explicit-constructor);
        operator ::realm::uuid() const; //NOLINT(google-explicit-constructor);
        [[nodiscard]] std::string to_string() const;
        [[nodiscard]] std::string to_base64() const;
        [[nodiscard]] std::array<uint8_t, 16> to_bytes() const;
    private:
#ifdef __i386__
        std::aligned_storage<16, 1>::type m_uuid[1];
#elif __x86_64__
        std::aligned_storage<16, 1>::type m_uuid[1];
#elif __arm__
        std::aligned_storage<16, 1>::type m_uuid[1];
#elif __aarch64__
        std::aligned_storage<16, 1>::type m_uuid[1];
#elif _WIN32
        std::aligned_storage<1, 1>::type m_uuid[1];
#endif
        friend bool operator ==(const uuid&, const uuid&);
        friend bool operator !=(const uuid&, const uuid&);
        friend bool operator >(const uuid&, const uuid&);
        friend bool operator <(const uuid&, const uuid&);
        friend bool operator >=(const uuid&, const uuid&);
        friend bool operator <=(const uuid&, const uuid&);
    };

    bool operator ==(const uuid&, const uuid&);
    bool operator !=(const uuid&, const uuid&);
    bool operator >(const uuid&, const uuid&);
    bool operator <(const uuid&, const uuid&);
    bool operator >=(const uuid&, const uuid&);
    bool operator <=(const uuid&, const uuid&);
}

#endif //CPP_REALM_BRIDGE_UUID_HPP
