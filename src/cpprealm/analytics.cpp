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
#import <sys/socket.h>
#import <sys/sysctl.h>
#import <net/if.h>
#import <net/if_dl.h>
#elif __linux__
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <netdb.h>
#endif

#import <CommonCrypto/CommonDigest.h>

namespace realm {
#if __APPLE__ || __MACH__
	// Wrapper for sysctl() that handles the memory management stuff
	static auto do_sysctl(int *mib, u_int mibSize, size_t *bufferSize) {
		std::unique_ptr<void, decltype(&free)> buffer(nullptr, &free);

		int ret = sysctl(mib, mibSize, nullptr, bufferSize, nullptr, 0);
		if (ret != 0) {
			return buffer;
		}

		buffer.reset(malloc(*bufferSize));
		if (!buffer) {
			return buffer;
		}

		ret = sysctl(mib, mibSize, buffer.get(), bufferSize, nullptr, 0);
		if (ret != 0) {
			buffer.reset();
		}

		return buffer;
	}

	// Hash the data in the given buffer and convert it to a hex-format string
	static std::string hash_data(const void *bytes, size_t length) {
		unsigned char buffer[CC_SHA256_DIGEST_LENGTH];
		CC_SHA256(bytes, static_cast<CC_LONG>(length), buffer);

		char formatted[CC_SHA256_DIGEST_LENGTH * 2 + 1];
		for (int i = 0; i < CC_SHA256_DIGEST_LENGTH; ++i) {
			snprintf(formatted + i * 2, sizeof(formatted) - i * 2, "%02x", buffer[i]);
		}

		return formatted;
	}

	std::string get_mac_address() {
		int en0 = static_cast<int>(if_nametoindex("en0"));
		if (!en0) {
			return nil;
		}

		std::array<int, 6> mib = {{CTL_NET, PF_ROUTE, 0, AF_LINK, NET_RT_IFLIST, en0}};
		size_t bufferSize;
		auto buffer = do_sysctl(&mib[0], mib.size(), &bufferSize);
		if (!buffer) {
			return nil;
		}

		// sockaddr_dl struct is immediately after the if_msghdr struct in the buffer
		auto sockaddr = reinterpret_cast<sockaddr_dl *>(static_cast<if_msghdr *>(buffer.get()) + 1);
		auto mac = reinterpret_cast<const unsigned char *>(sockaddr->sdl_data + sockaddr->sdl_nlen);

		return hash_data(mac, 6);
	}
#elif __linux__
	std::string get_mac_address() {
		return "unknown";
	}
#elif _WIN32 || _WIN64
	std::string get_mac_address() {
		return "unknown";
	}
#else
	std::string get_mac_address() {
		return "unknown";
	}
#endif

	void realm_analytics::send()
	{
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
				{"distinct_id", "some hashedMACAddress"},
				{"Anonymized MAC Address", "some hashedMACAddress"},
				{"Anonymized Bundle ID", "some app id"},
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
#endif
				{"Host OS Type", os_name},
				{"Host OS Version", "unknown"}

			}}
		};
	}

} // namespace realm

