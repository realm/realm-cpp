#ifndef CPP_REALM_BRIDGE_MIXED_HPP
#define CPP_REALM_BRIDGE_MIXED_HPP

#include <string>
#include <optional>
#include <variant>
#include <cpprealm/internal/bridge/property.hpp>
#include <cpprealm/internal/bridge/binary.hpp>
#include <cpprealm/internal/bridge/uuid.hpp>
#include <cpprealm/internal/bridge/timestamp.hpp>
#include <cpprealm/internal/bridge/obj_key.hpp>
#include <cpprealm/internal/bridge/object_id.hpp>

namespace realm {
    class Mixed;

    template<typename T>
    struct object_base;

    using mixed = std::variant<
            std::monostate,
            int64_t,
            bool,
            std::string,
            double,
            std::vector<uint8_t>,
            std::chrono::time_point<std::chrono::system_clock>,
            uuid,
            object_id>;
}

namespace realm::internal::bridge {
//    struct realm;
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
        mixed();
        mixed(const mixed& other) ;
        mixed& operator=(const mixed& other) ;
        mixed(mixed&& other);
        mixed& operator=(mixed&& other);
        ~mixed();

        explicit mixed(const std::string&);
        mixed(const std::monostate&); //NOLINT(google-explicit-constructor)
        mixed(const int&); //NOLINT(google-explicit-constructor)
        mixed(const int64_t&); //NOLINT(google-explicit-constructor)
        mixed(const double&); //NOLINT(google-explicit-constructor)
        mixed(const bool&); //NOLINT(google-explicit-constructor)
        mixed(const struct uuid&); //NOLINT(google-explicit-constructor)
        mixed(const struct object_id&); //NOLINT(google-explicit-constructor)
        mixed(const struct timestamp&); //NOLINT(google-explicit-constructor)
        mixed(const struct obj_link&); //NOLINT(google-explicit-constructor)
        mixed(const struct obj_key&); //NOLINT(google-explicit-constructor)
        mixed(const struct binary&); //NOLINT(google-explicit-constructor)
        mixed(const Mixed&); //NOLINT(google-explicit-constructor)
        template<typename T>
        mixed(const std::optional<T>& o);  //NOLINT(google-explicit-constructor)
        operator std::string() const; //NOLINT(google-explicit-constructor)
        operator int64_t() const; //NOLINT(google-explicit-constructor)
        operator double() const; //NOLINT(google-explicit-constructor)
        operator bool() const; //NOLINT(google-explicit-constructor)
        operator bridge::uuid() const; //NOLINT(google-explicit-constructor)
        operator bridge::object_id() const; //NOLINT(google-explicit-constructor)
        operator bridge::timestamp() const; //NOLINT(google-explicit-constructor)
        operator bridge::obj_link() const; //NOLINT(google-explicit-constructor)
        operator bridge::obj_key() const; //NOLINT(google-explicit-constructor)
        operator bridge::binary() const; //NOLINT(google-explicit-constructor)

        explicit operator Mixed() const;

        [[nodiscard]] data_type type() const noexcept;
        [[nodiscard]] bool is_null() const noexcept;
    private:
        std::string m_owned_string;
#ifdef __i386__
        std::aligned_storage<20, 4>::type m_mixed[1];
#elif __x86_64__
        std::aligned_storage<24, 8>::type m_mixed[1];
#elif __arm__
        std::aligned_storage<24, 8>::type m_mixed[1];
#elif __aarch64__
        std::aligned_storage<24, 8>::type m_mixed[1];
#elif _WIN32
        std::aligned_storage<1, 1>::type m_mixed[1];
#endif
        //friend bool operator ==(const mixed&, const mixed&);
        //friend bool operator !=(const mixed&, const mixed&);
        //friend bool operator >(const mixed&, const mixed&);
        //friend bool operator <(const mixed&, const mixed&);
        //friend bool operator >=(const mixed&, const mixed&);
        //friend bool operator <=(const mixed&, const mixed&);
    };

    //bool operator ==(const mixed&, const mixed&);
    //bool operator !=(const mixed&, const mixed&);
    //bool operator >(const mixed&, const mixed&);
    //bool operator <(const mixed&, const mixed&);
    //bool operator >=(const mixed&, const mixed&);
   // bool operator <=(const mixed&, const mixed&);
}


#endif //CPP_REALM_BRIDGE_MIXED_HPP
