set(BRIDGE_TYPE_REQUIRED_HEADERS
    realm.hpp
    realm/object-store/shared_realm.hpp
    realm/object-store/dictionary.hpp
    realm/object-store/list.hpp
    realm/object-store/results.hpp
    realm/object-store/thread_safe_reference.hpp
    realm/object-store/sync/app.hpp
    realm/sync/config.hpp
    realm/sync/subscriptions.hpp
    realm/util/bson/bson.hpp
)

set(BRIDGE_TYPES
    OwnedBinaryData realm::OwnedBinaryData
    Realm_Config realm::Realm::Config
    ColKey realm::ColKey
    Decimal128 realm::Decimal128
    Dictionary realm::object_store::Dictionary
    CoreDictionary realm::Dictionary
    List realm::List
    LnkLst realm::LnkLst
    Mixed realm::Mixed
    ObjKey realm::ObjKey
    ObjLink realm::ObjLink
    Obj realm::Obj
    ObjectId realm::ObjectId
    ObjectSchema realm::ObjectSchema
    Object realm::Object
    IndexSet realm::IndexSet
    CollectionChangeSet realm::CollectionChangeSet
    IndexSet_IndexIterator realm::IndexSet::IndexIterator
    IndexSet_IndexIteratableAdaptor realm::IndexSet::IndexIteratableAdaptor
    NotificationToken realm::NotificationToken
    Property realm::Property
    Query realm::Query
    Results realm::Results
    Schema realm::Schema
    Set realm::object_store::Set
    SyncError realm::SyncError
    TableRef realm::TableRef
    TableView realm::TableView
    ThreadSafeReference realm::ThreadSafeReference
    Timestamp realm::Timestamp
    UUID realm::UUID
    Status realm::Status
    ErrorCategory realm::ErrorCategory
    AppCredentials realm::app::AppCredentials
    AppError realm::app::AppError
    SyncSubscriptionSet realm::sync::SubscriptionSet
    MutableSyncSubscriptionSet realm::sync::MutableSubscriptionSet
    Bson realm::bson::Bson
    BsonIndexedMap realm::bson::IndexedMap<realm::bson::Bson>
    BsonDocument realm::bson::BsonDocument
)

set(TYPE_INFO_QUERY_DECLS "")

while(BRIDGE_TYPES)
    list(POP_FRONT BRIDGE_TYPES _name _expression)
    string(REGEX MATCHALL "." _name_char_matches "${_name}")
    list(TRANSFORM _name_char_matches PREPEND ')
    list(TRANSFORM _name_char_matches APPEND ')
    list(JOIN _name_char_matches ", " _name_chars)
    set(TYPE_INFO_QUERY_DECLS "${TYPE_INFO_QUERY_DECLS}    REALM_TYPE_INFO(${_name}, ${_expression}, ${_name_chars});\n")
endwhile()

foreach(h ${BRIDGE_TYPE_REQUIRED_HEADERS})
string(APPEND BRIDGE_TYPE_HEADER_DECLS "#include \"${h}\"\n")
endforeach()

configure_file(bridge_type_info_generator.cpp.in bridge_type_info_generator.cpp)
add_library(BridgeTypeInfoGenerator STATIC ${CMAKE_CURRENT_BINARY_DIR}/bridge_type_info_generator.cpp)

if(MSVC)
    target_compile_options(BridgeTypeInfoGenerator PRIVATE /O0)
else()
    target_compile_options(BridgeTypeInfoGenerator PRIVATE -O0)
endif()

target_link_libraries(BridgeTypeInfoGenerator Realm::Storage)
