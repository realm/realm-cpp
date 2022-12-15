#ifndef CPP_REALM_BRIDGE_MIXED_HPP
#define CPP_REALM_BRIDGE_MIXED_HPP

#include <string>
#include <cpprealm/internal/bridge/utils.hpp>
#include <cpprealm/internal/bridge/property.hpp>
#include <cpprealm/internal/bridge/binary.hpp>
#include <cpprealm/internal/bridge/uuid.hpp>
#include <cpprealm/internal/bridge/timestamp.hpp>
#include <cpprealm/internal/bridge/obj_key.hpp>

namespace realm {
    class Mixed;
}

namespace realm::internal::bridge {
    struct mixed : core_binding<Mixed> {
        explicit mixed(const std::string&);
        mixed(const int&); //NOLINT(google-explicit-constructor)
        mixed(const int64_t&); //NOLINT(google-explicit-constructor)
        mixed(const double&); //NOLINT(google-explicit-constructor)
        mixed(const bool&); //NOLINT(google-explicit-constructor)
        mixed(const struct uuid&); //NOLINT(google-explicit-constructor)
        mixed(const struct timestamp&); //NOLINT(google-explicit-constructor)
        mixed(const struct obj_key&); //NOLINT(google-explicit-constructor)
        mixed(const struct binary&); //NOLINT(google-explicit-constructor)
        mixed(const mixed&) = default;
        mixed(const Mixed&); //NOLINT(google-explicit-constructor)

        operator std::string() const; //NOLINT(google-explicit-constructor)
        operator int64_t() const; //NOLINT(google-explicit-constructor)
        operator double() const; //NOLINT(google-explicit-constructor)
        operator bool() const; //NOLINT(google-explicit-constructor)
        operator bridge::uuid() const; //NOLINT(google-explicit-constructor)
        operator bridge::timestamp() const; //NOLINT(google-explicit-constructor)
        operator bridge::obj_key() const; //NOLINT(google-explicit-constructor)
        operator bridge::binary() const; //NOLINT(google-explicit-constructor)

        explicit operator Mixed() const final;

        [[nodiscard]] property::type type() const noexcept;
    private:
        unsigned char m_mixed[24]{};
    };
}


#endif //CPP_REALM_BRIDGE_MIXED_HPP
