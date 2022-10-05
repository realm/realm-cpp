#include "test_objects.hpp"
#include "main.hpp"

template <typename T, typename V>
std::ostream& operator<< (std::ostream& stream, const std::chrono::duration<T, V>& value)
{
    return stream << std::chrono::duration_cast<std::chrono::seconds>(value).count();
}
template <typename T, typename V>
std::ostream& operator<< (std::ostream& stream, std::chrono::duration<T, V>&& value)
{
    return stream << std::chrono::duration_cast<std::chrono::seconds>(value).count();
}

using namespace realm;

TEST_CASE("date", "[date]") {
    realm_path path;
    SECTION("unmanaged_managed_ts_time_since_epoch", "[date]") {
        auto realm = open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>(
                {.path=path});
        auto ts = std::chrono::time_point<std::chrono::system_clock>();
        auto object = AllTypesObject({.date_col=ts});
        CHECK(object.date_col.time_since_epoch() == ts.time_since_epoch());
        realm.write([&] {
            realm.add(object);
        });
        CHECK(object.date_col.time_since_epoch() == ts.time_since_epoch());
    }
    SECTION("add") {
        auto realm = realm::open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>({.path=path});
        auto obj = AllTypesObject();
        CHECK(obj.date_col == std::chrono::time_point<std::chrono::system_clock>{});
        auto now = std::chrono::system_clock::now();
        obj.date_col = now;
        CHECK(obj.date_col == now);
        realm.write([&realm, &obj] {
            realm.add(obj);
        });
        CHECK(obj.date_col == now);
        realm.write([&obj] {
            obj.date_col += std::chrono::seconds(42);
        });
        CHECK(obj.date_col == now + std::chrono::seconds(42));
    }
}