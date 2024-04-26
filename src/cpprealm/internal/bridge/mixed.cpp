#include <cpprealm/internal/bridge/mixed.hpp>
#include <cpprealm/internal/bridge/utils.hpp>

#include <realm/mixed.hpp>

namespace realm::internal::bridge {

#define CPPREALM_OPTIONAL_MIXED(type) \
    template<> \
    mixed::mixed(const std::optional<type>& o) { \
        if (o) { \
            *this = mixed(*o); \
        } else { \
            *this = mixed(std::monostate()); \
        } \
    }                                 \

    mixed::mixed() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_mixed) Mixed();
#else
        m_mixed = std::make_shared<Mixed>();
#endif
    }

    mixed::mixed(const mixed& other) {
        if (!other.is_null()) {
            if (other.type() == data_type::String) {
                m_owned_string = other.m_owned_string;
            } else if (other.type() == data_type::Binary) {
                m_owned_data = other.m_owned_data;
            }
        }

#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_mixed) Mixed(*reinterpret_cast<const Mixed*>(&other.m_mixed));
#else
        m_mixed = other.m_mixed;
#endif
    }

    mixed& mixed::operator=(const mixed& other) {
        if (this != &other) {
            if (!other.is_null()) {
                if (other.type() == data_type::String) {
                    m_owned_string = other.m_owned_string;
                } else if (other.type() == data_type::Binary) {
                    m_owned_data = other.m_owned_data;
                }
            }
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
            *reinterpret_cast<Mixed*>(&m_mixed) = *reinterpret_cast<const Mixed*>(&other.m_mixed);
#else
            m_mixed = other.m_mixed;
#endif
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
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_mixed) Mixed(std::move(*reinterpret_cast<Mixed*>(&other.m_mixed)));
#else
        m_mixed = std::move(other.m_mixed);
#endif
    }

    mixed& mixed::operator=(mixed&& other) {
        if (this != &other) {
            if (!other.is_null()) {
                if (other.type() == data_type::String) {
                    m_owned_string = std::move(other.m_owned_string);
                } else if (other.type() == data_type::Binary) {
                    m_owned_data = std::move(other.m_owned_data);
                }
            }
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
            *reinterpret_cast<Mixed*>(&m_mixed) = std::move(*reinterpret_cast<Mixed*>(&other.m_mixed));
#else
            m_mixed = std::move(other.m_mixed);
#endif
        }
        return *this;
    }

    mixed::~mixed() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<Mixed*>(&m_mixed)->~Mixed();
#endif
    }

    mixed::mixed(const std::monostate&) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_mixed) Mixed(std::nullopt);
#else
        m_mixed = std::make_shared<Mixed>(std::nullopt);
#endif
    }

    mixed::mixed(const std::string &v) {
        m_owned_string = v;
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_mixed) Mixed(v);
#else
        m_mixed = std::make_shared<Mixed>(v);
#endif
    }
    mixed::mixed(const timestamp &v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_mixed) Mixed(v);
#else
        m_mixed = std::make_shared<Mixed>(v);
#endif
    }
    mixed::mixed(const int &v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_mixed) Mixed(v);
#else
        m_mixed = std::make_shared<Mixed>(v);
#endif
    }
    mixed::mixed(const int64_t &v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_mixed) Mixed(v);
#else
        m_mixed = std::make_shared<Mixed>(v);
#endif
    }
    mixed::mixed(const double &v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_mixed) Mixed(v);
#else
        m_mixed = std::make_shared<Mixed>(v);
#endif
    }
    mixed::mixed(const struct uuid &v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_mixed) Mixed(v.operator UUID());
#else
        m_mixed = std::make_shared<Mixed>(v.operator UUID());
#endif
    }
    mixed::mixed(const struct object_id &v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_mixed) Mixed(v.operator ObjectId());
#else
        m_mixed = std::make_shared<Mixed>(v.operator ObjectId());
#endif
    }
    mixed::mixed(const struct decimal128 &v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_mixed) Mixed(v.operator Decimal128());
#else
        m_mixed = std::make_shared<Mixed>(v.operator Decimal128());
