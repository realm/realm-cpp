#ifndef REALM_MODELS_HPP
#define REALM_MODELS_HPP

#include <cpprealm/sdk.hpp>

class DrinkTemplate : public realm::object<DrinkTemplate> {
public:
    realm::persisted<realm::object_id> _id;
    realm::persisted<std::string> name;
    realm::persisted<int64_t> milkQty;
    realm::persisted<int64_t> espressoQty;
    realm::persisted<int64_t> chocolateQty;
    realm::persisted<int64_t> sugarQty;

    static constexpr auto schema = realm::schema("DrinkTemplate",
        realm::property<&DrinkTemplate::_id, true>("_id"),
        realm::property<&DrinkTemplate::name>("name"),
        realm::property<&DrinkTemplate::milkQty>("milkQty"),
        realm::property<&DrinkTemplate::espressoQty>("espressoQty"),
        realm::property<&DrinkTemplate::chocolateQty>("chocolateQty"),
        realm::property<&DrinkTemplate::sugarQty>("sugarQty")
    );
};

class CoffeeMachine : public realm::object<CoffeeMachine> {
public:
    enum class State {
        OK,
        NEEDS_ATTENTION
    };
    realm::persisted<realm::object_id> _id;
    realm::persisted<std::string> location;
    realm::persisted<realm::object_id> ownerId;
    realm::persisted<int64_t> milkQty;
    realm::persisted<int64_t> espressoQty;
    realm::persisted<int64_t> chocolateQty;
    realm::persisted<int64_t> sugarQty;
    realm::persisted<State> state;
    realm::persisted<std::vector<DrinkTemplate>> availableDrinks;

    static constexpr auto schema = realm::schema("CoffeeMachine",
        realm::property<&CoffeeMachine::_id, true>("_id"),
        realm::property<&CoffeeMachine::location>("location"),
        realm::property<&CoffeeMachine::ownerId>("ownerId"),
        realm::property<&CoffeeMachine::milkQty>("milkQty"),
        realm::property<&CoffeeMachine::espressoQty>("espressoQty"),
        realm::property<&CoffeeMachine::chocolateQty>("chocolateQty"),
        realm::property<&CoffeeMachine::sugarQty>("sugarQty"),
        realm::property<&CoffeeMachine::state>("state"),
        realm::property<&CoffeeMachine::availableDrinks>("availableDrinks")
    );
};

#endif // REALM_MODELS_HPP
