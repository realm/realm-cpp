#include <cpprealm/internal/bridge/mixed.hpp>
#include <cpprealm/internal/bridge/utils.hpp>
#include <realm/mixed.hpp>

namespace realm::internal::bridge {
#ifdef __i386__
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
#else
    static_assert(SizeCheck<24, sizeof(Mixed)>{});
    static_assert(SizeCheck<8, alignof(Mixed)>{});
#endif

    mixed::mixed(const std::nullopt_t& v) {
        new (&m_mixed) Mixed(v);
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
    mixed::mixed(const realm::Mixed &v) {
        if (v.get_type() == type_String) {
            m_owned_string = v.get_string();
        }
        new (&m_mixed) Mixed(v);
    }
    mixed::mixed(const struct binary &v) {
        new (&m_mixed) Mixed(static_cast<BinaryData>(v));
    }
    mixed::mixed(const struct obj_key &v) {
        new (&m_mixed) Mixed(static_cast<ObjKey>(v));
    }
    mixed::mixed(const bool &v) {
        new (&m_mixed) Mixed(v);
    }
    mixed::operator Mixed() const {
        if (!is_null() && type() == data_type::String) {
            return m_owned_string;
        }
        return *reinterpret_cast<const Mixed*>(m_mixed);
    }

    mixed::operator std::string() const {
        return m_owned_string;
    }
    mixed::operator bridge::binary() const {
        return reinterpret_cast<const Mixed*>(m_mixed)->get_binary();
    }
    mixed::operator bridge::timestamp() const {
        return reinterpret_cast<const Mixed*>(m_mixed)->get_timestamp();
    }
    mixed::operator bridge::obj_key() const {
        return reinterpret_cast<const Mixed*>(m_mixed)->get_link().get_obj_key();
    }
    mixed::operator bridge::uuid() const {
        return static_cast<const uuid &>(reinterpret_cast<const Mixed *>(m_mixed)->get_uuid());
    }
    mixed::operator int64_t() const {
        return static_cast<const int64_t &>(reinterpret_cast<const Mixed *>(m_mixed)->get_int());
    }
    mixed::operator double() const {
        return static_cast<const double &>(reinterpret_cast<const Mixed *>(m_mixed)->get_double());
    }
    mixed::operator bool() const {
        return static_cast<const bool &>(reinterpret_cast<const Mixed *>(m_mixed)->get_bool());
    }
    data_type mixed::type() const noexcept {
        return data_type(static_cast<int>(reinterpret_cast<const Mixed *>(m_mixed)->get_type()));
    }
    bool mixed::is_null() const noexcept {
        return reinterpret_cast<const Mixed *>(m_mixed)->is_null();
    }
}