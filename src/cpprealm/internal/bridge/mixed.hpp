#ifndef CPP_REALM_BRIDGE_MIXED_HPP
#define CPP_REALM_BRIDGE_MIXED_HPP

#include <string>
#include <cpprealm/internal/bridge/property.hpp>
#include <cpprealm/internal/bridge/binary.hpp>
#include <cpprealm/internal/bridge/uuid.hpp>
#include <cpprealm/internal/bridge/timestamp.hpp>
#include <cpprealm/internal/bridge/obj_key.hpp>

namespace realm {
    class Mixed;
}

namespace realm::internal::bridge {
    enum class data_type {
        // Note: Value assignments must be kept in sync with <realm/column_type.h>
        // Note: Any change to this enum is a file-format breaking change.
        Int = 0,
        Bool = 1,
        String = 2,
        Binary = 4,
        Mixed = 6,
        Timestamp = 8,
        Float = 9,
        Double = 10,
        Decimal = 11,
        Link = 12,
        LinkList = 13,
        ObjectId = 15,
        TypedLink = 16,
        UUID = 17,
    };

    struct mixed {
//        explicit mixed(const std::variant<
//                int64_t,
//                bool,
//                std::string,
//                double,
//                std::vector<uint8_t>,
//                std::chrono::time_point<std::chrono::system_clock>,
//                ::realm::uuid>&);
        mixed(const std::string&);
        mixed(const int&); //NOLINT(google-explicit-constructor)
        mixed(const int64_t&); //NOLINT(google-explicit-constructor)
        template <typename E>
        mixed(std::enable_if_t<std::is_enum_v<E>, E> v)
        : mixed(static_cast<int64_t>(v))
        { //NOLINT(google-explicit-constructor)
        }
        mixed(const double&); //NOLINT(google-explicit-constructor)
        mixed(const bool&); //NOLINT(google-explicit-constructor)
        mixed(const struct uuid&); //NOLINT(google-explicit-constructor)
        mixed(const struct timestamp&); //NOLINT(google-explicit-constructor)
        mixed(const struct obj_key&); //NOLINT(google-explicit-constructor)
        mixed(const struct binary&); //NOLINT(google-explicit-constructor)
        mixed(const mixed&) = default;
        mixed(const Mixed&); //NOLINT(google-explicit-constructor)

        operator std::string() const; //NOLINT(google-explicit-constructor)
        operator int64_t() const; //NOLINT(google-explicit-constructor)
        operator double() const; //NOLINT(google-explicit-constructor)
        operator bool() const; //NOLINT(google-explicit-constructor)
        operator bridge::uuid() const; //NOLINT(google-explicit-constructor)
        operator bridge::timestamp() const; //NOLINT(google-explicit-constructor)
        operator bridge::obj_key() const; //NOLINT(google-explicit-constructor)
        operator bridge::binary() const; //NOLINT(google-explicit-constructor)

        explicit operator Mixed() const;

        [[nodiscard]] data_type type() const noexcept;
    private:
        std::string m_owned_string;
        unsigned char m_mixed[48]{};
    };
}


#endif //CPP_REALM_BRIDGE_MIXED_HPP