#endif
    }
    mixed::mixed(const realm::Mixed &v) {
        if (v.is_null()) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
            new (&m_mixed) Mixed();
#else
            m_mixed = std::make_shared<Mixed>();
#endif
        } else {
            if (v.get_type() == type_String) {
                m_owned_string = v.get_string();
            } else if (v.get_type() == type_Binary) {
                m_owned_data = v.get_binary();
            }
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
            new (&m_mixed) Mixed(v);
#else
            m_mixed = std::make_shared<Mixed>(v);
#endif
        }
    }
    mixed::mixed(const struct binary &v) {
        m_owned_data = v;
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_mixed) Mixed(v.operator BinaryData());
#else
        m_mixed = std::make_shared<Mixed>(v.operator BinaryData());
#endif
    }
    mixed::mixed(const struct obj_link &v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_mixed) Mixed(static_cast<ObjLink>(v));
#else
        m_mixed = std::make_shared<Mixed>(v.operator ObjLink());
#endif
    }
    mixed::mixed(const struct obj_key &v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_mixed) Mixed(static_cast<ObjKey>(v));
#else
        m_mixed = std::make_shared<Mixed>(v.operator ObjKey());
#endif
    }
    mixed::mixed(const bool &v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_mixed) Mixed(v);
#else
        m_mixed = std::make_shared<Mixed>(v);
#endif
    }

    CPPREALM_OPTIONAL_MIXED(std::string)
    CPPREALM_OPTIONAL_MIXED(timestamp)
    CPPREALM_OPTIONAL_MIXED(int)
    CPPREALM_OPTIONAL_MIXED(int64_t)
    CPPREALM_OPTIONAL_MIXED(double)
    CPPREALM_OPTIONAL_MIXED(struct uuid)
    CPPREALM_OPTIONAL_MIXED(struct object_id)
    CPPREALM_OPTIONAL_MIXED(struct decimal128)
    CPPREALM_OPTIONAL_MIXED(struct binary)
    CPPREALM_OPTIONAL_MIXED(struct obj_link)
    CPPREALM_OPTIONAL_MIXED(struct obj_key)
    CPPREALM_OPTIONAL_MIXED(bool)

    mixed::operator Mixed() const {
        if (!is_null()) {
            if (type() == data_type::String) {
                return m_owned_string;
            } else if (type() == data_type::Binary) {
                return m_owned_data.operator BinaryData();
            }
        }
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return *reinterpret_cast<const Mixed*>(m_mixed);
#else
        return *m_mixed;
#endif
    }

    mixed::operator std::string() const {
        return m_owned_string;
    }
    mixed::operator bridge::binary() const {
        return m_owned_data;
    }
    mixed::operator bridge::timestamp() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const Mixed*>(&m_mixed)->get_timestamp();
#else
        return m_mixed->get_timestamp();
#endif

    }
    mixed::operator bridge::obj_link() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const Mixed*>(&m_mixed)->get<ObjLink>();
#else
        return m_mixed->get<ObjLink>();
#endif
    }
    mixed::operator bridge::obj_key() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const Mixed*>(&m_mixed)->get<ObjKey>();
#else
        return m_mixed->get<ObjKey>();
#endif

    }
    mixed::operator bridge::uuid() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return static_cast<const uuid &>(reinterpret_cast<const Mixed *>(&m_mixed)->get_uuid());
#else
        return m_mixed->get_uuid();
#endif

    }
    mixed::operator bridge::object_id() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return static_cast<const object_id &>(reinterpret_cast<const Mixed *>(&m_mixed)->get_object_id());
#else
        return m_mixed->get_object_id();
#endif

    }
    mixed::operator bridge::decimal128() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return static_cast<const decimal128 &>(reinterpret_cast<const Mixed *>(&m_mixed)->get_decimal());
#else
        return m_mixed->get_decimal();
#endif

    }
    mixed::operator int64_t() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return static_cast<const int64_t &>(reinterpret_cast<const Mixed *>(&m_mixed)->get_int());
#else
        return m_mixed->get_int();
#endif

    }
    mixed::operator double() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return static_cast<const double &>(reinterpret_cast<const Mixed *>(&m_mixed)->get_double());
#else
        return m_mixed->get_double();
#endif

    }
    mixed::operator bool() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return static_cast<const bool &>(reinterpret_cast<const Mixed *>(&m_mixed)->get_bool());
#else
        return m_mixed->get_bool();
#endif

    }
    data_type mixed::type() const noexcept {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return data_type(static_cast<int>(reinterpret_cast<const Mixed *>(&m_mixed)->get_type()));
#else
        return data_type(static_cast<int>(m_mixed->get_type()));
#endif

    }
    bool mixed::is_null() const noexcept {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const Mixed *>(&m_mixed)->is_null();
#else
        return m_mixed->is_null();
#endif
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
