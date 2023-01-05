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
#ifdef __i386__
        std::aligned_storage<16, 4>::type m_timestamp[1];
#elif __x86_64__
        std::aligned_storage<16, 8>::type m_timestamp[1];
#elif __arm__
        std::aligned_storage<16, 8>::type m_timestamp[1];
#elif __aarch64__
        std::aligned_storage<16, 8>::type m_timestamp[1];
#else
        std::aligned_storage<12, 4>::type m_timestamp[1];
#endif
    };
}

#endif //CPP_REALM_BRIDGE_TIMESTAMP_HPP
