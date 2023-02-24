// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Coffee
import io.mongo.barista 1.0

ApplicationWindow {
    visible: true
    width: Constants.width
    height: Constants.height
    title: qsTr("Coffee")

    Barista {
            id: backend
            onUserNameChanged: {
                textField1.color = "red"
            }
        }

    ApplicationFlow {
    }
//    TextField {
//            id: textField1
//            text: backend.userName
//            placeholderText: qsTr("User name")
//            anchors.centerIn: parent
//            onEditingFinished: backend.userName = text
//        }
//    Button {
//        text: "Ok"
//        onClicked: backend.submit()
//    }
}
