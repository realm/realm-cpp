#include <cpprealm/experimental/managed_primary_key.hpp>
#include <cpprealm/persisted.hpp>

namespace realm::experimental {

#define __cpprealm_build_experimental_pk_query(op, name, type, rhs_type) \
    rbool managed<primary_key<type>>::operator op(const rhs_type& rhs) const noexcept { \
        if (this->should_detect_usage_for_queries) { \
            auto query = internal::bridge::query(this->query->get_table()); \
            query.name(this->m_key, serialize(rhs)); \
            return query; \
        } \
        return serialize(value().value) op serialize(rhs); \
    } \

// Int needs to be cast to int64_t
#define __cpprealm_build_experimental_int_pk_query(op, name, type, rhs_type, cast) \
    rbool managed<primary_key<type>>::operator op(const rhs_type& rhs) const noexcept { \
        if (this->should_detect_usage_for_queries) { \
            auto query = internal::bridge::query(this->query->get_table()); \
            query.name(this->m_key, serialize((cast)rhs)); \
            return query; \
        } \
        return serialize(value().value) op serialize((cast)rhs); \
    }  \

    __cpprealm_build_experimental_pk_query(==, equal, int64_t, int64_t)
    __cpprealm_build_experimental_pk_query(!=, not_equal, int64_t, int64_t)
    __cpprealm_build_experimental_pk_query(>, greater, int64_t, int64_t)
    __cpprealm_build_experimental_pk_query(<, less, int64_t, int64_t)
    __cpprealm_build_experimental_pk_query(>=, greater_equal, int64_t, int64_t)
    __cpprealm_build_experimental_pk_query(<=, less_equal, int64_t, int64_t)

    __cpprealm_build_experimental_int_pk_query(==, equal, int64_t, int, int64_t)
    __cpprealm_build_experimental_int_pk_query(!=, not_equal, int64_t, int, int64_t)
    __cpprealm_build_experimental_int_pk_query(>, greater, int64_t, int, int64_t)
    __cpprealm_build_experimental_int_pk_query(<, less, int64_t, int, int64_t)
    __cpprealm_build_experimental_int_pk_query(>=, greater_equal, int64_t, int, int64_t)
    __cpprealm_build_experimental_int_pk_query(<=, less_equal, int64_t, int, int64_t)

   __cpprealm_build_experimental_pk_query(==, equal, std::string, std::string)
   __cpprealm_build_experimental_pk_query(!=, not_equal, std::string, std::string)

   __cpprealm_build_experimental_pk_query(==, equal, realm::uuid, realm::uuid)
   __cpprealm_build_experimental_pk_query(!=, not_equal, realm::uuid, realm::uuid)

   __cpprealm_build_experimental_pk_query(==, equal, realm::object_id, realm::object_id)
   __cpprealm_build_experimental_pk_query(!=, not_equal, realm::object_id, realm::object_id)

   rbool managed<primary_key<std::string>>::operator ==(const char* rhs) const noexcept {
        if (this->should_detect_usage_for_queries) {
            auto query = internal::bridge::query(this->query->get_table());
            query.equal(this->m_key, serialize(std::string(rhs)));
            return query;
        }
        return serialize(value().value) == serialize(std::string(rhs));
    }

    rbool managed<primary_key<std::string>>::operator !=(const char* rhs) const noexcept {
        if (this->should_detect_usage_for_queries) {
            auto query = internal::bridge::query(this->query->get_table());
            query.not_equal(this->m_key, serialize(std::string(rhs)));
            return query;
        }
        return serialize(value().value) != serialize(std::string(rhs));
    }

    rbool managed<primary_key<std::optional<std::string>>>::operator ==(const char* rhs) const noexcept {
        if (this->should_detect_usage_for_queries) {
            auto query = internal::bridge::query(this->query->get_table());
            query.equal(this->m_key, serialize(std::string(rhs)));
            return query;
        }
        return serialize(value().value) == serialize(std::string(rhs));
    }

    rbool managed<primary_key<std::optional<std::string>>>::operator !=(const char* rhs) const noexcept {
        if (this->should_detect_usage_for_queries) {
            auto query = internal::bridge::query(this->query->get_table());
            query.not_equal(this->m_key, serialize(std::string(rhs)));
            return query;
        }
        return serialize(value().value) != serialize(std::string(rhs));
    }

    // MARK: Optional

    __cpprealm_build_experimental_pk_query(>, greater, std::optional<int64_t>, int64_t)
    __cpprealm_build_experimental_pk_query(<, less, std::optional<int64_t>, int64_t)
    __cpprealm_build_experimental_pk_query(>=, greater_equal, std::optional<int64_t>, int64_t)
    __cpprealm_build_experimental_pk_query(<=, less_equal, std::optional<int64_t>, int64_t)

    __cpprealm_build_experimental_int_pk_query(>, greater, std::optional<int64_t>, int, int64_t)
    __cpprealm_build_experimental_int_pk_query(<, less, std::optional<int64_t>, int, int64_t)
    __cpprealm_build_experimental_int_pk_query(>=, greater_equal, std::optional<int64_t>, int, int64_t)
    __cpprealm_build_experimental_int_pk_query(<=, less_equal, std::optional<int64_t>, int, int64_t)

#define __cpprealm_build_optional_experimental_pk_query(op, name, type, rhs_type) \
    rbool managed<primary_key<std::optional<type>>>::operator op(const rhs_type& rhs) const noexcept { \
        if (this->should_detect_usage_for_queries) { \
            auto query = internal::bridge::query(this->query->get_table()); \
            if (auto r = serialize(rhs)) { \
                query.name(this->m_key, *r); \
            } else { \
                query.name(this->m_key, std::nullopt); \
            } \
            return query; \
        } \
        return serialize(value()) op serialize(rhs); \
    } \

    __cpprealm_build_optional_experimental_pk_query(==, equal, int64_t, std::optional<int64_t>)
    __cpprealm_build_optional_experimental_pk_query(!=, not_equal, int64_t, std::optional<int64_t>)


    __cpprealm_build_optional_experimental_pk_query(==, equal, std::string, std::optional<std::string>)
    __cpprealm_build_optional_experimental_pk_query(!=, not_equal, std::string, std::optional<std::string>)

    __cpprealm_build_optional_experimental_pk_query(==, equal, realm::uuid, std::optional<realm::uuid>)
    __cpprealm_build_optional_experimental_pk_query(!=, not_equal, realm::uuid, std::optional<realm::uuid>)

    __cpprealm_build_optional_experimental_pk_query(==, equal, realm::object_id, std::optional<realm::object_id>)
    __cpprealm_build_optional_experimental_pk_query(!=, not_equal, realm::object_id, std::optional<realm::object_id>)
}
