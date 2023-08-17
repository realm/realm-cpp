#include <cpprealm/internal/bridge/binary.hpp>
#include <cpprealm/internal/bridge/utils.hpp>

#include <realm/binary_data.hpp>

namespace realm::internal::bridge {
#ifdef __i386__
        static_assert(SizeCheck<8, sizeof(OwnedBinaryData)>{});
        static_assert(SizeCheck<4, alignof(OwnedBinaryData)>{});
#elif __x86_64__
        static_assert(SizeCheck<16, sizeof(OwnedBinaryData)>{});
        static_assert(SizeCheck<8, alignof(OwnedBinaryData)>{});
#elif __arm__
        static_assert(SizeCheck<8, sizeof(OwnedBinaryData)>{});
        static_assert(SizeCheck<4, alignof(OwnedBinaryData)>{});
#elif __aarch64__
        static_assert(SizeCheck<16, sizeof(OwnedBinaryData)>{});
        static_assert(SizeCheck<8, alignof(OwnedBinaryData)>{});
#elif _WIN32
        static_assert(SizeCheck<16, sizeof(OwnedBinaryData)>{});
        static_assert(SizeCheck<8, alignof(OwnedBinaryData)>{});
#endif

    char binary::operator[](size_t i) const noexcept {
        return reinterpret_cast<const OwnedBinaryData*>(&m_data)->get().operator[](i);
    }
    binary::binary() {
        new (&m_data) OwnedBinaryData();
    }
    binary::binary(const binary& other) {
        new (&m_data) OwnedBinaryData(*reinterpret_cast<const OwnedBinaryData*>(&other.m_data));
    }
    binary& binary::operator=(const binary& other) {
        *reinterpret_cast<OwnedBinaryData*>(&m_data) = *reinterpret_cast<const OwnedBinaryData*>(&other.m_data);
        return *this;
    }
    binary::binary(binary&& other) {
        new (&m_data) OwnedBinaryData(std::move(*reinterpret_cast<OwnedBinaryData*>(&other.m_data)));
    }
    binary& binary::operator=(binary&& other) {
        *reinterpret_cast<OwnedBinaryData*>(&m_data) = std::move(*reinterpret_cast<OwnedBinaryData*>(&other.m_data));
        return *this;
    }
    binary::~binary() {
        reinterpret_cast<OwnedBinaryData*>(&m_data)->~OwnedBinaryData();
    }
    binary::binary(const realm::BinaryData &v) {
        new (&m_data) OwnedBinaryData(v);
    }

    binary::binary(const std::vector<uint8_t> &v) {
        if (v.empty()) {
            new (&m_data) OwnedBinaryData("", 0);
        } else {
            new (&m_data) OwnedBinaryData(reinterpret_cast<const char *>(v.data()), v.size());
        }
    }

    binary::operator OwnedBinaryData() const {
        return *reinterpret_cast<const OwnedBinaryData*>(&m_data);
    }
    binary::operator BinaryData() const {
        return reinterpret_cast<const OwnedBinaryData*>(&m_data)->get();
    }
    size_t binary::size() const {
        return reinterpret_cast<const OwnedBinaryData*>(&m_data)->size();
    }
    const char *binary::data() const {
        return reinterpret_cast<const OwnedBinaryData*>(&m_data)->data();
    }
    bool operator ==(binary const& lhs, binary const& rhs) {
        return static_cast<OwnedBinaryData>(lhs).get() == static_cast<OwnedBinaryData>(rhs).get();
    }
    binary::operator std::vector<uint8_t>() const {
        std::vector<uint8_t> v;
        for (size_t i = 0; i < size(); i++) {
            v.push_back(operator[](i));
        }
        return v;
    }
}
