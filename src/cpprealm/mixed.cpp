#include <cpprealm/mixed.hpp>
#include <cpprealm/types.hpp>

namespace realm {

    mixed::mixed(std::monostate) {
        m_mixed = internal::bridge::mixed();
    }

    mixed::mixed(int64_t v) {
        new (&m_mixed) internal::bridge::mixed(v);
        m_storage_mode = storage_mode::mixed_bridge;
    }

    mixed::mixed(bool v) {
        m_mixed = internal::bridge::mixed(v);
    }

    mixed::mixed(const std::string& v) {
        m_mixed = internal::bridge::mixed(v);
        m_storage_mode = storage_mode::mixed_bridge;
    }

    mixed::mixed(const char* v) {
        new (&m_mixed) internal::bridge::mixed(std::string(v));
        m_storage_mode = storage_mode::mixed_bridge;
    }

    mixed::mixed(double v) {
        new (&m_mixed) internal::bridge::mixed(v);
        m_storage_mode = storage_mode::mixed_bridge;
    }

    mixed::mixed(std::vector<uint8_t> v) {
        m_mixed = internal::bridge::mixed(v);
    }

    mixed::mixed(std::chrono::time_point<std::chrono::system_clock> v) {
        m_mixed = internal::bridge::mixed(v);
    }

    mixed::mixed(uuid v) {
        m_mixed = internal::bridge::mixed(serialize(v));
    }

    mixed::mixed(object_id v) {
        m_mixed = internal::bridge::mixed(serialize(v));
    }

    mixed::mixed(decimal128 v) {
        m_mixed = internal::bridge::mixed(serialize(v));
    }

    mixed::map mixed::get_dictionary() const {
        if (m_storage_mode == storage_mode::managed) {
//            auto x = m_obj->get_dictionary(*m_col_key);
//            auto xx = x.get_pair(0);
//            auto xxx = xx.second.operator std::string();
//            auto xxxx = xxx;
            return mixed::map(m_obj->get_dictionary(*m_col_key));
        } else if (m_storage_mode == storage_mode::dictionary) {
            return mixed::map(m_unmanaged_map);
        } else {
            throw std::runtime_error("mixed is not dictionary type");
        }

        return std::map<std::string, mixed>();
    }

    bool operator==(const realm::mixed& lhs, const realm::mixed& rhs) {
        return lhs.m_mixed == rhs.m_mixed;
    }
    bool operator!=(const realm::mixed& lhs, const realm::mixed& rhs) {
        return lhs.m_mixed != rhs.m_mixed;
    }
    bool operator>(const realm::mixed& lhs, const realm::mixed& rhs) {
        return lhs.m_mixed > rhs.m_mixed;
    }
    bool operator>=(const realm::mixed& lhs, const realm::mixed& rhs) {
        return lhs.m_mixed >= rhs.m_mixed;
    }
    bool operator<(const realm::mixed& lhs, const realm::mixed& rhs) {
        return lhs.m_mixed < rhs.m_mixed;
    }
    bool operator<=(const realm::mixed& lhs, const realm::mixed& rhs) {
        return lhs.m_mixed <= rhs.m_mixed;
    }

    namespace internal {
        internal::bridge::mixed get_mixed_bridge(const mixed& m) {
            return m.m_mixed;
        }
    }

} // namespace realm