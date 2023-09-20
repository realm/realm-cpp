#ifndef CPP_REALM_BRIDGE_BINARY_HPP
#define CPP_REALM_BRIDGE_BINARY_HPP

#include <vector>
#include <cpprealm/internal/bridge/utils.hpp>

namespace realm {
    class BinaryData;
    class OwnedBinaryData;
}
namespace realm::internal::bridge {
    struct binary {
        binary();
        binary(const binary& other) ;
        binary& operator=(const binary& other) ;
        binary(binary&& other);
        binary& operator=(binary&& other);
        ~binary();
        binary(const BinaryData&); //NOLINT(google-explicit-constructor)
        binary(const std::vector<uint8_t>&); //NOLINT(google-explicit-constructor)
        [[nodiscard]] const char* data() const;
        [[nodiscard]] size_t size() const;
        operator std::vector<uint8_t>() const; //NOLINT(google-explicit-constructor)
        operator OwnedBinaryData() const; //NOLINT(google-explicit-constructor)
        operator BinaryData() const; //NOLINT(google-explicit-constructor)
        char operator[](size_t i) const noexcept;
    private:
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
            storage::OwnedBinaryData m_data[1];
#elif __i386__
            std::aligned_storage<8, 4>::type m_data[1];
#elif __x86_64__
            std::aligned_storage<16, 8>::type m_data[1];
#elif __arm__
            std::aligned_storage<8, 4>::type m_data[1];
#elif __aarch64__
            std::aligned_storage<16, 8>::type m_data[1];
#elif _WIN32
        std::aligned_storage<16, 8>::type m_data[1];
#endif
    };

    bool operator ==(const binary& lhs, const binary& rhs);
    bool operator !=(const binary& lhs, const binary& rhs);
}

#endif //CPP_REALM_BRIDGE_BINARY_HPP
