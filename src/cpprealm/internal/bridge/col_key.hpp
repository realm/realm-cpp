#ifndef REALM_COL_KEY_HPP
#define REALM_COL_KEY_HPP

#include <cinttypes>

namespace realm {
    class ColKey;
}
namespace realm::internal::bridge {
    struct col_key {
        col_key() noexcept;
        col_key(int64_t);
        col_key(const ColKey&);
        int64_t value() const;
    private:
        unsigned char m_col_key[8]{};
    };
}

#endif //REALM_COL_KEY_HPP
