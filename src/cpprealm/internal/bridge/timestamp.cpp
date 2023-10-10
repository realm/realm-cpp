#include <cpprealm/internal/bridge/timestamp.hpp>

#include <realm/timestamp.hpp>

namespace realm::internal::bridge {
    timestamp::timestamp(const realm::Timestamp &v) {
        m_seconds = v.get_seconds();
        m_nanoseconds = v.get_nanoseconds();
    }

    timestamp::timestamp(int64_t seconds, int32_t nanoseconds) {
        m_seconds = seconds;
        m_nanoseconds = nanoseconds;
    }

    timestamp::timestamp(const std::chrono::time_point<std::chrono::system_clock> &tp) {
        auto ts = Timestamp(tp);
        m_seconds = ts.get_seconds();
        m_nanoseconds = ts.get_nanoseconds();
    }
    timestamp::operator std::chrono::time_point<std::chrono::system_clock>() const {
        return Timestamp(m_seconds, m_nanoseconds).get_time_point();
    }

    int64_t timestamp::get_seconds() const noexcept {
        return m_seconds;
    }

    int32_t timestamp::get_nanoseconds() const noexcept {
        return m_nanoseconds;
    }

    timestamp::operator Timestamp() const {
        return Timestamp(m_seconds, m_nanoseconds);
    }

#define __cpp_realm_gen_timestamp_op(op) \
    bool operator op(const timestamp& a, const timestamp& b) { \
        return a.operator Timestamp() op b.operator Timestamp(); \
    } \

    __cpp_realm_gen_timestamp_op(==)
    __cpp_realm_gen_timestamp_op(!=)
    __cpp_realm_gen_timestamp_op(>)
    __cpp_realm_gen_timestamp_op(<)
    __cpp_realm_gen_timestamp_op(>=)
    __cpp_realm_gen_timestamp_op(<=)
}
