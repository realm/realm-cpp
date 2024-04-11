#include <cpprealm/internal/bridge/binary.hpp>

#include <realm/binary_data.hpp>

namespace realm::internal::bridge {
    char binary::operator[](size_t i) const noexcept {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const OwnedBinaryData*>(&m_data)->get().operator[](i);
#else
        return BinaryData{reinterpret_cast<const char*>(m_data), m_size}.operator[](i);
#endif
    }
    binary::binary() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_data) OwnedBinaryData();
#else
        m_size = 0;
        m_data = new char[m_size];
#endif
    }
    binary::binary(const binary& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_data) OwnedBinaryData(*reinterpret_cast<const OwnedBinaryData*>(&other.m_data));
#else
        m_size = other.m_size;
        m_data = new char[m_size];
        memcpy(m_data, other.m_data, m_size);
#endif
    }
    binary& binary::operator=(const binary& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        *reinterpret_cast<OwnedBinaryData*>(&m_data) = *reinterpret_cast<const OwnedBinaryData*>(&other.m_data);

#else
        delete[] m_data;
        m_size = other.m_size;
        m_data = new char[m_size];
        memcpy(m_data, other.m_data, m_size);
#endif
        return *this;
    }
    binary::binary(binary&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_data) OwnedBinaryData(std::move(*reinterpret_cast<OwnedBinaryData*>(&other.m_data)));
#else
        m_size = std::move(other.m_size);
        m_data = new char[m_size];
        memmove(m_data, other.m_data, m_size);
#endif
    }
    binary& binary::operator=(binary&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        *reinterpret_cast<OwnedBinaryData*>(&m_data) = std::move(*reinterpret_cast<OwnedBinaryData*>(&other.m_data));
#else
        delete[] m_data;
        m_size = std::move(other.m_size);
        m_data = new char[m_size];
        memmove(m_data, other.m_data, m_size);
#endif
        return *this;
    }
    binary::~binary() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<OwnedBinaryData*>(&m_data)->~OwnedBinaryData();
#else
        delete[] m_data;
#endif
    }
    binary::binary(const realm::BinaryData &v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_data) OwnedBinaryData(v);
#else
        m_size = v.size();
        m_data = new char[m_size];
        memcpy(m_data, v.data(), m_size);
#endif
    }

    binary::binary(const std::vector<uint8_t> &v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (v.empty()) {
            new (&m_data) OwnedBinaryData("", 0);
        } else {
            new (&m_data) OwnedBinaryData(reinterpret_cast<const char *>(v.data()), v.size());
        }
#else
        m_size = v.size();
        m_data = new char[m_size];
        memcpy(m_data, v.data(), m_size);
#endif
    }

    binary::operator OwnedBinaryData() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return *reinterpret_cast<const OwnedBinaryData*>(&m_data);
#else
        return {m_data, m_size};
#endif
    }
    binary::operator BinaryData() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const OwnedBinaryData*>(&m_data)->get();
#else
        return {m_data, m_size};
#endif
    }
    size_t binary::size() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const OwnedBinaryData*>(&m_data)->size();
#else
        return m_size;
#endif
    }
    const char *binary::data() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const OwnedBinaryData*>(&m_data)->data();
#else
        return m_data;
#endif
    }
    bool operator ==(binary const& lhs, binary const& rhs) {
        return lhs.operator OwnedBinaryData().get() == rhs.operator OwnedBinaryData().get();
    }
    bool operator !=(binary const& lhs, binary const& rhs) {
        return lhs.operator OwnedBinaryData().get() != rhs.operator OwnedBinaryData().get();
    }
    binary::operator std::vector<uint8_t>() const {
        std::vector<uint8_t> v;
        for (size_t i = 0; i < size(); i++) {
            v.push_back(operator[](i));
        }
        return v;
    }
}
