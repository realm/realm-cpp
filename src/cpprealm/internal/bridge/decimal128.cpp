#include "cpprealm/types.hpp"
#include <cpprealm/internal/bridge/decimal128.hpp>

#include <realm/decimal128.hpp>

namespace realm::internal::bridge {

    inline void copy_values(uint64_t (&dest)[2], const uint64_t (&src)[2]) {
        dest[0] = src[0];
        dest[1] = src[1];
    }

    decimal128::decimal128(const decimal128& other) {
        copy_values(m_decimal, other.m_decimal);
    }

    decimal128& decimal128::operator=(const decimal128& other) {
        copy_values(m_decimal, other.m_decimal);
        return *this;
    }

    decimal128::decimal128(decimal128&& other) {
        copy_values(m_decimal, std::move(other.m_decimal));
    }

    decimal128& decimal128::operator=(decimal128&& other) {
        copy_values(m_decimal, std::move(other.m_decimal));
        return *this;
    }

    decimal128::decimal128(const std::string &v) {
        copy_values(m_decimal, Decimal128(v).raw()->w);
    }

    decimal128::decimal128(const double &v) {
        copy_values(m_decimal, Decimal128(v).raw()->w);
    }

    decimal128::decimal128(const ::realm::decimal128 &v) {
        copy_values(m_decimal, Decimal128(v.to_string()).raw()->w);
    }

    decimal128::decimal128(const Decimal128 &v) {
        copy_values(m_decimal, Decimal128(v).raw()->w);
    }

    std::string decimal128::to_string() const {
        Decimal128::Bid128 bid128;
        copy_values(bid128.w, m_decimal);
        return Decimal128(std::move(bid128)).to_string();
    }

    bool decimal128::is_NaN() const {
        Decimal128::Bid128 bid128;
        copy_values(bid128.w, m_decimal);
        return Decimal128(std::move(bid128)).is_nan();
    }

    decimal128::operator ::realm::decimal128() const {
        return ::realm::decimal128(*this);
    }

    decimal128::operator Decimal128() const {
        Decimal128::Bid128 bid128;
        copy_values(bid128.w, m_decimal);
        return Decimal128(std::move(bid128));
    }

    decimal128 decimal128::operator+(const decimal128& o) {
        Decimal128::Bid128 bid128;
        copy_values(bid128.w, m_decimal);
        auto tmp = Decimal128(std::move(bid128));
        return tmp.operator+(o.operator Decimal128());
    }
    decimal128 decimal128::operator*(const decimal128& o) {
        Decimal128::Bid128 bid128;
        copy_values(bid128.w, m_decimal);
        auto tmp = Decimal128(std::move(bid128));
        return tmp.operator*(o.operator Decimal128());
    }
    decimal128 decimal128::operator/(const decimal128& o) {
        Decimal128::Bid128 bid128;
        copy_values(bid128.w, m_decimal);
        auto tmp = Decimal128(std::move(bid128));
        return tmp.operator/(o.operator Decimal128());
    }
    decimal128 decimal128::operator-(const decimal128& o) {
        Decimal128::Bid128 bid128;
        copy_values(bid128.w, m_decimal);
        auto tmp = Decimal128(std::move(bid128));
        return tmp.operator-(o.operator Decimal128());
    }

    decimal128& decimal128::operator+=(const decimal128& o) {
        Decimal128::Bid128 bid128;
        copy_values(bid128.w, m_decimal);
        auto tmp = Decimal128(std::move(bid128));
        auto res = tmp.operator+=(o.operator Decimal128()).raw();
        m_decimal[0] = res->w[0];
        m_decimal[1] = res->w[1];
        copy_values(m_decimal, res->w);
        return *this;
    }
    decimal128& decimal128::operator*=(const decimal128& o) {
        Decimal128::Bid128 bid128;
        copy_values(bid128.w, m_decimal);
        auto tmp = Decimal128(std::move(bid128));
        auto res = tmp.operator*=(o.operator Decimal128()).raw();
        copy_values(m_decimal, res->w);
        return *this;
    }
    decimal128& decimal128::operator/=(const decimal128& o) {
        Decimal128::Bid128 bid128;
        copy_values(bid128.w, m_decimal);
        auto tmp = Decimal128(std::move(bid128));
        auto res = tmp.operator/=(o.operator Decimal128()).raw();
        copy_values(m_decimal, res->w);
        return *this;
    }
    decimal128& decimal128::operator-=(const decimal128& o) {
        Decimal128::Bid128 bid128;
        copy_values(bid128.w, m_decimal);
        auto tmp = Decimal128(std::move(bid128));
        auto res = tmp.operator-=(o.operator Decimal128()).raw();
        copy_values(m_decimal, res->w);
        return *this;
    }

    bool operator ==(const decimal128& a, const decimal128& b) {
        return a.m_decimal[0] == b.m_decimal[0] && a.m_decimal[1] == b.m_decimal[1];
    }

    bool operator !=(const decimal128& a, const decimal128& b) {
        return a.m_decimal[0] != b.m_decimal[0] || a.m_decimal[1] != b.m_decimal[1];
    }

    bool operator >(const decimal128& a, const decimal128& b) {
        Decimal128 lhs, rhs;
        copy_values(lhs.raw()->w, a.m_decimal);
        copy_values(rhs.raw()->w, b.m_decimal);
        return lhs > rhs;
    }

    bool operator <(const decimal128& a, const decimal128& b) {
        Decimal128 lhs, rhs;
        copy_values(lhs.raw()->w, a.m_decimal);
        copy_values(rhs.raw()->w, b.m_decimal);
        return lhs < rhs;
    }

    bool operator >=(const decimal128& a, const decimal128& b) {
        Decimal128 lhs, rhs;
        copy_values(lhs.raw()->w, a.m_decimal);
        copy_values(rhs.raw()->w, b.m_decimal);
        return lhs >= rhs;
    }

    bool operator <=(const decimal128& a, const decimal128& b) {
        Decimal128 lhs, rhs;
        copy_values(lhs.raw()->w, a.m_decimal);
        copy_values(rhs.raw()->w, b.m_decimal);
        return lhs <= rhs;
    }
}