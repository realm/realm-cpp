#include <cpprealm/internal/bridge/timestamp.hpp>

#include <realm/timestamp.hpp>

namespace realm::internal::bridge {
    timestamp::timestamp(int64_t seconds, int32_t nanoseconds) {
        new (&m_timestamp) Timestamp(seconds, nanoseconds);
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
}
