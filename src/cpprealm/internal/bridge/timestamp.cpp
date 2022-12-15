#include <cpprealm/internal/bridge/timestamp.hpp>

#include <realm/timestamp.hpp>

namespace realm::internal::bridge {
    timestamp::timestamp(const realm::Timestamp &v) {
        new (&m_timestamp) Timestamp(v);
    }
    timestamp::timestamp(int64_t seconds, int32_t nanoseconds) {
        new (&m_timestamp) Timestamp(seconds, nanoseconds);
    }

    timestamp::timestamp(const std::chrono::time_point<std::chrono::system_clock> &tp) {
        new (&m_timestamp) Timestamp(tp);
    }
    timestamp::operator std::chrono::time_point<std::chrono::system_clock>() const {
        return reinterpret_cast<const Timestamp*>(m_timestamp)->get_time_point();
    }

    int64_t timestamp::get_seconds() const noexcept {
        return reinterpret_cast<const Timestamp*>(m_timestamp)->get_seconds();
    }

    int32_t timestamp::get_nanoseconds() const noexcept {
        return reinterpret_cast<const Timestamp*>(m_timestamp)->get_nanoseconds();
    }

    timestamp::timestamp() {
        new (&m_timestamp) Timestamp();
    }

    timestamp::operator Timestamp() const {
        return *reinterpret_cast<const Timestamp*>(m_timestamp);
    }
}
