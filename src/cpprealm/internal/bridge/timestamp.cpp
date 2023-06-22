#include <cpprealm/internal/bridge/timestamp.hpp>

#include <realm/timestamp.hpp>

namespace realm::internal::bridge {
#ifdef __i386__
    static_assert(SizeCheck<16, sizeof(Timestamp)>{});
    static_assert(SizeCheck<4, alignof(Timestamp)>{});
#elif __x86_64__
    static_assert(SizeCheck<16, sizeof(Timestamp)>{});
    static_assert(SizeCheck<8, alignof(Timestamp)>{});
#elif __arm__
    static_assert(SizeCheck<16, sizeof(Timestamp)>{});
    static_assert(SizeCheck<8, alignof(Timestamp)>{});
#elif __aarch64__
    static_assert(SizeCheck<16, sizeof(Timestamp)>{});
    static_assert(SizeCheck<8, alignof(Timestamp)>{});
#elif _WIN32
    static_assert(SizeCheck<16, sizeof(Timestamp)>{});
    static_assert(SizeCheck<8, alignof(Timestamp)>{});
#endif

    timestamp::timestamp() {
        new (&m_timestamp) Timestamp();
    }

    timestamp::timestamp(const timestamp& other) {
        new (&m_timestamp) Timestamp(*reinterpret_cast<const Timestamp*>(&other.m_timestamp));
    }

    timestamp& timestamp::operator=(const timestamp& other) {
        if (this != &other) {
            *reinterpret_cast<Timestamp*>(&m_timestamp) = *reinterpret_cast<const Timestamp*>(&other.m_timestamp);
        }
        return *this;
    }

    timestamp::timestamp(timestamp&& other) {
        new (&m_timestamp) Timestamp(std::move(*reinterpret_cast<Timestamp*>(&other.m_timestamp)));
    }

    timestamp& timestamp::operator=(timestamp&& other) {
        if (this != &other) {
            *reinterpret_cast<Timestamp*>(&m_timestamp) = std::move(*reinterpret_cast<Timestamp*>(&other.m_timestamp));
        }
        return *this;
    }

    timestamp::~timestamp() {
        reinterpret_cast<Timestamp*>(&m_timestamp)->~Timestamp();
    }

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
        return reinterpret_cast<const Timestamp*>(&m_timestamp)->get_time_point();
    }

    int64_t timestamp::get_seconds() const noexcept {
        return reinterpret_cast<const Timestamp*>(&m_timestamp)->get_seconds();
    }

    int32_t timestamp::get_nanoseconds() const noexcept {
        return reinterpret_cast<const Timestamp*>(&m_timestamp)->get_nanoseconds();
    }

    timestamp::operator Timestamp() const {
        return *reinterpret_cast<const Timestamp*>(&m_timestamp);
    }


#define __cpp_realm_gen_timestamp_op(op) \
    bool operator op(const timestamp& a, const timestamp& b) { \
        return *reinterpret_cast<const Timestamp*>(a.m_timestamp) op *reinterpret_cast<const Timestamp*>(b.m_timestamp); \
    } \

    __cpp_realm_gen_timestamp_op(==)
    __cpp_realm_gen_timestamp_op(!=)
    __cpp_realm_gen_timestamp_op(>)
    __cpp_realm_gen_timestamp_op(<)
    __cpp_realm_gen_timestamp_op(>=)
    __cpp_realm_gen_timestamp_op(<=)
}
