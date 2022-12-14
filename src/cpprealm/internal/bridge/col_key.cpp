#include <cpprealm/internal/bridge/col_key.hpp>
#include <realm/obj.hpp>
#include <cpprealm/internal/bridge/utils.hpp>

namespace realm::internal::bridge {
    static_assert(SizeCheck<8, sizeof(ColKey)>{});
    col_key::col_key() noexcept {
        new (&m_col_key) ColKey();
    }
    col_key::col_key(int64_t v) {
        new (&m_col_key) ColKey(v);
    }
    col_key::col_key(const realm::ColKey &v) {
        new (&m_col_key) ColKey(v);
    }

    int64_t col_key::value() const {
        return reinterpret_cast<const ColKey*>(m_col_key)->value;
    }

    col_key::operator ColKey() const {
        return *reinterpret_cast<const ColKey*>(m_col_key);
    }
}
