#include <cpprealm/managed_objectid.hpp>
#include <cpprealm/rbool.hpp>

namespace realm {
    __cpprealm_build_query(==, equal, realm::object_id);
    __cpprealm_build_query(!=, not_equal, realm::object_id);
    __cpprealm_build_optional_query(==, equal, realm::object_id);
    __cpprealm_build_optional_query(!=, not_equal, realm::object_id);
}
