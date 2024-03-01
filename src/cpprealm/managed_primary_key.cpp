#include <cpprealm/managed_primary_key.hpp>

namespace realm {

#define __cpprealm_build_pk_query(op, name, type, rhs_type) \
    rbool managed<primary_key<type>>::operator op(const rhs_type& rhs) const noexcept { \
        if (this->rbool_query) { \
            return this->rbool_query->name(m_key, rhs); \
        } \
        return serialize(detach().value) op serialize(rhs); \
    } \

// Int needs to be cast to int64_t
#define __cpprealm_build_int_pk_query(op, name, type, rhs_type, cast) \
    rbool managed<primary_key<type>>::operator op(const rhs_type& rhs) const noexcept { \
        if (this->rbool_query) { \
            return this->rbool_query->name(m_key, serialize((cast)rhs)); \
        } \
        return serialize(detach().value) op serialize((cast)rhs); \
    }  \

    __cpprealm_build_pk_query(==, equal, int64_t, int64_t)
    __cpprealm_build_pk_query(!=, not_equal, int64_t, int64_t)
    __cpprealm_build_pk_query(>, greater, int64_t, int64_t)
    __cpprealm_build_pk_query(<, less, int64_t, int64_t)
    __cpprealm_build_pk_query(>=, greater_equal, int64_t, int64_t)
    __cpprealm_build_pk_query(<=, less_equal, int64_t, int64_t)

    __cpprealm_build_int_pk_query(==, equal, int64_t, int, int64_t)
    __cpprealm_build_int_pk_query(!=, not_equal, int64_t, int, int64_t)
    __cpprealm_build_int_pk_query(>, greater, int64_t, int, int64_t)
    __cpprealm_build_int_pk_query(<, less, int64_t, int, int64_t)
    __cpprealm_build_int_pk_query(>=, greater_equal, int64_t, int, int64_t)
    __cpprealm_build_int_pk_query(<=, less_equal, int64_t, int, int64_t)

    __cpprealm_build_pk_query(==, equal, std::string, std::string)
    __cpprealm_build_pk_query(!=, not_equal, std::string, std::string)

    __cpprealm_build_pk_query(==, equal, realm::uuid, realm::uuid)
    __cpprealm_build_pk_query(!=, not_equal, realm::uuid, realm::uuid)

    __cpprealm_build_pk_query(==, equal, realm::object_id, realm::object_id)
    __cpprealm_build_pk_query(!=, not_equal, realm::object_id, realm::object_id)

   rbool managed<primary_key<std::string>>::operator ==(const char* rhs) const noexcept {
        if (this->rbool_query) {
            return this->rbool_query->equal(m_key, std::string(rhs));
        }
        return serialize(detach().value) == serialize(std::string(rhs));
    }

    rbool managed<primary_key<std::string>>::operator !=(const char* rhs) const noexcept {
        if (this->rbool_query) {
            return this->rbool_query->not_equal(m_key, std::string(rhs));
        }
        return serialize(detach().value) != serialize(std::string(rhs));
    }

    rbool managed<primary_key<std::optional<std::string>>>::operator ==(const char* rhs) const noexcept {
        if (this->rbool_query) {
            return this->rbool_query->equal(m_key, std::string(rhs));
        }
        return serialize(detach().value) == serialize(std::string(rhs));
    }

    rbool managed<primary_key<std::optional<std::string>>>::operator !=(const char* rhs) const noexcept {
        if (this->rbool_query) {
            return this->rbool_query->not_equal(m_key, std::string(rhs));
        }
        return serialize(detach().value) != serialize(std::string(rhs));
    }

    // MARK: Optional

    __cpprealm_build_pk_query(>, greater, std::optional<int64_t>, int64_t)
    __cpprealm_build_pk_query(<, less, std::optional<int64_t>, int64_t)
    __cpprealm_build_pk_query(>=, greater_equal, std::optional<int64_t>, int64_t)
    __cpprealm_build_pk_query(<=, less_equal, std::optional<int64_t>, int64_t)

    __cpprealm_build_int_pk_query(>, greater, std::optional<int64_t>, int, int64_t)
    __cpprealm_build_int_pk_query(<, less, std::optional<int64_t>, int, int64_t)
    __cpprealm_build_int_pk_query(>=, greater_equal, std::optional<int64_t>, int, int64_t)
    __cpprealm_build_int_pk_query(<=, less_equal, std::optional<int64_t>, int, int64_t)

#define __cpprealm_build_optional_pk_query(op, name, type, rhs_type) \
    rbool managed<primary_key<std::optional<type>>>::operator op(const rhs_type& rhs) const noexcept { \
        if (this->rbool_query) {                 \
            return this->rbool_query->name(m_key, rhs);  \
        } \
        return serialize(detach()) op serialize(rhs); \
    } \

    __cpprealm_build_optional_pk_query(==, equal, int64_t, std::optional<int64_t>)
    __cpprealm_build_optional_pk_query(!=, not_equal, int64_t, std::optional<int64_t>)


    __cpprealm_build_optional_pk_query(==, equal, std::string, std::optional<std::string>)
    __cpprealm_build_optional_pk_query(!=, not_equal, std::string, std::optional<std::string>)

    __cpprealm_build_optional_pk_query(==, equal, realm::uuid, std::optional<realm::uuid>)
    __cpprealm_build_optional_pk_query(!=, not_equal, realm::uuid, std::optional<realm::uuid>)

    __cpprealm_build_optional_pk_query(==, equal, realm::object_id, std::optional<realm::object_id>)
    __cpprealm_build_optional_pk_query(!=, not_equal, realm::object_id, std::optional<realm::object_id>)
}
