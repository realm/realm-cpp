#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/internal/bridge/results.hpp>
#include <cpprealm/internal/bridge/realm.hpp>
#include <cpprealm/internal/bridge/table.hpp>
#include <cpprealm/internal/bridge/query.hpp>
#include <realm/object-store/results.hpp>

namespace realm::internal::bridge {
    results::results(const realm &realm, const query &query) {
        new (m_results) Results(realm, query);
    }

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

    results::results(Results &&v) {
        new (&m_results) Results(std::move(v));
    }

    notification_token results::add_notification_callback(std::function<void(collection_change_set)> &&fn) {
        return reinterpret_cast<Results*>(m_results)->add_notification_callback(fn);
    }
}