#ifndef BARISTA_HPP
#define BARISTA_HPP

#include <qqml.h>
#include <QAbstractTableModel>
#include <QItemSelectionModel>
#include <QModelIndex>
#include <QObject>
#include <QString>

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

class DrinkSelectionModel : public QAbstractTableModel
{
  Q_OBJECT
 public :
    enum Role
    {
        Id = Qt::UserRole + 1,
        Name,
        MilkQty,
        EspressoQty
    };
    DrinkSelectionModel(CoffeeMachineModel& machine);
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    Q_INVOKABLE QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
 private:
   CoffeeMachineModel& mMachine;
   realm::notification_token mToken;
};


class CoffeeMachineManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(NOTIFY enableMachine NOTIFY disableMachine)
    Q_PROPERTY(QAbstractTableModel* drinkSelectionModel READ drinkSelectionModel CONSTANT)

    QML_ELEMENT

public:
    explicit CoffeeMachineManager(QObject *parent = nullptr);

    Q_INVOKABLE void prepareForBrew(const QString &coffeeId);
    Q_INVOKABLE void startBrew(const QString &coffeeId, int64_t milkQty, int64_t espressoQty, int64_t sugarQty);

    DrinkSelectionModel *drinkSelectionModel() { return mDrinksTableModel.data(); }

signals:
    void enableMachine();
    void disableMachine();

private:
    realm::user mUser;
    CoffeeMachineModel mCoffeeMachine;
    QScopedPointer<DrinkSelectionModel> mDrinksTableModel;
    realm::notification_token mToken;
};

#endif // BARISTA_HPP
