#include <cpprealm/internal/bridge/decimal128.hpp>
#include <cpprealm/internal/bridge/utils.hpp>
#include <cpprealm/experimental/types.hpp>

#include <realm/decimal128.hpp>

namespace realm::internal::bridge {
    decimal128::decimal128() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new(&m_decimal) Decimal128();
#endif
    }
    decimal128::decimal128(const decimal128& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_decimal) Decimal128(*reinterpret_cast<const Decimal128*>(&other.m_decimal));
#else
        m_decimal[0] = other.m_decimal[0];
        m_decimal[1] = other.m_decimal[1];
#endif
    }

    decimal128& decimal128::operator=(const decimal128& other) {
        if (this != &other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
            *reinterpret_cast<Decimal128*>(&m_decimal) = *reinterpret_cast<const Decimal128*>(&other.m_decimal);
#else
            m_decimal[0] = other.m_decimal[0];
            m_decimal[1] = other.m_decimal[1];
#endif
        }
        return *this;
    }

    decimal128::decimal128(decimal128&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_decimal) Decimal128(std::move(*reinterpret_cast<Decimal128*>(&other.m_decimal)));
#else
        m_decimal[0] = std::move(other.m_decimal[0]);
        m_decimal[1] = std::move(other.m_decimal[1]);
#endif
    }

    decimal128& decimal128::operator=(decimal128&& other) {
        if (this != &other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
            *reinterpret_cast<Decimal128*>(&m_decimal) = std::move(*reinterpret_cast<Decimal128*>(&other.m_decimal));
#else
            m_decimal[0] = std::move(other.m_decimal[0]);
            m_decimal[1] = std::move(other.m_decimal[1]);
#endif
        }
        return *this;
    }

    decimal128::~decimal128() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<Decimal128*>(&m_decimal)->~Decimal128();
#endif
    }

    decimal128::decimal128(const std::string &v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new(&m_decimal) Decimal128(v);
#else
        auto tmp = Decimal128(v).raw()->w;
        m_decimal[0] = tmp[0];
        m_decimal[1] = tmp[1];
#endif
    }

    decimal128::decimal128(const double &v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new(&m_decimal) Decimal128(v);
#else
        auto tmp = Decimal128(v).raw()->w;
        m_decimal[0] = tmp[0];
        m_decimal[1] = tmp[1];
#endif
    }

    decimal128::decimal128(const ::realm::decimal128 &v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new(&m_decimal) Decimal128(v.to_string());
#else
        auto tmp = Decimal128(v.to_string()).raw()->w;
        m_decimal[0] = tmp[0];
        m_decimal[1] = tmp[1];
#endif
    }

    decimal128::decimal128(const Decimal128 &v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new(&m_decimal) Decimal128(v);
#else
        auto tmp = Decimal128(v).raw()->w;
        m_decimal[0] = tmp[0];
        m_decimal[1] = tmp[1];
#endif
    }

    std::string decimal128::to_string() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const Decimal128 *>(&m_decimal)->to_string();
#else
        Decimal128::Bid128 bid128;
        bid128.w[0] = m_decimal[0];
        bid128.w[1] = m_decimal[1];
        return Decimal128(std::move(bid128)).to_string();
#endif
    }

    bool decimal128::is_NaN() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const Decimal128 *>(&m_decimal)->is_nan();
#else
        Decimal128::Bid128 b128;
        b128.w[0] = m_decimal[0];
        b128.w[1] = m_decimal[1];
        return Decimal128(std::move(b128)).is_nan();
#endif
    }

    decimal128::operator ::realm::decimal128() const {
        return ::realm::decimal128(*this);
    }

    decimal128::operator Decimal128() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return *reinterpret_cast<const Decimal128 *>(&m_decimal);
#else
        Decimal128::Bid128 bid128;
        bid128.w[0] = m_decimal[0];
        bid128.w[1] = m_decimal[1];
        return Decimal128(std::move(bid128));
#endif
    }

    decimal128 decimal128::operator+(const decimal128& o) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const Decimal128 *>(&m_decimal)->operator+(o.operator Decimal128());
#else
        Decimal128::Bid128 bid128;
        bid128.w[0] = m_decimal[0];
        bid128.w[1] = m_decimal[1];
        auto tmp = Decimal128(std::move(bid128));
        return tmp.operator+(o.operator Decimal128());
#endif
    }
    decimal128 decimal128::operator*(const decimal128& o) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const Decimal128 *>(&m_decimal)->operator*(o.operator Decimal128());
#else
        Decimal128::Bid128 bid128;
        bid128.w[0] = m_decimal[0];
        bid128.w[1] = m_decimal[1];
        auto tmp = Decimal128(std::move(bid128));
        return tmp.operator*(o.operator Decimal128());
#endif
    }
    decimal128 decimal128::operator/(const decimal128& o) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const Decimal128 *>(&m_decimal)->operator/(o.operator Decimal128());
#else
        Decimal128::Bid128 bid128;
        bid128.w[0] = m_decimal[0];
        bid128.w[1] = m_decimal[1];
        auto tmp = Decimal128(std::move(bid128));
        return tmp.operator/(o.operator Decimal128());
#endif
    }
    decimal128 decimal128::operator-(const decimal128& o) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const Decimal128 *>(&m_decimal)->operator-(o.operator Decimal128());
#else
        Decimal128::Bid128 bid128;
        bid128.w[0] = m_decimal[0];
        bid128.w[1] = m_decimal[1];
        auto tmp = Decimal128(std::move(bid128));
        return tmp.operator-(o.operator Decimal128());
