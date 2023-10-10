#include <cpprealm/experimental/managed_timestamp.hpp>
#include <cpprealm/rbool.hpp>

namespace realm::experimental {
    __cpprealm_build_experimental_query(==, equal, std::chrono::time_point<std::chrono::system_clock>);
    __cpprealm_build_experimental_query(!=, not_equal, std::chrono::time_point<std::chrono::system_clock>);
    __cpprealm_build_experimental_query(>, greater, std::chrono::time_point<std::chrono::system_clock>);
    __cpprealm_build_experimental_query(>=, greater_equal, std::chrono::time_point<std::chrono::system_clock>);
    __cpprealm_build_experimental_query(<, less, std::chrono::time_point<std::chrono::system_clock>);
    __cpprealm_build_experimental_query(<=, less_equal, std::chrono::time_point<std::chrono::system_clock>);

    __cpprealm_build_optional_experimental_query(==, equal, std::chrono::time_point<std::chrono::system_clock>);
    __cpprealm_build_optional_experimental_query(!=, not_equal, std::chrono::time_point<std::chrono::system_clock>);
};
