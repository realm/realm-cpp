#ifndef REALM_COL_KEY_HPP
#define REALM_COL_KEY_HPP

#include <cinttypes>

namespace realm {
    struct ColKey;
}
namespace realm::internal::bridge {
    struct col_key {
        col_key() noexcept;
        col_key(const col_key& other) ;
        col_key& operator=(const col_key& other) ;
        col_key(col_key&& other);
        col_key& operator=(col_key&& other);
        ~col_key() = default;
        col_key(int64_t); //NOLINT(google-explicit-constructor)
        col_key(const ColKey&); //NOLINT(google-explicit-constructor)
        operator ColKey() const; //NOLINT(google-explicit-constructor)
        operator bool() const;
        [[nodiscard]] int64_t value() const;
    private:
        uint64_t m_col_key;
    };
}

#endif //REALM_COL_KEY_HPP
