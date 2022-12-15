#include <cpprealm/persisted_binary.hpp>
#include <cpprealm/internal/bridge/binary.hpp>

namespace realm {
    void persisted<std::vector<uint8_t>>::push_back(uint8_t v) {
        if (is_managed()) {
            auto v2 = deserialize(m_object->obj().get<internal::bridge::binary>(managed));
            v2.push_back(v);
            m_object->obj().set(managed, serialize(v2));
        } else {
            unmanaged.push_back(v);
        }
    }

    uint8_t persisted<std::vector<uint8_t>>::operator[](size_t idx) {
        if (is_managed()) {
            return m_object->obj().get<internal::bridge::binary>(managed)[idx];
        } else {
            return unmanaged[idx];
        }
    }

    size_t persisted<std::vector<uint8_t>>::size() const {
        if (is_managed()) {
            return m_object->obj().get<internal::bridge::binary>(managed).size();
        } else {
            return unmanaged.size();
        }
    }

    internal::bridge::binary persisted<std::vector<uint8_t>>::serialize(const std::vector<uint8_t> &v) {
        return v;
    }

    std::vector<uint8_t> persisted<std::vector<uint8_t>>::deserialize(const internal::bridge::binary& v) {
        std::vector<uint8_t> v2;
        for (size_t i = 0; i < v.size(); i++) {
            v2.push_back(v.data()[i]);
        }
        return v2;
    }
}
