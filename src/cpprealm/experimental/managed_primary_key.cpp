#include <cpprealm/experimental/managed_primary_key.hpp>
#include <cpprealm/persisted.hpp>

namespace realm::experimental {
    template<>
    rbool managed<primary_key<int64_t>>::operator==(const int64_t& rhs) const noexcept {
        if (this->should_detect_usage_for_queries) {
            auto query = internal::bridge::query(this->query->get_table());
            query.equal(this->m_key, rhs);
            return query;
        }
        return value() == rhs;
    }

    template<>
    rbool managed<primary_key<int64_t>>::operator!=(const int64_t& rhs) const noexcept {
        if (this->should_detect_usage_for_queries) {
            auto query = internal::bridge::query(this->query->get_table());
            query.not_equal(this->m_key, rhs);
            return query;
        }
        return value() != rhs;
    }
    template<>
    rbool managed<primary_key<int64_t>>::operator>(const int64_t& rhs) const noexcept {
        if (this->should_detect_usage_for_queries) {
            auto query = internal::bridge::query(this->query->get_table());
            query.greater(this->m_key, rhs);
            return query;
        }
        return value() > rhs;
    }
    template<>
    rbool managed<primary_key<int64_t>>::operator>=(const int64_t& rhs) const noexcept {
        if (this->should_detect_usage_for_queries) {
            auto query = internal::bridge::query(this->query->get_table());
            query.greater_equal(this->m_key, rhs);
            return query;
        }
        return value() >= rhs;
    }
    template<>
    rbool managed<primary_key<int64_t>>::operator<(const int64_t& rhs) const noexcept {
        if (this->should_detect_usage_for_queries) {
            auto query = internal::bridge::query(this->query->get_table());
            query.less(this->m_key, rhs);
            return query;
        }
        return value() < rhs;
    }
    template<>
    rbool managed<primary_key<int64_t>>::operator<=(const int64_t& rhs) const noexcept {
        if (this->should_detect_usage_for_queries) {
            auto query = internal::bridge::query(this->query->get_table());
            query.less_equal(this->m_key, rhs);
            return query;
        }
        return value() <= rhs;
    }
}
