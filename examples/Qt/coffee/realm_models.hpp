#ifndef REALM_MODELS_HPP
#define REALM_MODELS_HPP

#include <cpprealm/sdk.hpp>
#include <cpprealm/sdk.hpp>

namespace realm {
    struct DrinkTemplate {
        primary_key<realm::object_id> _id;
        std::string name;
        int64_t milkQty;
        int64_t espressoQty;
        int64_t chocolateQty;
        int64_t sugarQty;
    };

    REALM_SCHEMA(DrinkTemplate,
                 _id,
                 name,
                 milkQty,
                 espressoQty,
                 chocolateQty,
                 sugarQty)

    struct CoffeeMachine {
        enum class State {
            NEEDS_ATTENTION,
            OK,
            MAINTENANCE_MODE
        };
        primary_key<realm::object_id> _id;
        std::string location;
        realm::object_id ownerId;
        int64_t milkQty;
        int64_t espressoQty;
        int64_t chocolateQty;
        int64_t sugarQty;
        State state;
        std::vector<DrinkTemplate *> availableDrinks;
    };

    REALM_SCHEMA(CoffeeMachine,
                 _id,
                 location,
                 ownerId,
                 milkQty,
                 espressoQty,
                 chocolateQty,
                 sugarQty,
                 state,
                 availableDrinks)
}

#endif // REALM_MODELS_HPP
