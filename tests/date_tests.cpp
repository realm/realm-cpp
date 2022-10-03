#include "test_objects.hpp"
#include "test_utils.hpp"

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

TEST(unmanaged_managed_ts_time_since_opech) {
    auto realm = open<AllTypesObject, AllTypesObjectLink, AllTypesObjectEmbedded>({.path=path});
    auto ts = std::chrono::time_point<std::chrono::system_clock>();
    auto object = AllTypesObject({.date_col=ts});
    CHECK_EQUALS(object.date_col.time_since_epoch(), ts.time_since_epoch());
    realm.write([&]{
        realm.add(object);
    });
    CHECK_EQUALS(object.date_col.time_since_epoch(), ts.time_since_epoch());
}