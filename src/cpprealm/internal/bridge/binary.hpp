#ifndef CPP_REALM_BRIDGE_BINARY_HPP
#define CPP_REALM_BRIDGE_BINARY_HPP

#include <vector>
#include <cpprealm/internal/bridge/utils.hpp>

namespace realm {
    class BinaryData;
}
namespace realm::internal::bridge {
    struct binary : core_binding<BinaryData> {
        binary(const BinaryData&); //NOLINT(google-explicit-constructor)
        binary(const std::vector<uint8_t>&); //NOLINT(google-explicit-constructor)
        [[nodiscard]] const char* data() const;
        [[nodiscard]] size_t size() const;
        operator std::vector<uint8_t>() const; //NOLINT(google-explicit-constructor)
        operator BinaryData() const; //NOLINT(google-explicit-constructor)
        char operator[](size_t i) const noexcept;
    private:
        unsigned char m_data[16]{};
    };

    bool operator ==(const binary& lhs, const binary& rhs);
}

#endif //CPP_REALM_BRIDGE_BINARY_HPP
