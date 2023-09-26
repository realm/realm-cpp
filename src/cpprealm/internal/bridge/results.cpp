#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/internal/bridge/results.hpp>
#include <cpprealm/internal/bridge/realm.hpp>
#include <cpprealm/internal/bridge/table.hpp>
#include <cpprealm/internal/bridge/query.hpp>
#include <realm/object-store/results.hpp>

namespace realm::internal::bridge {
    results::results() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_results) Results();
#else
        m_results = std::make_shared<Results>();
#endif
    }

    results::results(const results& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_results) Results(*reinterpret_cast<const Results*>(&other.m_results));
#else
        m_results = other.m_results;
#endif
    }

    results& results::operator=(const results& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<Results*>(&m_results) = *reinterpret_cast<const Results*>(&other.m_results);
        }
#else
        m_results = other.m_results;
#endif
        return *this;
    }

    results::results(results&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_results) Results(std::move(*reinterpret_cast<Results*>(&other.m_results)));
#else
        m_results = std::move(other.m_results);
#endif
    }

    results& results::operator=(results&& other) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        if (this != &other) {
            *reinterpret_cast<Results*>(&m_results) = std::move(*reinterpret_cast<Results*>(&other.m_results));
        }
#else
        m_results = std::move(other.m_results);
#endif
        return *this;
    }

    results::~results() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        reinterpret_cast<Results*>(&m_results)->~Results();
#endif
    }

    results::results(const realm &realm, const query &query) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_results) Results(realm, query);
#else
        m_results = std::make_shared<Results>(Results(realm, query));
#endif
    }

    results::results(const Results &v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_results) Results(v);
#else
        m_results = std::make_shared<Results>(v);
#endif
    }

    size_t results::size() {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<Results*>(&m_results)->size();
#else
        return m_results->size();
#endif
    }

    realm results::get_realm() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const Results*>(&m_results)->get_realm();
#else
        return m_results->get_realm();
#endif
    }

    table results::get_table() const {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<const Results*>(&m_results)->get_table();
#else
        return m_results->get_table();
#endif
    }

    template <>
    obj get(results& res, size_t v) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<Results*>(&res.m_results)->get(v);
#else
        return res.m_results->get(v);
#endif
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
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        return reinterpret_cast<Results*>(&m_results)->add_notification_callback(ccb);
#else
        return m_results->add_notification_callback(ccb);
#endif
    }

    results::results(const realm &realm, const table_view &tv) {
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        new (&m_results) Results(realm, tv);
#else
        m_results = std::make_shared<Results>(Results(realm, tv));
#endif
    }
}
