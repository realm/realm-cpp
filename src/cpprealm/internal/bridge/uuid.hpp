#ifndef CPP_REALM_BRIDGE_UUID_HPP
#define CPP_REALM_BRIDGE_UUID_HPP

#include <string_view>

namespace realm {
    struct uuid;
    class UUID;
}

namespace realm::internal::bridge {
    struct uuid {
        uuid();
        uuid(const UUID&);
        uuid(const std::string&);
        uuid(const struct ::realm::uuid&);
        [[nodiscard]] std::string to_string() const;
        operator UUID() const;
    private:
        unsigned char m_uuid[16]{};
    };
}

#endif //CPP_REALM_BRIDGE_UUID_HPP
