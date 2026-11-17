import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.0
import Qt5Compat.GraphicalEffects

Popup {
    id: welcomePopup
    x: 100
    y: 100
    width: 500
    height: 300
    modal: true
    focus: true
    anchors.centerIn: parent
    closePolicy: Popup.NoAutoClose

    //To get rid of the gap, use contentItem: Rectangle {}
    Rectangle{
        anchors.fill: parent

        RectangularGlow {
            z: -1
            anchors.fill: parent
            glowRadius: 20
            color: "#80000000"
            cached: false
        }

        Rectangle {
            id: welcomePopUpHeaderArea

            width: parent.width
            height: 40
            color: "#d80000"

            Rectangle{
                id: welcomePopUpHeader
                anchors.centerIn: parent
                height: parent.height

                Text {
                    id: welcomePopUpHeaderText

                    color: "#ffffff"
                    text: "Welcome to the Ticket Machine!"
                    font.pixelSize: 25
                    anchors.centerIn: parent
                }
            }
        }

    Text {
        id: instructionText

        anchors.verticalCenter: parent.verticalCenter
        anchors.margins: 10
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        text: '<html><style type="text/css"></style>Please retrieve input token for remote support functionality from <a href="https://web.teamviewer.com/settings/profile/account">web.teamviewer.com/settings/profile/account</a>  - "Token for embedded device". This can be changed later under Settings -> Instant Support</html>'
        onLinkActivated: Qt.openUrlExternally(link)

        color: "black"
        width: parent.width
        font.pixelSize: 20
        wrapMode: Text.WordWrap
    }

    TextField {
        id: token
        placeholderText: qsTr("Input token...")
        focus: true
        anchors.top: instructionText.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 8
        width: 250
        height: 20
        clip: true
        color: "gray"
        selectByMouse: true
        layer.enabled: true
        font.italic: true
    }

    Row {
        spacing: 5
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 8

        RoundButton {
            id: submitEmptyToken

            text: qsTr("Skip for now")

            width: 125
            RectangularGlow {
                z: -1
                anchors.fill: parent
                glowRadius: 3
                color: "#80000000"
                cornerRadius: parent.radius + glowRadius
                cached: true
            }

            onClicked: {
                appModel.instantSupportAccessToken = "";

                welcomePopup.close();
            }
        }

        RoundButton {
            id: submitToken

            text: qsTr("Submit")

            width: 125
            RectangularGlow {
                z: -1
                anchors.fill: parent
                glowRadius: 3
                color: "#80000000"
                cornerRadius: parent.radius + glowRadius
                cached: true
            }

            onClicked: {
                appModel.instantSupportAccessToken = token.text;
                appModel.instantSupportName = "Select Ticket Help Requested";
                appModel.instantSupportGroup = "InstantSupportGroup";
                appModel.instantSupportDescription = "Help me select a ticket!";
                welcomePopup.close();
            }
        }
    }
    }
}
