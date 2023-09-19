#include <cpprealm/internal/bridge/mixed.hpp>
#include <cpprealm/internal/bridge/utils.hpp>

#include <realm/mixed.hpp>

namespace realm::internal::bridge {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
    static_assert(LayoutCheck<storage::Mixed, Mixed>{});
#elif __i386__
    static_assert(SizeCheck<20, sizeof(Mixed)>{});
    static_assert(SizeCheck<4, alignof(Mixed)>{});
#elif __x86_64__
    static_assert(SizeCheck<24, sizeof(Mixed)>{});
    static_assert(SizeCheck<8, alignof(Mixed)>{});
#elif __arm__
    static_assert(SizeCheck<24, sizeof(Mixed)>{});
    static_assert(SizeCheck<8, alignof(Mixed)>{});
#elif __aarch64__
    static_assert(SizeCheck<24, sizeof(Mixed)>{});
    static_assert(SizeCheck<8, alignof(Mixed)>{});
#elif _WIN32
    static_assert(SizeCheck<24, sizeof(Mixed)>{});
    static_assert(SizeCheck<8, alignof(Mixed)>{});
#endif

#define CPPREALM_OPTIONAL_MIXED(type) \
    template<> \
    mixed::mixed(const std::optional<type>& o) { \
        if (o) { \
            *this = mixed(*o); \
        } else { \
            new (&m_mixed) Mixed(realm::none); \
        } \
    }                                 \

    mixed::mixed() {
        new (&m_mixed) Mixed();
    }

    mixed::mixed(const mixed& other) {
        if (other.type() == data_type::String) {
            m_owned_string = other.m_owned_string;
        } else if (other.type() == data_type::Binary) {
            m_owned_data = other.m_owned_data;
        }
        new (&m_mixed) Mixed(*reinterpret_cast<const Mixed*>(&other.m_mixed));
    }

    mixed& mixed::operator=(const mixed& other) {
        if (this != &other) {
            if (other.type() == data_type::String) {
                m_owned_string = other.m_owned_string;
            } else if (other.type() == data_type::Binary) {
                m_owned_data = other.m_owned_data;
            }
            *reinterpret_cast<Mixed*>(&m_mixed) = *reinterpret_cast<const Mixed*>(&other.m_mixed);
        }
        return *this;
    }

    mixed::mixed(mixed&& other) {
        if (!other.is_null()) {
            if (other.type() == data_type::String) {
                m_owned_string = std::move(other.m_owned_string);
            } else if (other.type() == data_type::Binary) {
                m_owned_data = std::move(other.m_owned_data);
            }
        }
        new (&m_mixed) Mixed(std::move(*reinterpret_cast<Mixed*>(&other.m_mixed)));
    }

    mixed& mixed::operator=(mixed&& other) {
        if (this != &other) {
            if (other.type() == data_type::String) {
                m_owned_string = std::move(other.m_owned_string);
            } else if (other.type() == data_type::Binary) {
                m_owned_data = std::move(other.m_owned_data);
            }
            *reinterpret_cast<Mixed*>(&m_mixed) = std::move(*reinterpret_cast<Mixed*>(&other.m_mixed));
        }
        return *this;
    }

    mixed::~mixed() {
        reinterpret_cast<Mixed*>(&m_mixed)->~Mixed();
    }

    mixed::mixed(const std::monostate&) {
        new (&m_mixed) Mixed(std::nullopt);
    }

    mixed::mixed(const std::string &v) {
        m_owned_string = v;
        new (&m_mixed) Mixed(v);
    }
    mixed::mixed(const timestamp &v) {
        new (&m_mixed) Mixed(v);
    }
    mixed::mixed(const int &v) {
        new (&m_mixed) Mixed(v);
    }
    mixed::mixed(const int64_t &v) {
        new (&m_mixed) Mixed(v);
    }
    mixed::mixed(const double &v) {
        new (&m_mixed) Mixed(v);
    }
    mixed::mixed(const struct uuid &v) {
        new (&m_mixed) Mixed(static_cast<UUID>(v));
    }
    mixed::mixed(const struct object_id &v) {
        new (&m_mixed) Mixed(static_cast<ObjectId>(v));
    }
    mixed::mixed(const struct decimal128 &v) {
        new (&m_mixed) Mixed(static_cast<Decimal128>(v));
    }
    mixed::mixed(const realm::Mixed &v) {
        if (v.is_null()) {
            new (&m_mixed) Mixed();
        } else {
            if (v.get_type() == type_String) {
                m_owned_string = v.get_string();
            } else if (v.get_type() == type_Binary) {
                m_owned_data = v.get_binary();
            }
            new (&m_mixed) Mixed(v);
        }
    }
    mixed::mixed(const struct binary &v) {
        m_owned_data = v;
        new (&m_mixed) Mixed(static_cast<BinaryData>(v));
    }
    mixed::mixed(const struct obj_link &v) {
        new (&m_mixed) Mixed(static_cast<ObjLink>(v));
    }
    mixed::mixed(const struct obj_key &v) {
        new (&m_mixed) Mixed(static_cast<ObjKey>(v));
    }
    mixed::mixed(const bool &v) {
        new (&m_mixed) Mixed(v);
    }

