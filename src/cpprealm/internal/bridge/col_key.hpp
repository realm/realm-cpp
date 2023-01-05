#ifndef REALM_COL_KEY_HPP
#define REALM_COL_KEY_HPP

#include <cinttypes>
#include <type_traits>

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
#ifdef __i386__
        std::aligned_storage<8, 4>::type m_col_key[1];
#elif __x86_64__
        std::aligned_storage<8, 8>::type m_col_key[1];
    #elif __arm__
        std::aligned_storage<8, 8>::type m_col_key[1];
    #elif __aarch64__
        std::aligned_storage<8, 8>::type m_col_key[1];
    #else
        std::aligned_storage<8, 4>::type m_col_key[1];
#endif
    };
}

#endif //REALM_COL_KEY_HPP
