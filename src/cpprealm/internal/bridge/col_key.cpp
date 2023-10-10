#include <cpprealm/internal/bridge/col_key.hpp>
#include <realm/keys.hpp>

namespace realm::internal::bridge {
    col_key::col_key() noexcept {
        m_col_key = ColKey::null_value;
    }
    col_key::col_key(const col_key& other) {
        m_col_key = other.m_col_key;
    }
    col_key& col_key::operator=(const col_key& other) {
        m_col_key = other.m_col_key;
        return *this;
    }
    col_key::col_key(col_key&& other) {
        m_col_key = std::move(other.m_col_key);
    }
    col_key& col_key::operator=(col_key&& other) {
        m_col_key = std::move(other.m_col_key);
        return *this;
    }

    col_key::col_key(int64_t v) {
        m_col_key = v;
    }
    col_key::col_key(const ::realm::ColKey &v) {
        m_col_key = v.value;
    }

    int64_t col_key::value() const {
        return m_col_key;
    }
    col_key::operator bool() const {
        return ColKey(m_col_key).operator bool();
    }
    col_key::operator ColKey() const {
        return ColKey(m_col_key);
    }
}
