#include <cpprealm/schema.hpp>
#include <mutex>

namespace realm {
namespace schemagen {

    std::mutex schema_mtx;

    std::vector<internal::bridge::object_schema> registered_schemas(const std::optional<internal::bridge::object_schema>& schema) {
        static std::vector<internal::bridge::object_schema> schemas;
        std::lock_guard<std::mutex> lock(schema_mtx);
        if (schema) {
            auto it = std::find(std::begin(schemas), std::end(schemas), *schema);
            if (it == std::end(schemas))
                schemas.push_back(*schema);
        }
        return schemas;
    }

} // namespace schemagen
} // namespace realm
