#include <cpprealm/db.hpp>
#include <cpprealm/schema.hpp>

#include <realm/object-store/schema.hpp>
#include <realm/object-store/sync/async_open_task.hpp>
#include <realm/object-store/thread_safe_reference.hpp>

namespace realm::internal {
//    realm::Schema to_schema(const std::vector<realm::object_schema>& schemas)
//    {
//        std::vector<ObjectSchema> oss;
//        for (auto& s : schemas) {
//            ObjectSchema os;
//            os.name = s.name;
//            os.table_type = ObjectSchema::ObjectType(s.table_type);
//            std::transform(s.properties.begin(), s.properties.end(), os.persisted_properties.begin(),
//                           [](const realm::object_schema::property& p) {
//                if (!p.object_type.empty()) {
//                    return Property(p.name, PropertyType(p.type), p.object_type);
//                }
//                return Property(p.name, PropertyType(p.type), p.is_primary_key);
//            });
//            oss.push_back(os);
//        }
//        return {oss};
//    }

//    std::shared_ptr<Realm> get_shared_realm(const db_config& config, const std::vector<object_schema>& schemas)
//    {
//        return Realm::get_shared_realm({
//            .path = config.path + ".realm",
//            .schema_mode = SchemaMode::AdditiveExplicit,
//            .schema = to_schema(schemas),
//            .schema_version = 0,
//            .scheduler = scheduler(),
//            .sync_config = config.sync_config
//        });
//    }
//
//    void get_synchronized_realm(const db_config& config,
//                                const std::vector<object_schema>& schemas,
//                                std::function<void(std::unique_ptr<ThreadSafeReference>, std::exception_ptr)> f)
//    {
//        std::shared_ptr<AsyncOpenTask> async_open_task = Realm::get_synchronized_realm({
//            .path = config.path,
//            .schema_mode = SchemaMode::AdditiveExplicit,
//            .schema = to_schema(schemas),
//            .schema_version = 0,
//            .sync_config = config.sync_config
//        });
//
//        async_open_task->start([f = std::move(f)](ThreadSafeReference tsr, std::exception_ptr ex) {
//            f(std::make_unique<ThreadSafeReference>(std::move(tsr)), ex);
//        });
//    }
}