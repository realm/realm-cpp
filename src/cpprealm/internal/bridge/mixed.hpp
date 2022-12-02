#ifndef CPP_REALM_BRIDGE_MIXED_HPP
#define CPP_REALM_BRIDGE_MIXED_HPP

#include <string>
#include <cpprealm/internal/bridge/property.hpp>
#include <cpprealm/internal/bridge/binary.hpp>
#include <cpprealm/internal/bridge/uuid.hpp>
#include <cpprealm/internal/bridge/timestamp.hpp>
#include <cpprealm/internal/bridge/obj_key.hpp>

namespace realm {
    class Mixed;
}

namespace realm::internal::bridge {
    struct mixed {
        mixed(const std::string_view&);
        mixed(const int64_t&);
        mixed(const struct uuid&);
        mixed(const struct timestamp&);
        mixed(const struct obj_key&);
        mixed(const struct binary&);
        mixed(const mixed&) = default;
        mixed(const Mixed&);

        operator std::string_view() const;
        operator int64_t() const;
        operator bridge::uuid() const;
        operator bridge::timestamp() const;
        operator bridge::obj_key() const;
        operator bridge::binary() const;

        explicit operator Mixed() const;

        property::type type() const noexcept;
    private:
        unsigned char m_mixed[24]{};
    };
}


#endif //CPP_REALM_BRIDGE_MIXED_HPP
