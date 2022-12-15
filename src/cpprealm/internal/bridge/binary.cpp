#include <cpprealm/internal/bridge/binary.hpp>
#include <cpprealm/internal/bridge/utils.hpp>

#include <realm/binary_data.hpp>

namespace realm::internal::bridge {
    static_assert(SizeCheck<16, sizeof(BinaryData)>{});

    char binary::operator[](size_t i) const noexcept {
        return reinterpret_cast<const BinaryData*>(m_data)->operator[](i);
    }
    binary::binary() {
        new (&m_data) BinaryData();
    }
    binary::binary(const realm::BinaryData &v) {
        new (&m_data) BinaryData(v);
    }
    binary::binary(const std::vector<uint8_t> &v) {
        if (v.empty()) {
            new (&m_data) BinaryData("", 0);
        } else {
            new (&m_data) BinaryData(reinterpret_cast<const char *>(v.data()), v.size());
        }
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
    bool operator ==(binary const& lhs, binary const& rhs) {
        return static_cast<BinaryData>(lhs) == static_cast<BinaryData>(rhs);
    }
    binary::operator std::vector<uint8_t>() const {
        std::vector<uint8_t> v;
        for (size_t i = 0; i < size(); i++) {
            v.push_back(operator[](i));
        }
        return v;
    }
}