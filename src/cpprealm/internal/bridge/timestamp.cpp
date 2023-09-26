#include <cpprealm/internal/bridge/timestamp.hpp>

#include <realm/timestamp.hpp>

namespace realm::internal::bridge {

    timestamp::timestamp() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_timestamp) Timestamp();
#else
        m_timestamp = {0, 0};
#endif
    }

    timestamp::timestamp(const timestamp& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_timestamp) Timestamp(*reinterpret_cast<const Timestamp*>(&other.m_timestamp));
#else
        m_timestamp = other.m_timestamp;
#endif

    }

    timestamp& timestamp::operator=(const timestamp& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<Timestamp*>(&m_timestamp) = *reinterpret_cast<const Timestamp*>(&other.m_timestamp);
        }
#else
        m_timestamp = other.m_timestamp;
#endif
        return *this;
    }

    timestamp::timestamp(timestamp&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_timestamp) Timestamp(std::move(*reinterpret_cast<Timestamp*>(&other.m_timestamp)));
#else
        m_timestamp = std::move(other.m_timestamp);
#endif
    }

    timestamp& timestamp::operator=(timestamp&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<Timestamp*>(&m_timestamp) = std::move(*reinterpret_cast<Timestamp*>(&other.m_timestamp));
        }
#else
        m_timestamp = std::move(other.m_timestamp);
#endif
        return *this;
    }

    timestamp::~timestamp() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<Timestamp*>(&m_timestamp)->~Timestamp();
#endif
    }

    timestamp::timestamp(const realm::Timestamp &v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_timestamp) Timestamp(v);
#else
        m_timestamp = {v.get_seconds(), v.get_nanoseconds()};
#endif

    }
    timestamp::timestamp(int64_t seconds, int32_t nanoseconds) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_timestamp) Timestamp(seconds, nanoseconds);
#else
        m_timestamp = {seconds, nanoseconds};
#endif
    }

    timestamp::timestamp(const std::chrono::time_point<std::chrono::system_clock> &tp) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_timestamp) Timestamp(tp);
#else
        auto ts = Timestamp(tp);
        m_timestamp = {ts.get_seconds(), ts.get_nanoseconds()};
#endif
    }
    timestamp::operator std::chrono::time_point<std::chrono::system_clock>() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const Timestamp*>(&m_timestamp)->get_time_point();
#else
        return Timestamp(m_timestamp.first, m_timestamp.second).get_time_point();
#endif
    }

    int64_t timestamp::get_seconds() const noexcept {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const Timestamp*>(&m_timestamp)->get_seconds();
#else
        return m_timestamp.first;
#endif
    }

    int32_t timestamp::get_nanoseconds() const noexcept {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const Timestamp*>(&m_timestamp)->get_nanoseconds();
#else
        return m_timestamp.second;
#endif
    }

    timestamp::operator Timestamp() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return *reinterpret_cast<const Timestamp*>(&m_timestamp);
#else
        return Timestamp(m_timestamp.first, m_timestamp.second);
#endif
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
