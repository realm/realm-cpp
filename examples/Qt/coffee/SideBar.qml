// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick



Flickable {
    id: sideBar
    property string currentCoffeeId
    property string currentCoffee: qsTr("Cappucino")
    signal coffeeSelected
    property real currentMilk: 0
    property real currentCoffeeAmount: 4

    Behavior on currentMilk {
         NumberAnimation { duration: 250 }
    }

    Behavior on currentCoffeeAmount {
         NumberAnimation { duration: 250 }
    }

    x: 0
    y: 0
    width: 354
    height: 768


    contentWidth: 354
    boundsBehavior: Flickable.StopAtBounds
    contentHeight: 1506
    clip: true

    Rectangle {
        id: background
        x: 0
        width: 354
        height: 1506
        color: "#eec1a2"
    }

    TableView {
        implicitWidth: 250
        height: 768

        model: backend.drinkSelectionModel
        anchors.horizontalCenter: parent.horizontalCenter

        delegate: CoffeeButton {
            required property var model
            required property string name
            required property int milkQty
            required property int espressoQty

            id: espressoButton
            text: model.name
//            source: "images/icons/coffees/Espresso.png"
            onClicked: {
                sideBar.currentCoffeeId = qsTr(model.id)
                sideBar.currentCoffee = qsTr(model.name)
                sideBar.currentMilk = model.milkQty
                sideBar.currentCoffeeAmount = model.espressoQty
                sideBar.coffeeSelected()
            }
        }
    }
}
