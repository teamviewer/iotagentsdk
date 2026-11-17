import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.0
import Qt5Compat.GraphicalEffects

Popup {
    id: sosPopup

    x: 100
    y: 100
    width: 375
    height: 175
    modal: true
    focus: true
    anchors.centerIn: parent
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    visible: {
        appModel.isInstantSupportRequested && (appModel.instantSupportResponseCode === appModel.Success || appModel.instantSupportResponseCode === appModel.Busy)
        return appModel.isInstantSupportRequested && (appModel.instantSupportResponseCode === appModel.Success || appModel.instantSupportResponseCode === appModel.Busy);
    }

    contentItem: Rectangle{
        anchors.fill: parent

        RectangularGlow {
            z: -1
            anchors.fill: parent
            glowRadius: 20
            color: "#80000000"
            cached: false
        }

        Rectangle {
            id: supportedRequestedHeaderArea

            width: parent.width
            height: 40
            color: "#d80000"

            Rectangle{
                id: supportedRequestedHeader
                anchors.centerIn: parent
                height: parent.height

                Text {
                    id: supportedRequestedHeaderText

                    color: "#ffffff"
                    text: "Support Requested!"
                    font.pixelSize: 25
                    anchors.centerIn: parent
                }
            }
        }

        Text {
            id: supportConnectionConfirmationText

            anchors.centerIn: parent
            anchors.margins: 15
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            visible: appModel.isInstantSupportConfirmationRequested && appModel.isTVCommunicationRunning
            text: "Allow expert to connect?"
            color: "black"
            fontSizeMode: Text.Fit
            font.pixelSize: 24
            wrapMode: Text.WordWrap
        }

        Row {
            spacing: 8
            anchors.top: supportConnectionConfirmationText.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.margins: 15

            visible: appModel.isInstantSupportConfirmationRequested && appModel.isTVCommunicationRunning

            RoundButton {
                id: acceptInstantSupportRequest

                width: 100
                height: 30

                text: qsTr("Accept")

                RectangularGlow {
                    z: -1
                    anchors.fill: parent
                    glowRadius: 3
                    color: "#80000000"
                    cornerRadius: parent.radius + glowRadius
                    cached: true
                }

                onClicked: {
                    appModel.isInstantSupportRequested = false;
                    appModel.acceptInstantSupportConfirmationRequest();
                }
            }

            RoundButton {
                id: denyInstantSupportRequest

                width: 100
                height: 30

                RectangularGlow {
                    z: -1
                    anchors.fill: parent
                    glowRadius: 3
                    color: "#80000000"
                    cornerRadius: parent.radius + glowRadius
                    cached: true
                }

                text: qsTr("Deny")
                onClicked: {
                    appModel.isInstantSupportRequested = false;
                    appModel.denyInstantSupportConfirmationRequest();
                }
            }
        }

        Text {
            id: supportRequestText

            anchors.centerIn: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            visible: !appModel.isInstantSupportConfirmationRequested && appModel.isTVCommunicationRunning
            text: "Waiting on expert"
            color: "black"
            fontSizeMode: Text.Fit
            font.pixelSize: 24
            wrapMode: Text.WordWrap
        }

        BusyIndicator {
            visible: !appModel.isInstantSupportConfirmationRequested && appModel.isTVCommunicationRunning

            anchors.margins: 8
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: supportRequestText.bottom
        }
    }
}
