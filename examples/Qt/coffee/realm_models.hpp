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

class CoffeeMachineModel : public realm::object<CoffeeMachineModel> {
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

    static constexpr auto schema = realm::schema("CoffeeMachineModel",
        realm::property<&CoffeeMachineModel::_id, true>("_id"),
        realm::property<&CoffeeMachineModel::location>("location"),
        realm::property<&CoffeeMachineModel::ownerId>("ownerId"),
        realm::property<&CoffeeMachineModel::milkQty>("milkQty"),
        realm::property<&CoffeeMachineModel::espressoQty>("espressoQty"),
        realm::property<&CoffeeMachineModel::chocolateQty>("chocolateQty"),
        realm::property<&CoffeeMachineModel::sugarQty>("sugarQty"),
        realm::property<&CoffeeMachineModel::state>("state"),
        realm::property<&CoffeeMachineModel::availableDrinks>("availableDrinks")
    );
};

#endif // REALM_MODELS_HPP
