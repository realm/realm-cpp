// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Barista
import Coffee

ApplicationWindow {
    visible: true
    width: Constants.width
    height: Constants.height
    title: qsTr("Coffee")

    CoffeeMachineManager {
        id: backend
        onEnableMachine: {
            outOfOrderView.visible = false;
            coffeeView.visible = true;
        }
        onDisableMachine: {
            outOfOrderView.visible = true;
            coffeeView.visible = false;
        }
    }

    ApplicationFlow {
        id: coffeeView
    }

    Text {
        id: outOfOrderView
        visible: false
        text: "Out of order. Help is coming."
        font.family: "Helvetica"
        font.pointSize: 24
        color: "white"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
    }
}
