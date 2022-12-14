#include <cpprealm/internal/bridge/binary.hpp>
#include <cpprealm/internal/bridge/utils.hpp>

#include <realm/binary_data.hpp>

namespace realm::internal::bridge {
    static_assert(SizeCheck<16, sizeof(BinaryData)>{});

    char binary::operator[](size_t i) const noexcept {
        return reinterpret_cast<const BinaryData*>(m_data)->operator[](i);
    }
    binary::binary(const realm::BinaryData &v) {
        new (&m_data) BinaryData(v);
    }
    binary::binary(const std::vector<uint8_t> &v) {
        new (&m_data) BinaryData(reinterpret_cast<const char *>(v.data()), v.size());
    }

    binary::operator BinaryData() const {
        return *reinterpret_cast<const BinaryData*>(m_data);
    }
    size_t binary::size() const {
        return reinterpret_cast<const BinaryData*>(m_data)->size();
    }

    const char *binary::data() const {
        return reinterpret_cast<const BinaryData*>(m_data)->data();
    }
}