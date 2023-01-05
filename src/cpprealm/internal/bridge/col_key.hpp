#ifndef CPP_REALM_BRIDGE_COL_KEY_HPP
#define CPP_REALM_BRIDGE_COL_KEY_HPP

#include <cinttypes>

namespace realm {
    class ColKey;
}
namespace realm::internal::bridge {
    struct col_key {
        col_key() noexcept;
        col_key(int64_t); //NOLINT(google-explicit-constructor)
        col_key(const ColKey&); //NOLINT(google-explicit-constructor)
        operator ColKey() const; //NOLINT(google-explicit-constructor)
        [[nodiscard]] int64_t value() const;
    private:
        unsigned char m_col_key[8]{};
    };
}

#endif //CPP_REALM_BRIDGE_COL_KEY_HPP