    CPPREALM_OPTIONAL_MIXED(std::string);
    CPPREALM_OPTIONAL_MIXED(timestamp);
    CPPREALM_OPTIONAL_MIXED(int);
    CPPREALM_OPTIONAL_MIXED(int64_t);
    CPPREALM_OPTIONAL_MIXED(double);
    CPPREALM_OPTIONAL_MIXED(struct uuid);
    CPPREALM_OPTIONAL_MIXED(struct object_id);
    CPPREALM_OPTIONAL_MIXED(struct decimal128);
    CPPREALM_OPTIONAL_MIXED(struct binary);
    CPPREALM_OPTIONAL_MIXED(struct obj_link);
    CPPREALM_OPTIONAL_MIXED(struct obj_key);
    CPPREALM_OPTIONAL_MIXED(bool);

    mixed::operator Mixed() const {
        if (!is_null()) {
            if (type() == data_type::String) {
                return m_owned_string;
            } else if (type() == data_type::Binary) {
                return m_owned_data.operator BinaryData();
            }
        }
        return *reinterpret_cast<const Mixed*>(m_mixed);
    }

    mixed::operator std::string() const {
        return m_owned_string;
    }
    mixed::operator bridge::binary() const {
        return m_owned_data;
    }
    mixed::operator bridge::timestamp() const {
        return reinterpret_cast<const Mixed*>(&m_mixed)->get_timestamp();
    }
    mixed::operator bridge::obj_link() const {
        return reinterpret_cast<const Mixed*>(&m_mixed)->get<ObjLink>();
    }
    mixed::operator bridge::obj_key() const {
        return reinterpret_cast<const Mixed*>(&m_mixed)->get<ObjKey>();
    }
    mixed::operator bridge::uuid() const {
        return static_cast<const uuid &>(reinterpret_cast<const Mixed *>(&m_mixed)->get_uuid());
    }
    mixed::operator bridge::object_id() const {
        return static_cast<const object_id &>(reinterpret_cast<const Mixed *>(&m_mixed)->get_object_id());
    }
    mixed::operator bridge::decimal128() const {
        return static_cast<const decimal128 &>(reinterpret_cast<const Mixed *>(&m_mixed)->get_decimal());
    }
    mixed::operator int64_t() const {
        return static_cast<const int64_t &>(reinterpret_cast<const Mixed *>(&m_mixed)->get_int());
    }
    mixed::operator double() const {
        return static_cast<const double &>(reinterpret_cast<const Mixed *>(&m_mixed)->get_double());
    }
    mixed::operator bool() const {
        return static_cast<const bool &>(reinterpret_cast<const Mixed *>(&m_mixed)->get_bool());
    }
    data_type mixed::type() const noexcept {
        return data_type(static_cast<int>(reinterpret_cast<const Mixed *>(&m_mixed)->get_type()));
    }
    bool mixed::is_null() const noexcept {
        return reinterpret_cast<const Mixed *>(&m_mixed)->is_null();
    }
#define __cpp_realm_gen_mixed_op(op) \
    bool operator op(const mixed& a, const mixed& b) { \
        return a.operator Mixed() op b.operator Mixed(); \
    }

    __cpp_realm_gen_mixed_op(==)
    __cpp_realm_gen_mixed_op(!=)
    __cpp_realm_gen_mixed_op(>)
    __cpp_realm_gen_mixed_op(<)
    __cpp_realm_gen_mixed_op(>=)
    __cpp_realm_gen_mixed_op(<=)
}
