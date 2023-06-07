#include <cpprealm/experimental/managed_binary.hpp>
#include <cpprealm/persisted.hpp>

namespace realm::experimental {
    std::vector<uint8_t> managed<std::vector<uint8_t>>::value() const {
        return m_obj->template get<realm::internal::bridge::binary>(m_key);
    }

    [[nodiscard]] managed<std::vector<uint8_t>>::operator std::vector<uint8_t>() const {
        return m_obj->template get<realm::internal::bridge::binary>(m_key);
    }

    std::vector<uint8_t> managed<std::vector<uint8_t>>::operator*() const {
        return value();
    }

    void managed<std::vector<uint8_t>>::push_back(uint8_t v) {
        auto v2 = m_obj->template get<internal::bridge::binary>(m_key).operator std::vector<uint8_t>();
        v2.push_back(v);
        m_obj->template set<internal::bridge::binary>(m_key, v2);
    }

    uint8_t managed<std::vector<uint8_t>>::operator[](uint8_t idx) const {
        return m_obj->template get<internal::bridge::binary>(m_key)[idx];
    }

    size_t managed<std::vector<uint8_t>>::size() const {
        return m_obj->template get<internal::bridge::binary>(m_key).size();
    }

    __cpprealm_build_experimental_query(==, equal, std::vector<uint8_t>);
    __cpprealm_build_experimental_query(!=, not_equal, std::vector<uint8_t>);

    // MARK: Optional

    std::optional<std::vector<uint8_t>> managed<std::optional<std::vector<uint8_t>>>::value() const {
        return m_obj->get_optional<realm::internal::bridge::binary>(m_key);
    }

    [[nodiscard]] managed<std::optional<std::vector<uint8_t>>>::operator std::optional<std::vector<uint8_t>>() const {
        return m_obj->get_optional<realm::internal::bridge::binary>(m_key);
    }

    void managed<std::optional<std::vector<uint8_t>>>::box::push_back(uint8_t v) {
        if (auto b = m_parent.get().m_obj->get_optional<internal::bridge::binary>(m_parent.get().m_key)) {
            auto v2 = b->operator std::vector<uint8_t>();
            v2.push_back(v);
            m_parent.get().m_obj->template set<internal::bridge::binary>(m_parent.get().m_key, v2);
        }

    }

    uint8_t managed<std::optional<std::vector<uint8_t>>>::box::operator[](uint8_t idx) const {
        auto b = m_parent.get().m_obj->get_optional<internal::bridge::binary>(m_parent.get().m_key);
        return b->operator[](idx);
    }

    size_t managed<std::optional<std::vector<uint8_t>>>::box::size() const {
        return m_parent.get().m_obj->template get<internal::bridge::binary>(m_parent.get().m_key).size();
    }

    __cpprealm_build_optional_experimental_query(==, equal, std::vector<uint8_t>);
    __cpprealm_build_optional_experimental_query(!=, not_equal, std::vector<uint8_t>);
}
