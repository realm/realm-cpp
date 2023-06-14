#include <cpprealm/experimental/managed_uuid.hpp>
#include <cpprealm/persisted.hpp>

namespace realm::experimental {
    __cpprealm_build_experimental_query(==, equal, realm::uuid)
    __cpprealm_build_experimental_query(!=, not_equal, realm::uuid)
    __cpprealm_build_optional_experimental_query(==, equal, realm::uuid)
    __cpprealm_build_optional_experimental_query(!=, not_equal, realm::uuid)
}