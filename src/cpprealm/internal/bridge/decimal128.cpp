#include <cpprealm/internal/bridge/decimal128.hpp>
#include <cpprealm/internal/bridge/utils.hpp>
#include <cpprealm/experimental/types.hpp>

#include <realm/decimal128.hpp>

namespace realm::internal::bridge {
#ifdef __i386__
    static_assert(SizeCheck<16, sizeof(::realm::Decimal128)>{});
    static_assert(SizeCheck<4, alignof(::realm::Decimal128)>{});
#elif __x86_64__
    static_assert(SizeCheck<16, sizeof(::realm::Decimal128)>{});
    static_assert(SizeCheck<8, alignof(::realm::Decimal128)>{});
#elif __arm__
    static_assert(SizeCheck<16, sizeof(::realm::Decimal128)>{});
    static_assert(SizeCheck<8, alignof(::realm::Decimal128)>{});
#elif __aarch64__
    static_assert(SizeCheck<16, sizeof(::realm::Decimal128)>{});
    static_assert(SizeCheck<8, alignof(::realm::Decimal128)>{});
#elif _WIN32
    static_assert(SizeCheck<16, sizeof(::realm::Decimal128)>{});
    static_assert(SizeCheck<8, alignof(::realm::Decimal128)>{});
#endif

    decimal128::decimal128() {
        new(&m_decimal) Decimal128();
    }
    decimal128::decimal128(const decimal128& other) {
        new (&m_decimal) Decimal128(*reinterpret_cast<const Decimal128*>(&other.m_decimal));
    }

    decimal128& decimal128::operator=(const decimal128& other) {
        if (this != &other) {
            *reinterpret_cast<Decimal128*>(&m_decimal) = *reinterpret_cast<const Decimal128*>(&other.m_decimal);
        }
        return *this;
    }

    decimal128::decimal128(decimal128&& other) {
        new (&m_decimal) Decimal128(std::move(*reinterpret_cast<Decimal128*>(&other.m_decimal)));
    }

    decimal128& decimal128::operator=(decimal128&& other) {
        if (this != &other) {
            *reinterpret_cast<Decimal128*>(&m_decimal) = std::move(*reinterpret_cast<Decimal128*>(&other.m_decimal));
        }
        return *this;
    }

    decimal128::~decimal128() {
        reinterpret_cast<Decimal128*>(&m_decimal)->~Decimal128();
    }

    decimal128::decimal128(const std::string &v) {
        new(&m_decimal) Decimal128(v);
    }

    decimal128::decimal128(const double &v) {
        new(&m_decimal) Decimal128(v);
    }

    decimal128::decimal128(const ::realm::decimal128 &v) {
        new(&m_decimal) Decimal128(v.to_string());
    }

    decimal128::decimal128(const Decimal128 &v) {
        new(&m_decimal) Decimal128(v);
    }

    std::string decimal128::to_string() const {
        return reinterpret_cast<const Decimal128 *>(&m_decimal)->to_string();
    }

    bool decimal128::is_NaN() const {
        return reinterpret_cast<const Decimal128 *>(&m_decimal)->is_nan();
    }

    decimal128::operator ::realm::decimal128() const {
        return ::realm::decimal128(*this);
    }

    decimal128::operator Decimal128() const {
        return *reinterpret_cast<const Decimal128 *>(&m_decimal);
    }

    decimal128 decimal128::operator+(const decimal128& o) {
        return reinterpret_cast<const Decimal128 *>(&m_decimal)->operator+(o.operator Decimal128());
    }
    decimal128 decimal128::operator*(const decimal128& o) {
        return reinterpret_cast<const Decimal128 *>(&m_decimal)->operator*(o.operator Decimal128());
    }
    decimal128 decimal128::operator/(const decimal128& o) {
        return reinterpret_cast<const Decimal128 *>(&m_decimal)->operator/(o.operator Decimal128());
    }
    decimal128 decimal128::operator-(const decimal128& o) {
        return reinterpret_cast<const Decimal128 *>(&m_decimal)->operator-(o.operator Decimal128());
    }

    decimal128& decimal128::operator+=(const decimal128& o) {
        new(&m_decimal) Decimal128(reinterpret_cast<Decimal128 *>(&m_decimal)->operator+=(o.operator Decimal128()));
        return *this;
    }
    decimal128& decimal128::operator*=(const decimal128& o) {
        new(&m_decimal) Decimal128(reinterpret_cast<Decimal128 *>(&m_decimal)->operator*=(o.operator Decimal128()));
        return *this;
    }
    decimal128& decimal128::operator/=(const decimal128& o) {
        new(&m_decimal) Decimal128(reinterpret_cast<Decimal128 *>(&m_decimal)->operator/=(o.operator Decimal128()));
        return *this;
    }
    decimal128& decimal128::operator-=(const decimal128& o) {
        new(&m_decimal) Decimal128(reinterpret_cast<Decimal128 *>(&m_decimal)->operator-=(o.operator Decimal128()));
        return *this;
    }

#define __cpp_realm_gen_decimal_op(op) \
    bool operator op(const decimal128& a, const decimal128& b) { \
        return *reinterpret_cast<const Decimal128*>(&a.m_decimal) op *reinterpret_cast<const Decimal128*>(&b.m_decimal); \
    } \

    __cpp_realm_gen_decimal_op(==)
    __cpp_realm_gen_decimal_op(!=)
    __cpp_realm_gen_decimal_op(>)
    __cpp_realm_gen_decimal_op(<)
    __cpp_realm_gen_decimal_op(>=)
    __cpp_realm_gen_decimal_op(<=)
}