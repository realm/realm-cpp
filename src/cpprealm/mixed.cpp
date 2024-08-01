#include <cpprealm/mixed.hpp>
#include <cpprealm/types.hpp>
#include <cpprealm/internal/bridge/obj.hpp>

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

            if (m_nested_managed_map) {
                return mixed::map(*m_nested_managed_map);
            }

            return mixed::map(m_obj->get_dictionary(*m_col_key), *m_col_key);
        } else if (m_storage_mode == storage_mode::unmanaged_dictionary) {
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

    internal::bridge::dictionary context::create_nested_dictionary(internal::bridge::dictionary& d, const std::string& key) {
        d.insert_dictionary(key);
        return d.get_dictionary(key);
    }

    void context::insert_dictionary_value(internal::bridge::dictionary& d, const std::string& key, const mixed& value) {
        switch (value.m_storage_mode) {
            case mixed::storage_mode::uninitialized:
                break;
            case mixed::storage_mode::mixed_bridge:
                d.insert(key, value.m_mixed);
                break;
            case mixed::storage_mode::managed:
                break;
            case mixed::storage_mode::unmanaged_object:
                break;
            case mixed::storage_mode::unmanaged_dictionary: {
                auto nested_dict = create_nested_dictionary(d, key);
                insert_dictionary_values(nested_dict, value.m_unmanaged_map);
                break;
            }
            case mixed::storage_mode::vector:
                break;
        }
    }

    void context::insert_dictionary_values(internal::bridge::dictionary& d, const std::map<std::string, mixed>& values) {
        for (auto& [k, value] : values) {
            insert_dictionary_value(d, k, value);
        }
    }

    void context::set_managed(internal::bridge::obj& o,
                     const internal::bridge::col_key& c,
                     const internal::bridge::realm& r,
                     const realm::mixed& v) {

        switch (v.m_storage_mode) {
            case mixed::storage_mode::uninitialized:
                break;
            case mixed::storage_mode::mixed_bridge:
                break;
            case mixed::storage_mode::managed:
                break;
            case mixed::storage_mode::unmanaged_object:
                break;
            case mixed::storage_mode::unmanaged_dictionary: {
                o.set_dictionary(c);
                auto obj = internal::bridge::object(r, o);
                auto d = obj.get_dictionary(c);
                insert_dictionary_values(d, v.m_unmanaged_map);
                break;
            }
            case mixed::storage_mode::vector:
                break;
        }
    }




} // namespace realm