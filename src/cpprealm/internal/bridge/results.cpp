#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/internal/bridge/results.hpp>
#include <cpprealm/internal/bridge/realm.hpp>
#include <cpprealm/internal/bridge/table.hpp>
#include <cpprealm/internal/bridge/query.hpp>
#include <realm/object-store/results.hpp>
#include <cpprealm/internal/bridge/utils.hpp>

namespace realm::internal::bridge {
#ifdef __i386__
    static_assert(SizeCheck<496, sizeof(Results)>{});
    static_assert(SizeCheck<4, alignof(Results)>{});
#elif __x86_64__
    #if defined(__clang__)
    static_assert(SizeCheck<896, sizeof(Results)>{});
    #elif defined(__GNUC__) || defined(__GNUG__)
    static_assert(SizeCheck<912, sizeof(Results)>{});
    #endif
    static_assert(SizeCheck<8, alignof(Results)>{});
#elif __arm__
    static_assert(SizeCheck<568, sizeof(Results)>{});
    static_assert(SizeCheck<8, alignof(Results)>{});
#elif __aarch64__
#if defined(__clang__)
    static_assert(SizeCheck<896, sizeof(Results)>{});
#elif defined(__GNUC__) || defined(__GNUG__)
    static_assert(SizeCheck<912, sizeof(Results)>{});
#endif
    static_assert(SizeCheck<8, alignof(Results)>{});
#endif

    results::results() {
        new (&m_results) Results();
    }

    results::results(const results& other) {
        new (&m_results) Results(*reinterpret_cast<const Results*>(&other.m_results));
    }

    results& results::operator=(const results& other) {
        if (this != &other) {
            *reinterpret_cast<Results*>(&m_results) = *reinterpret_cast<const Results*>(&other.m_results);
        }
        return *this;
    }

    results::results(results&& other) {
        new (&m_results) Results(std::move(*reinterpret_cast<Results*>(&other.m_results)));
    }

    results& results::operator=(results&& other) {
        if (this != &other) {
            *reinterpret_cast<Results*>(&m_results) = std::move(*reinterpret_cast<Results*>(&other.m_results));
        }
        return *this;
    }

    results::~results() {
        reinterpret_cast<Results*>(&m_results)->~Results();
    }

    results::results(const realm &realm, const query &query) {
        new (&m_results) Results(realm, query);
    }

    results::results(const Results &v) {
        new (&m_results) Results(v);
    }

    size_t results::size() {
        return reinterpret_cast<Results*>(&m_results)->size();
    }

    realm results::get_realm() const {
        return reinterpret_cast<const Results*>(&m_results)->get_realm();
    }

    table results::get_table() const {
        return reinterpret_cast<const Results*>(&m_results)->get_table();
    }

    template <>
    obj get(results& res, size_t v) {
        return reinterpret_cast<Results*>(&res.m_results)-> template get(v);
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
        return reinterpret_cast<Results*>(&m_results)->add_notification_callback(ccb);
    }
}
