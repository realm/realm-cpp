#ifndef COFFEE_MANAGER_HPP
#define COFFEE_MANAGER_HPP

#include <qqml.h>
#include <QAbstractTableModel>
#include <QItemSelectionModel>
#include <QModelIndex>
#include <QObject>
#include <QString>

#include "realm_models.hpp"

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
    DrinkSelectionModel(realm::experimental::managed<realm::experimental::CoffeeMachine>& machine);
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    Q_INVOKABLE QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
 private:
   realm::experimental::managed<realm::experimental::CoffeeMachine> mMachine;
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
    realm::experimental::managed<realm::experimental::CoffeeMachine> mCoffeeMachine;
    QScopedPointer<DrinkSelectionModel> mDrinksTableModel;
    realm::notification_token mToken;
};

#endif // COFFEE_MANAGER_HPP
