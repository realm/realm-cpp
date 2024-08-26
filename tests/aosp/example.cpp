#include "cpprealm/sdk.hpp"

// Verifies linkage in AOSP.
int main() {
    auto db = realm::db(realm::db_config());
}