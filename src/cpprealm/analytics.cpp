////////////////////////////////////////////////////////////////////////////
//
// Copyright 2022 Realm Inc.
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

#include "db.hpp"
#include <external/json/json.hpp>

#if __APPLE__ || __MACH__
#include <sys/socket.h>
#include <sys/sysctl.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <CommonCrypto/CommonDigest.h>
#elif __linux__
#include <ifaddrs.h>
#include <iomanip>
#include <fcntl.h>
#include <netpacket/packet.h>
#include <sys/utsname.h>
#include <unistd.h>
#endif

#include <cpprealm/app.hpp>
#include <cpprealm/internal/generic_network_transport.hpp>

#include <realm/util/base64.hpp>

#include <iostream>

namespace realm {
#if __APPLE__ || __MACH__
    // Wrapper for sysctl() that handles the memory management stuff
    static auto do_sysctl(int *mib, u_int mib_size, size_t *buffer_size)
    {
        std::unique_ptr<void, decltype(&free)> buffer(nullptr, &free);

        int ret = sysctl(mib, mib_size, nullptr, buffer_size, nullptr, 0);
        if (ret != 0) {
            return buffer;
        }

        buffer.reset(malloc(*buffer_size));
        if (!buffer) {
            return buffer;
        }

        ret = sysctl(mib, mib_size, buffer.get(), buffer_size, nullptr, 0);
        if (ret != 0) {
            buffer.reset();
        }

        return buffer;
    }

    // Hash the data in the given buffer and convert it to a hex-format string
    static std::string hash_data(const void *bytes, size_t length)
    {
        unsigned char buffer[CC_SHA256_DIGEST_LENGTH];
        CC_SHA256(bytes, static_cast<CC_LONG>(length), buffer);

        char formatted[CC_SHA256_DIGEST_LENGTH * 2 + 1];
        for (int i = 0; i < CC_SHA256_DIGEST_LENGTH; ++i) {
            snprintf(formatted + i * 2, sizeof(formatted) - i * 2, "%02x", buffer[i]);
        }

        return formatted;
    }

    std::string get_mac_address()
    {
        int en0 = static_cast<int>(if_nametoindex("en0"));
        if (!en0) {
            return nil;
        }

        std::array<int, 6> mib = {{CTL_NET, PF_ROUTE, 0, AF_LINK, NET_RT_IFLIST, en0}};
        size_t buffer_size;
        auto buffer = do_sysctl(&mib[0], mib.size(), &buffer_size);
        if (!buffer) {
            return nil;
        }

        // sockaddr_dl struct is immediately after the if_msghdr struct in the buffer
        auto sockaddr = reinterpret_cast<sockaddr_dl *>(static_cast<if_msghdr *>(buffer.get()) + 1);
        auto mac = reinterpret_cast<const unsigned char *>(sockaddr->sdl_data + sockaddr->sdl_nlen);

        return hash_data(mac, 6);
    }

    std::string get_host_os_verion()
    {
        std::array<int, 2> mib = {{CTL_KERN, KERN_OSRELEASE}};
        size_t buffer_size;
        auto buffer = do_sysctl(&mib[0], mib.size(), &buffer_size);
        if (!buffer) {
            return "unknown";
        }

        return std::string(static_cast<const char*>(buffer.release()), buffer_size - 1);
    }

    bool debugger_attached()
    {
        int name[] = {
                CTL_KERN,
                KERN_PROC,
                KERN_PROC_PID,
                getpid()
        };

        struct kinfo_proc info;
        size_t info_size = sizeof(info);
        if (sysctl(name, sizeof(name)/sizeof(name[0]), &info, &info_size, NULL, 0) == -1) {
            std::cout << util::format("sysctl() failed: %1", strerror(errno)) << std::endl;
            return false;
        }

        return (info.kp_proc.p_flag & P_TRACED) != 0;
    }
#elif __linux__
    std::string print_hex(unsigned char* bs, unsigned int len)
    {
        std::stringstream ss;
        for (size_t i = 0; i < len; i++) {
            ss << std::hex << static_cast<int>(bs[i]);
        }

        return ss.str();
    }

    std::string get_mac_address()
    {
        struct ifaddrs *ifaddr=NULL;
        struct ifaddrs *ifa = NULL;
        std::string concatenated_mac_addresses = "";

        if (getifaddrs(&ifaddr) == -1)
        {
            return "unknown";
        }
        else {
            // concatenate all MAC addresses to try guarantee a more unique hash.
            // For example the first available MAC maybe all zero values and we want to
            // avoid that scenario.
            for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
                if ((ifa->ifa_addr) && (ifa->ifa_addr->sa_family == AF_PACKET)) {
                    struct sockaddr_ll *s = (struct sockaddr_ll*)ifa->ifa_addr;
                    concatenated_mac_addresses += print_hex(s->sll_addr, s->sll_halen);
                }
            }
            freeifaddrs(ifaddr);
        }

