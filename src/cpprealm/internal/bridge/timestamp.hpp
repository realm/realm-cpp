#ifndef CPP_REALM_BRIDGE_TIMESTAMP_HPP
#define CPP_REALM_BRIDGE_TIMESTAMP_HPP

#include <chrono>
#include <cpprealm/internal/bridge/utils.hpp>

namespace realm {
    class Timestamp;
}

namespace realm::internal::bridge {
    struct timestamp : core_binding<Timestamp> {
        timestamp();
        timestamp(const timestamp& other) ;
        timestamp& operator=(const timestamp& other) ;
        timestamp(timestamp&& other);
        timestamp& operator=(timestamp&& other);
        ~timestamp();
        timestamp(const Timestamp&); //NOLINT(google-explicit-constructor)
        operator Timestamp() const final; //NOLINT(google-explicit-constructor)
        operator std::chrono::time_point<std::chrono::system_clock>() const; //NOLINT(google-explicit-constructor)
        timestamp(int64_t seconds, int32_t nanoseconds);
        timestamp(const std::chrono::time_point<std::chrono::system_clock>& tp); //NOLINT(google-explicit-constructor)
        [[nodiscard]] int64_t get_seconds() const noexcept;
        [[nodiscard]] int32_t get_nanoseconds() const noexcept;
        [[nodiscard]] std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration>
                get_time_point() const {
            int64_t native_nano = get_seconds() * nanoseconds_per_second + get_nanoseconds();
            auto duration = std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::duration<int64_t, std::nano>{native_nano});
            return std::chrono::time_point<std::chrono::system_clock,
                                std::chrono::system_clock::duration>(duration);
        }
    private:
        static constexpr int32_t nanoseconds_per_second = 1000000000;
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        storage::Timestamp m_timestamp[1];
#else
        std::pair<int64_t, int32_t> m_timestamp;
#endif
        friend bool operator ==(const timestamp&, const timestamp&);
        friend bool operator !=(const timestamp&, const timestamp&);
        friend bool operator >(const timestamp&, const timestamp&);
        friend bool operator <(const timestamp&, const timestamp&);
        friend bool operator >=(const timestamp&, const timestamp&);
        friend bool operator <=(const timestamp&, const timestamp&);
    };

    bool operator ==(const timestamp&, const timestamp&);
    bool operator !=(const timestamp&, const timestamp&);
    bool operator >(const timestamp&, const timestamp&);
    bool operator <(const timestamp&, const timestamp&);
    bool operator >=(const timestamp&, const timestamp&);
    bool operator <=(const timestamp&, const timestamp&);
}

#endif //CPP_REALM_BRIDGE_TIMESTAMP_HPP
