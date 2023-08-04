#include <cpprealm/experimental/managed_decimal.hpp>
#include <cpprealm/persisted.hpp>
#include <cpprealm/internal/bridge/decimal128.hpp>

namespace realm::experimental {
    __cpprealm_build_experimental_query(==, equal, realm::decimal128);
    __cpprealm_build_experimental_query(!=, not_equal, realm::decimal128);
    __cpprealm_build_experimental_query(>, greater, realm::decimal128);
    __cpprealm_build_experimental_query(>=, greater_equal, realm::decimal128);
    __cpprealm_build_experimental_query(<, less, realm::decimal128);
    __cpprealm_build_experimental_query(<=, less_equal, realm::decimal128);
    __cpprealm_build_optional_experimental_query(==, equal, realm::decimal128);
    __cpprealm_build_optional_experimental_query(!=, not_equal, realm::decimal128);

#define managed_decimal_arithmetic(op) \
    realm::decimal128 managed<realm::decimal128>::operator op(const decimal128& o) { \
        auto old_val = m_obj->template get<internal::bridge::decimal128>(m_key); \
        auto new_val = old_val.operator op(o.m_decimal); \
        m_obj->template set<internal::bridge::decimal128>(this->m_key, new_val); \
        return new_val.operator ::realm::decimal128(); \
    } \

    managed_decimal_arithmetic(+)
    managed_decimal_arithmetic(-)
    managed_decimal_arithmetic(*)
    managed_decimal_arithmetic(/)

#define managed_decimal_arithmetic_assignment(op, arithmetic_op) \
    managed<realm::decimal128>& managed<realm::decimal128>::operator op(const decimal128& o) { \
        auto old_val = m_obj->template get<internal::bridge::decimal128>(m_key); \
        auto new_val = old_val.operator arithmetic_op(o.m_decimal); \
        m_obj->template set<internal::bridge::decimal128>(this->m_key, new_val); \
        return *this; \
    } \

    managed_decimal_arithmetic_assignment(+=, +)
    managed_decimal_arithmetic_assignment(-=, -)
    managed_decimal_arithmetic_assignment(*=, *)
    managed_decimal_arithmetic_assignment(/=, /)

    // MARK: Optional

#define managed_optional_decimal_arithmetic(op) \
    realm::decimal128 managed<std::optional<realm::decimal128>>::operator op(const decimal128& o) { \
        if (auto old_val = m_obj->template get_optional<internal::bridge::decimal128>(m_key)) { \
            auto new_val = old_val->operator op(o.m_decimal); \
            m_obj->template set<internal::bridge::decimal128>(this->m_key, new_val); \
            return new_val.operator ::realm::decimal128(); \
        } \
        throw std::runtime_error("Optional decimal128 property has null value"); \
    } \

    managed_optional_decimal_arithmetic(+)
    managed_optional_decimal_arithmetic(-)
    managed_optional_decimal_arithmetic(*)
    managed_optional_decimal_arithmetic(/)

#define managed_optional_decimal_arithmetic_assignment(op, arithmetic_op) \
    managed<std::optional<realm::decimal128>>&  managed<std::optional<realm::decimal128>>::operator op(const decimal128& o) { \
        if (auto old_val = m_obj->template get_optional<internal::bridge::decimal128>(m_key)) { \
        auto new_val = old_val->operator arithmetic_op(o.m_decimal); \
        m_obj->template set<internal::bridge::decimal128>(this->m_key, new_val); \
        return *this; \
        } \
        throw std::runtime_error("Optional decimal128 property has null value"); \
    } \

    managed_optional_decimal_arithmetic_assignment(+=, +)
    managed_optional_decimal_arithmetic_assignment(-=, -)
    managed_optional_decimal_arithmetic_assignment(*=, *)
    managed_optional_decimal_arithmetic_assignment(/=, /)
}
