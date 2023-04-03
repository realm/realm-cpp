#include "coffee_manager.hpp"
#include <iostream>

CoffeeMachineManager::CoffeeMachineManager(QObject *parent)
    : QObject{parent}
{
    auto app = realm::App("MY_APP_ID");
    mUser = app.login(realm::App::credentials::anonymous()).get_future().get();
    auto realm = realm::open<CoffeeMachine, DrinkTemplate>(mUser.flexible_sync_configuration());
    realm.subscriptions().update([](realm::mutable_sync_subscription_set& subs) {
        if (!subs.find("all")) {
            subs.add<CoffeeMachine>("all");
            subs.add<DrinkTemplate>("all_drinks");
        }
    }).get_future().get();
    realm.get_sync_session()->wait_for_download_completion().get_future().get();
    realm.refresh();
    auto coffeeMachines = realm.objects<CoffeeMachine>();
    if (coffeeMachines.size() == 0) {
        realm.write([&]() {
            auto espresso = DrinkTemplate();
            espresso._id = realm::object_id::generate();
            espresso.name = "Espresso";
            espresso.chocolateQty = 0;
            espresso.milkQty = 0;
            espresso.espressoQty = 5;
            realm.add(espresso);

            auto flatWhite = DrinkTemplate();
            flatWhite._id = realm::object_id::generate();
            flatWhite.name = "Flat White";
            flatWhite.chocolateQty = 0;
            flatWhite.milkQty = 3;
            flatWhite.espressoQty = 3;
            realm.add(flatWhite);

            auto cappucino = DrinkTemplate();
            cappucino._id = realm::object_id::generate();
            cappucino.name = "Cappucino";
            cappucino.chocolateQty = 0;
            cappucino.milkQty = 5;
            cappucino.espressoQty = 3;
            realm.add(cappucino);

            auto machine = CoffeeMachine();
            machine._id = realm::object_id::generate();
            machine.location = "1st floor kitchen";
            machine.ownerId = realm::object_id::generate();
            machine.chocolateQty = 100;
            machine.espressoQty = 100;
            machine.milkQty = 100;
            machine.sugarQty = 100;
            machine.state = CoffeeMachine::State::OK;
            machine.availableDrinks.push_back(espresso);
            machine.availableDrinks.push_back(flatWhite);
            machine.availableDrinks.push_back(cappucino);

            realm.add(machine);
        });
    }
    realm.get_sync_session()->wait_for_upload_completion().get_future().get();

    mCoffeeMachine = coffeeMachines[0];

    mToken = mCoffeeMachine.observe([&](realm::ObjectChange<CoffeeMachine>&& change) {
        for (auto& change : change.property_changes) {
            if (change.name == "state") {
                auto oldVal = std::get<CoffeeMachine::State>(change.old_value.value());
                auto newVal = std::get<CoffeeMachine::State>(change.new_value.value());

                if (oldVal == CoffeeMachine::State::NEEDS_ATTENTION && newVal == CoffeeMachine::State::OK)
                {
                    emit enableMachine();
                }
                else if (oldVal == CoffeeMachine::State::OK && newVal == CoffeeMachine::State::NEEDS_ATTENTION)
                {
                    emit disableMachine();
                }
                break;
            }
        }
    });

    mDrinksTableModel.reset(new DrinkSelectionModel(mCoffeeMachine));
}

void CoffeeMachineManager::prepareForBrew(const QString &)
{
    // NOOP, but can be used for somthing useful like analytics.
}

void CoffeeMachineManager::startBrew(const QString&, int64_t milkQty, int64_t espressoQty, int64_t sugarQty)
{
    auto realm = realm::open<CoffeeMachine, DrinkTemplate>(mUser.flexible_sync_configuration());
    realm.write([&]() {
        mCoffeeMachine.espressoQty -= espressoQty;
        mCoffeeMachine.milkQty -= milkQty;
        mCoffeeMachine.sugarQty -= sugarQty;
    });
}

DrinkSelectionModel::DrinkSelectionModel(CoffeeMachine& machine) : mMachine(machine)
{
    mToken = mMachine.availableDrinks.observe([&](auto&&) {
        this->beginResetModel();
        this->endResetModel();
    });
}


int DrinkSelectionModel::rowCount(const QModelIndex&) const
{
  return mMachine.availableDrinks.size();
}
int DrinkSelectionModel::columnCount(const QModelIndex&) const
{
  return 1;
}

QVariant DrinkSelectionModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (role == Role::Id)
        return QString::fromStdString((*mMachine.availableDrinks[index.row()]._id).to_string());
    if (role == Role::Name)
        return QString::fromStdString(*mMachine.availableDrinks[index.row()].name);
    if (role == Role::MilkQty)
        return *mMachine.availableDrinks[index.row()].milkQty;
    if (role == Role::EspressoQty)
        return *mMachine.availableDrinks[index.row()].espressoQty;
 return QVariant();
}

QHash<int, QByteArray> DrinkSelectionModel::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[Id] = "id";
        roles[Name] = "name";
        roles[MilkQty] = "milkQty";
        roles[EspressoQty] = "espressoQty";
        return roles;
}
