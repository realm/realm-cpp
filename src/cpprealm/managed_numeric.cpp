#include "managed_numeric.hpp"

namespace realm {
    __cpprealm_build_query(==, equal, bool)
    __cpprealm_build_query(!=, not_equal, bool)

    __cpprealm_build_optional_query(==, equal, int64_t)
    __cpprealm_build_optional_query(!=, not_equal, int64_t)

    __cpprealm_build_optional_query(==, equal, double)
    __cpprealm_build_optional_query(!=, not_equal, double)

    __cpprealm_build_optional_query(==, equal, bool)
    __cpprealm_build_optional_query(!=, not_equal, bool)
}