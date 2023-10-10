#ifndef CPP_REALM_BRIDGE_DECIMAL128_HPP
#define CPP_REALM_BRIDGE_DECIMAL128_HPP

#include <cpprealm/internal/bridge/utils.hpp>

namespace realm {
    struct decimal128;
    class Decimal128;
}

namespace realm::internal::bridge {
    struct decimal128 : core_binding<Decimal128> {
        decimal128() = default;
        decimal128(const decimal128& other) ;
        decimal128& operator=(const decimal128& other) ;
        decimal128(decimal128&& other);
        decimal128& operator=(decimal128&& other);
        ~decimal128() = default;
        decimal128(const Decimal128&); //NOLINT(google-explicit-constructor);
        explicit decimal128(const std::string&);
        decimal128(const double&);
        decimal128(const struct ::realm::decimal128&); //NOLINT(google-explicit-constructor);
        operator Decimal128() const final; //NOLINT(google-explicit-constructor);
        operator ::realm::decimal128() const; //NOLINT(google-explicit-constructor);
        std::string to_string() const;
        bool is_NaN() const;

        decimal128 operator+(const decimal128& o);
        decimal128 operator*(const decimal128& o);
        decimal128 operator/(const decimal128& o);
        decimal128 operator-(const decimal128& o);

        decimal128& operator+=(const decimal128& o);
        decimal128& operator*=(const decimal128& o);
        decimal128& operator/=(const decimal128& o);
        decimal128& operator-=(const decimal128& o);
    private:
        uint64_t m_decimal[2];

        friend bool operator ==(const decimal128&, const decimal128&);
        friend bool operator !=(const decimal128&, const decimal128&);
        friend bool operator >(const decimal128&, const decimal128&);
        friend bool operator <(const decimal128&, const decimal128&);
        friend bool operator >=(const decimal128&, const decimal128&);
        friend bool operator <=(const decimal128&, const decimal128&);
    };

    bool operator ==(const decimal128&, const decimal128&);
    bool operator !=(const decimal128&, const decimal128&);
    bool operator >(const decimal128&, const decimal128&);
    bool operator <(const decimal128&, const decimal128&);
    bool operator >=(const decimal128&, const decimal128&);
    bool operator <=(const decimal128&, const decimal128&);
}

#endif //CPP_REALM_BRIDGE_DECIMAL128_HPP
