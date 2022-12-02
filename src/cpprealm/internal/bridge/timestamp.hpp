#ifndef CPP_REALM_BRIDGE_TIMESTAMP_HPP
#define CPP_REALM_BRIDGE_TIMESTAMP_HPP

#include <chrono>

namespace realm {
    class Timestamp;
}
namespace realm::internal::bridge {
    struct timestamp {
        timestamp();
        timestamp(const Timestamp&);
        operator Timestamp() const;
        timestamp(int64_t seconds, int32_t nanoseconds);
        template <typename C = std::chrono::system_clock, typename D = typename C::duration>
        timestamp(const std::chrono::time_point<C, D>& tp)
        : timestamp(std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch()).count(),
                    std::chrono::duration_cast<std::chrono::nanoseconds>(tp.time_since_epoch()).count())
        {
        }
        int64_t get_seconds() const noexcept;
        int32_t get_nanoseconds() const noexcept;
        template <typename C = std::chrono::system_clock, typename D = typename C::duration>
        std::chrono::time_point<C, D> get_time_point() const {
            int64_t native_nano = get_seconds() * nanoseconds_per_second + get_nanoseconds();
            auto duration = std::chrono::duration_cast<D>(std::chrono::duration<int64_t, std::nano>{native_nano});
            return std::chrono::time_point<C, D>(duration);
        }
    private:
        static constexpr int32_t nanoseconds_per_second = 1000000000;
        unsigned char m_timestamp[32]{};
    };
}

#endif //CPP_REALM_BRIDGE_TIMESTAMP_HPP
