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

    struct sort_descriptor {
        std::string key_path;
        bool ascending;
    private:
        friend struct results;
        friend struct list;
        operator std::pair<std::string, bool>() const;
    };

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
        results sort(const std::vector<sort_descriptor>&);
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
    template <>
    int64_t get(results&, size_t);
    template <>
    bool get(results&, size_t);
    template <>
    double get(results&, size_t);
    template <>
    std::string get(results&, size_t);
    template <>
    ::realm::uuid get(results&, size_t);
    template <>
    ::realm::object_id get(results&, size_t);
    template <>
    ::realm::decimal128 get(results&, size_t);
    template <>
    std::vector<uint8_t> get(results&, size_t);
    template <>
    std::chrono::time_point<std::chrono::system_clock> get(results&, size_t);
    template <>
    mixed get(results&, size_t);
}

#endif //CPP_REALM_BRIDGE_RESULTS_HPP
