#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/internal/bridge/results.hpp>
#include <cpprealm/internal/bridge/realm.hpp>
#include <cpprealm/internal/bridge/table.hpp>
#include <realm/object-store/results.hpp>

namespace realm::internal::bridge {
    size_t results::size() {
        return reinterpret_cast<Results*>(m_results)->size();
    }

    realm results::get_realm() const {
        return reinterpret_cast<const Results*>(m_results)->get_realm();
    }

    table results::get_table() const {
        return reinterpret_cast<const Results*>(m_results)->get_table();
    }

    template <>
    obj results::get(size_t v) {
        return reinterpret_cast<Results*>(m_results)->template get(v);
    }
}