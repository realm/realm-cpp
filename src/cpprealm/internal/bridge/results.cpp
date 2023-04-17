#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/internal/bridge/results.hpp>
#include <cpprealm/internal/bridge/realm.hpp>
#include <cpprealm/internal/bridge/table.hpp>
#include <cpprealm/internal/bridge/query.hpp>
#include <realm/object-store/results.hpp>

namespace realm::internal::bridge {
    results::results(const realm &realm, const query &query) {
        m_results = std::make_shared<Results>(Results(realm, query));
    }

    results::results(const Results &v) {
        m_results = std::make_shared<Results>(v);
    }

    size_t results::size() {
        return m_results->size();
    }

    realm results::get_realm() const {
        return m_results->get_realm();
    }

    table results::get_table() const {
        return m_results->get_table();
    }

    template <>
    obj get(results& res, size_t v) {
        return res.m_results-> template get(v);
    }

    notification_token results::add_notification_callback(std::shared_ptr<collection_change_callback> &&cb) {
        struct wrapper : CollectionChangeCallback {
            std::shared_ptr<collection_change_callback> m_cb;
            explicit wrapper(std::shared_ptr<collection_change_callback>&& cb)
                    : m_cb(std::move(cb)) {}
            void before(const CollectionChangeSet& v) const {
                m_cb->before(v);
            }
            void after(const CollectionChangeSet& v) const {
                m_cb->after(v);
            }
        } ccb(std::move(cb));
        return m_results->add_notification_callback(ccb);
    }
}
