#ifndef CPP_REALM_BRIDGE_BINARY_HPP
#define CPP_REALM_BRIDGE_BINARY_HPP

#include <vector>

namespace realm {
    class BinaryData;
}
namespace realm::internal::bridge {
    struct binary {
        binary(const BinaryData&);
        binary(const std::vector<uint8_t>&);
        [[nodiscard]] const char* data() const;
        [[nodiscard]] size_t size() const;
        operator BinaryData() const;
    private:
        unsigned char m_data[16]{};
    };

    bool operator ==(const binary& lhs, const binary& rhs);
}

#endif //CPP_REALM_BRIDGE_BINARY_HPP
