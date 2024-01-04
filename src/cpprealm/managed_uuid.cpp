#include <cpprealm/managed_uuid.hpp>
#include <cpprealm/rbool.hpp>

namespace realm {
    __cpprealm_build_query(==, equal, realm::uuid)
    __cpprealm_build_query(!=, not_equal, realm::uuid)
    __cpprealm_build_optional_query(==, equal, realm::uuid)
    __cpprealm_build_optional_query(!=, not_equal, realm::uuid)
}