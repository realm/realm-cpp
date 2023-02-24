#include "barista.hpp"
#include <iostream>

Barista::Barista(QObject *parent)
    : QObject{parent}, m_myTableModel(new CoffeeSelectionModel)
{
    m_myTableModel->populate();
}

QString Barista::userName()
{
    return m_userName;
}

void Barista::setUserName(const QString &userName)
{
    if (userName == m_userName)
        return;

    m_userName = userName;
    emit userNameChanged();
}

void Barista::prepareForBrew(const QString &coffeeId)
{
    std::cout << coffeeId.toStdString() << "\n";
}

void Barista::startBrew(const QString &coffeeId, int64_t milkQty, int64_t espressoQty)
{
    std::cout << coffeeId.toStdString() << "\n";
}

CoffeeSelectionModel::CoffeeSelectionModel(QObject *)
{

}


int CoffeeSelectionModel::rowCount(const QModelIndex&) const
{
  return mCoffeeSelection.size();
}
int CoffeeSelectionModel::columnCount(const QModelIndex&) const
{
  return 1;
}

QVariant CoffeeSelectionModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (role == Role::Id)
        return mCoffeeSelection[index.row()].id;
    if (role == Role::Name)
        return mCoffeeSelection[index.row()].name;
    if (role == Role::MilkQty)
        return mCoffeeSelection[index.row()].milkQty;
    if (role == Role::EspressoQty)
        return mCoffeeSelection[index.row()].coffeeQty;
 return QVariant();
}

void CoffeeSelectionModel::populate()
 {
       beginResetModel();
       mCoffeeSelection.clear();
       mCoffeeSelection.append(Coffee("1", "Espresso", 0, 5));
       mCoffeeSelection.append(Coffee("1", "Flat White", 3, 5));
       mCoffeeSelection.append(Coffee("1", "Cappucino", 4, 5));
       mCoffeeSelection.append(Coffee("1", "Latte", 2, 5));
       mCoffeeSelection.append(Coffee("1", "Mocha", 2, 5));

       endResetModel();
 }

QHash<int, QByteArray> CoffeeSelectionModel::roleNames() const {
        QHash<int, QByteArray> roles;
        roles[Id] = "id";
        roles[Name] = "name";
        roles[MilkQty] = "milkQty";
        roles[EspressoQty] = "espressoQty";
        return roles;
}
