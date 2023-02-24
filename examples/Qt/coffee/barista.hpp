#ifndef BARISTA_HPP
#define BARISTA_HPP

#include <QObject>
#include <QString>
#include <qqml.h>
#include <QItemSelectionModel>

class Coffee
{
 public :
    Coffee(const QString& id, const QString& name, uint64_t milkQty, uint64_t coffeeQty) :
        id(id), name(name), milkQty(milkQty), coffeeQty(coffeeQty) { }
    QString id;
    QString name;
    uint64_t milkQty;
    uint64_t coffeeQty;
};

#include <QAbstractTableModel>
#include <QModelIndex>

class CoffeeSelectionModel : public QAbstractTableModel
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
    CoffeeSelectionModel(QObject * parent = 0);
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    Q_INVOKABLE QVariant data(const QModelIndex &index, int role) const override;
    void populate();
    QHash<int, QByteArray> roleNames() const override;
 private:
   QList<Coffee> mCoffeeSelection;
};


class Barista : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged)
    Q_PROPERTY(QAbstractTableModel* myTableModel READ myTableModel CONSTANT)

    QML_ELEMENT

public:
    explicit Barista(QObject *parent = nullptr);


    QString userName();
    void setUserName(const QString &userName);
    Q_INVOKABLE void prepareForBrew(const QString &coffeeId);
    Q_INVOKABLE void startBrew(const QString &coffeeId, int64_t milkQty, int64_t espressoQty);

    CoffeeSelectionModel *myTableModel() { return m_myTableModel.data(); }

signals:
    void userNameChanged();

private:
    QString m_userName;
    QScopedPointer<CoffeeSelectionModel> m_myTableModel;

};

#endif // BARISTA_HPP
