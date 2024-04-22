////////////////////////////////////////////////////////////////////////////
//
// Copyright 2024 Realm Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
////////////////////////////////////////////////////////////////////////////

#ifndef CPPREALM_BRIDGE_TIMESTAMP_HPP
#define CPPREALM_BRIDGE_TIMESTAMP_HPP

#include <chrono>
#include <cpprealm/internal/bridge/utils.hpp>

namespace realm {
    class Timestamp;
}

namespace realm::internal::bridge {
    struct timestamp {
        timestamp() = default;
        timestamp(const timestamp& other) = default;
        timestamp& operator=(const timestamp& other) = default;
        timestamp(timestamp&& other) = default;
        timestamp& operator=(timestamp&& other) = default;
        ~timestamp() = default;
        timestamp(const Timestamp&); //NOLINT(google-explicit-constructor)
        operator Timestamp() const; //NOLINT(google-explicit-constructor)
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
        int64_t m_seconds = 0;
        int32_t m_nanoseconds = 0;
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

#endif //CPPREALM_BRIDGE_TIMESTAMP_HPP