        return std::to_string(std::hash<std::string>{}(concatenated_mac_addresses));
    }

    std::string get_host_os_verion()
    {
        struct utsname host_info;
        std::stringstream ss;
        if (uname(&host_info) == 0){
            ss << "Operating system name: " << host_info.sysname << ", ";
            ss << "Node name: " << host_info.nodename << ", ";
            ss << "Operating system release: " << host_info.release << ", ";
            ss << "Operating system name: " << host_info.version << ", ";
            ss << "Hardware identifier: " << host_info.machine;
            return ss.str();
        }
        else {
            return "unknown";
        }
    }

    bool debugger_attached()
    {
        char buf[4096];

        const int status_fd = ::open("/proc/self/status", O_RDONLY);
        if (status_fd == -1)
            return false;

        const ssize_t num_read = ::read(status_fd, buf, sizeof(buf) - 1);
        ::close(status_fd);

        if (num_read <= 0)
            return false;

        buf[num_read] = '\0';
        constexpr char tracer_pid_string[] = "TracerPid:";
        const auto tracer_pid_ptr = ::strstr(buf, tracer_pid_string);
        if (!tracer_pid_ptr)
            return false;

        for (const char* character_ptr = tracer_pid_ptr + sizeof(tracer_pid_string) - 1; character_ptr <= buf + num_read; ++character_ptr)
        {
            if (::isspace(*character_ptr))
                continue;
            else
                return ::isdigit(*character_ptr) != 0 && *character_ptr != '0';
        }

        return false;
    }
#elif _WIN32 || _WIN64
    std::string get_mac_address() {
        return "unknown";
    }
    std::string get_host_os_verion()
    {
        return "unknown"
    }
    bool debugger_attached()
    {
        return false;
    }
#else
    std::string get_mac_address() {
        return "unknown";
    }
    std::string get_host_os_verion()
    {
        return "unknown"
    }
    bool debugger_attached()
    {
        return false;
    }
#endif

    void realm_analytics::send()
    {
#ifdef __ANDROID__
        return; // TODO: Implement metrics for Android.
#endif
        if (!debugger_attached() || getenv("REALM_DISABLE_ANALYTICS"))
            return;
        std::string os_name;
#ifdef _WIN32
        os_name = "Windows 32-bit";
#elif _WIN64
        os_name = "Windows 64-bit";
#elif __APPLE__ || __MACH__
        os_name = "macOS";
#elif __linux__
        os_name = "Linux";
#elif __FreeBSD__
        os_name = "FreeBSD";
#elif __unix || __unix__
        os_name = "Unix";
#else
        os_name = "Other";
#endif

        auto mac_address = get_mac_address();
        nlohmann::json post_data {
                {"event", "Run"},
                {"properties", {
                    {"token", "ce0fac19508f6c8f20066d345d360fd0"},
                    {"distinct_id", mac_address},
                    {"Anonymized MAC Address", mac_address},
                    {"Anonymized Bundle ID", "unknown"},
                    {"Binding", "cpp"},
#if __cplusplus >= 202002L
                    {"Language", "cpp20"},
#else
                    {"Language", "cpp17"},
#endif
                    {"Realm Version", "0.0.0"},
                    {"Target OS Type", "unknown"},
#if defined(__clang__)
                    {"Clang Version", __clang_version__},
                    {"Clang Major Version", __clang_major__},
#elif defined(__GNUC__) || defined(__GNUG__)
                    {"GCC Version", __GNUC__},
                    {"GCC Minor Version", __GNUC_MINOR__},
#endif
                    {"Host OS Type", os_name},
                    {"Host OS Version", get_host_os_verion()}
                }}
        };

        std::stringstream json_ss;
        json_ss << post_data;
        auto json_str = json_ss.str();
        auto transport = std::make_unique<internal::DefaultTransport>();

        std::vector<char> buffer;
        buffer.resize(5000);
        realm::util::base64_encode(json_str.c_str(), json_str.size(),
                                   buffer.data(), buffer.size());

        size_t s = 0;
        while(buffer[s] != '\0') {
            s++;
        }
        buffer.resize(s);

        auto base64_str = std::string(buffer.begin(), buffer.end());
        transport->send_request_to_server({
                                        .method = realm::app::HttpMethod::get,
                                        .url = util::format("https://data.mongodb-api.com/app/realmsdkmetrics-zmhtm/endpoint/metric_webhook/metric?data=%1", base64_str),
        }, [](auto) {
            // noop
        });
    }

} // namespace realm

