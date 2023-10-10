#ifndef CPP_REALM_BRIDGE_RESULTS_HPP
#define CPP_REALM_BRIDGE_RESULTS_HPP

#include <cstddef>
#include <cpprealm/internal/bridge/obj.hpp>
#include <cpprealm/internal/bridge/utils.hpp>

namespace realm {
    class Results;
}
namespace realm::internal::bridge {
    struct realm;
    struct table;
    struct query;
    struct notification_token;
    struct obj;
    struct collection_change_set;

    struct results {
        results();
        results(const results& other) ;
        results& operator=(const results& other) ;
        results(results&& other);
        results& operator=(results&& other);
        ~results();

        results(const Results&); //NOLINT(google-explicit-constructor)
        results(const realm&, const table_view&);
        size_t size();
        [[nodiscard]] realm get_realm() const;
        [[nodiscard]] table get_table() const;
        results(const realm&, const query&);
        notification_token add_notification_callback(std::shared_ptr<collection_change_callback>&&);
    private:
        template <typename T>
        friend T get(results&, size_t);
#ifdef CPPREALM_HAVE_GENERATED_BRIDGE_TYPES
        storage::Results m_results[1];
#else
        std::shared_ptr<Results> m_results;
#endif
    };

    template <typename T>
    T get(results&, size_t);
    template <>
    obj get(results&, size_t);
}

#endif //CPP_REALM_BRIDGE_RESULTS_HPP
