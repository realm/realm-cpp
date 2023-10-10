#include <cpprealm/experimental/managed_objectid.hpp>
#include <cpprealm/rbool.hpp>

namespace realm::experimental {
    __cpprealm_build_experimental_query(==, equal, realm::object_id);
    __cpprealm_build_experimental_query(!=, not_equal, realm::object_id);
    __cpprealm_build_optional_experimental_query(==, equal, realm::object_id);
    __cpprealm_build_optional_experimental_query(!=, not_equal, realm::object_id);
}
