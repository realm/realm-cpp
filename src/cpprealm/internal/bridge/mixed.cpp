#include <cpprealm/internal/bridge/mixed.hpp>
#include <cpprealm/internal/bridge/utils.hpp>
#include <realm/mixed.hpp>

namespace realm::internal::bridge {
    static_assert(SizeCheck<24, sizeof(Mixed)>{});

    mixed::mixed(const int64_t &v) {
        new (&m_mixed) Mixed(v);
    }
    mixed::mixed(const struct uuid &v) {
        new (&m_mixed) Mixed(static_cast<UUID>(v));
    }
    mixed::mixed(const realm::Mixed &v) {
        new (&m_mixed) Mixed(v);
    }
    mixed::mixed(const struct binary &v) {
        new (&m_mixed) Mixed(static_cast<BinaryData>(v));
    }
    mixed::mixed(const struct obj_key &v) {
        new (&m_mixed) Mixed(static_cast<ObjKey>(v));
    }
    mixed::operator Mixed() const {
        return reinterpret_cast<const Mixed*>(m_mixed);
    }

    mixed::operator bridge::binary() const {
        return reinterpret_cast<const Mixed*>(m_mixed)->get_binary();
    }

    mixed::operator bridge::obj_key() const {
        return reinterpret_cast<const Mixed*>(m_mixed)->get_link().get_obj_key();
    }

    mixed::operator bridge::uuid() const {
        return static_cast<const uuid &>(reinterpret_cast<const Mixed *>(m_mixed)->get_uuid());
    }
}