#endif
    }

    decimal128& decimal128::operator+=(const decimal128& o) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new(&m_decimal) Decimal128(reinterpret_cast<Decimal128 *>(&m_decimal)->operator+=(o.operator Decimal128()));
#else
        Decimal128::Bid128 bid128;
        bid128.w[0] = m_decimal[0];
        bid128.w[1] = m_decimal[1];
        auto tmp = Decimal128(std::move(bid128));
        auto res = tmp.operator+=(o.operator Decimal128()).raw();
        m_decimal[0] = res->w[0];
        m_decimal[1] = res->w[1];
#endif
        return *this;
    }
    decimal128& decimal128::operator*=(const decimal128& o) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new(&m_decimal) Decimal128(reinterpret_cast<Decimal128 *>(&m_decimal)->operator*=(o.operator Decimal128()));
#else
        Decimal128::Bid128 bid128;
        bid128.w[0] = m_decimal[0];
        bid128.w[1] = m_decimal[1];
        auto tmp = Decimal128(std::move(bid128));
        auto res = tmp.operator*=(o.operator Decimal128()).raw();
        m_decimal[0] = res->w[0];
        m_decimal[1] = res->w[1];
#endif
        return *this;
    }
    decimal128& decimal128::operator/=(const decimal128& o) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new(&m_decimal) Decimal128(reinterpret_cast<Decimal128 *>(&m_decimal)->operator/=(o.operator Decimal128()));
#else
        Decimal128::Bid128 bid128;
        bid128.w[0] = m_decimal[0];
        bid128.w[1] = m_decimal[1];
        auto tmp = Decimal128(std::move(bid128));
        auto res = tmp.operator/=(o.operator Decimal128()).raw();
        m_decimal[0] = res->w[0];
        m_decimal[1] = res->w[1];
#endif
        return *this;
    }
    decimal128& decimal128::operator-=(const decimal128& o) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new(&m_decimal) Decimal128(reinterpret_cast<Decimal128 *>(&m_decimal)->operator-=(o.operator Decimal128()));
#else
        Decimal128::Bid128 bid128;
        bid128.w[0] = m_decimal[0];
        bid128.w[1] = m_decimal[1];
        auto tmp = Decimal128(std::move(bid128));
        auto res = tmp.operator-=(o.operator Decimal128()).raw();
        m_decimal[0] = res->w[0];
        m_decimal[1] = res->w[1];
#endif
        return *this;
    }

    bool operator ==(const decimal128& a, const decimal128& b) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return *reinterpret_cast<const Decimal128*>(&a.m_decimal) == *reinterpret_cast<const Decimal128*>(&b.m_decimal);
#else
        return a.m_decimal[0] == b.m_decimal[0] && a.m_decimal[1] == b.m_decimal[1];
#endif
    }

    bool operator !=(const decimal128& a, const decimal128& b) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return *reinterpret_cast<const Decimal128*>(&a.m_decimal) != *reinterpret_cast<const Decimal128*>(&b.m_decimal);
#else
        return a.m_decimal[0] != b.m_decimal[0] || a.m_decimal[1] != b.m_decimal[1];
#endif
    }

    bool operator >(const decimal128& a, const decimal128& b) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return *reinterpret_cast<const Decimal128*>(&a.m_decimal) > *reinterpret_cast<const Decimal128*>(&b.m_decimal);
#else
        Decimal128 lhs, rhs;
        lhs.raw()->w[0] = a.m_decimal[0];
        lhs.raw()->w[1] = a.m_decimal[1];
        rhs.raw()->w[0] = b.m_decimal[0];
        rhs.raw()->w[1] = b.m_decimal[1];
        return lhs > rhs;
#endif
    }

    bool operator <(const decimal128& a, const decimal128& b) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return *reinterpret_cast<const Decimal128*>(&a.m_decimal) < *reinterpret_cast<const Decimal128*>(&b.m_decimal);
#else
        Decimal128 lhs, rhs;
        lhs.raw()->w[0] = a.m_decimal[0];
        lhs.raw()->w[1] = a.m_decimal[1];
        rhs.raw()->w[0] = b.m_decimal[0];
        rhs.raw()->w[1] = b.m_decimal[1];
        return lhs < rhs;
#endif
    }

    bool operator >=(const decimal128& a, const decimal128& b) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return *reinterpret_cast<const Decimal128*>(&a.m_decimal) >= *reinterpret_cast<const Decimal128*>(&b.m_decimal);
#else
        Decimal128 lhs, rhs;
        lhs.raw()->w[0] = a.m_decimal[0];
        lhs.raw()->w[1] = a.m_decimal[1];
        rhs.raw()->w[0] = b.m_decimal[0];
        rhs.raw()->w[1] = b.m_decimal[1];
        return lhs >= rhs;
#endif
    }

    bool operator <=(const decimal128& a, const decimal128& b) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return *reinterpret_cast<const Decimal128*>(&a.m_decimal) <= *reinterpret_cast<const Decimal128*>(&b.m_decimal);
#else
        Decimal128 lhs, rhs;
        lhs.raw()->w[0] = a.m_decimal[0];
        lhs.raw()->w[1] = a.m_decimal[1];
        rhs.raw()->w[0] = b.m_decimal[0];
        rhs.raw()->w[1] = b.m_decimal[1];
        return lhs <= rhs;
#endif
    }
}