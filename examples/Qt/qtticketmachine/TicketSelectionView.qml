import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

Rectangle{
	visible: true
	anchors.fill: parent

	Rectangle {
		anchors.fill: parent
		color: "#7a0101"
	}

	Rectangle {
		id: requestHelpFooter

		height: parent.height * 0.08
		width: parent.width
		anchors.bottom: parent.bottom

		Rectangle {
			anchors.fill: parent
			color: "#7a0101"
			Text {
				id: requestHelpButtonText
				anchors.centerIn: parent
				text: "Can't decide? Press here to get support!"
				color: getHelp.containsMouse? "#ccb6b6": "#ffffff"
				font.pixelSize: 24
			}

			MouseArea {
				id: getHelp
				anchors.fill: parent
				hoverEnabled: true
				onClicked: {
					appModel.isInstantSupportRequested = true;
					if(appModel.instantSupportResponseCode !== appModel.Busy){
						appModel.requestInstantSupport(
									appModel.instantSupportAccessToken,
									appModel.instantSupportName,
									appModel.instantSupportGroup,
									appModel.instantSupportDescription,
									appModel.instantSupportSessionCode,
									appModel.instantSupportEmail)
					}
				}
			}
		}
	}

	Rectangle {
		id: ticketSelectionWindow

		anchors.fill: parent
		anchors.bottomMargin: requestHelpFooter.height
		radius: 12
		color: "#fafafa"

		Item {
			anchors.top: parent.top
			anchors.left: parent.left
			anchors.right: parent.right
			height: parent.height * 0.85
			anchors.margins: 25

			GridLayout {
				columns: 2
				width: parent.width
				height: parent.height
				columnSpacing: 20
				rowSpacing: 20

				Rectangle {
					Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
					Layout.fillWidth: true
					Layout.fillHeight: true
					Layout.preferredWidth: parent.width / 2
					Layout.preferredHeight: parent.height
					color: button1MouseArea.containsMouse ? "gray" : "lightgray"
					radius: 5

					Text {
						text: "1 Zone Single Fare"

						anchors.left: ticketIcon.right
						anchors.verticalCenter: ticketIcon.verticalCenter
						anchors.margins: 7

						width: parent.width - (ticketIcon.width + parent.width / 12) - 10
						Layout.maximumWidth: parent.width - (ticketIcon.width + parent.width / 12) - 10
						font.pixelSize: Math.max(parent.width, parent.height * 2.5) / 20
						wrapMode: Text.WordWrap
					}

					Image {
						id: ticketIcon
						source: "images/ticket_icon.png"
						anchors.topMargin: 1 - parent.width/20 + parent.height / 4
						anchors.bottomMargin: 1 - parent.width/20 + parent.height / 4
						anchors.leftMargin: parent.width / 20

						width: parent.height/2 + parent.width/20
						height: parent.height
						anchors.bottom: parent.bottom
						anchors.top: parent.top
						anchors.left: parent.left
					}

					MouseArea {
						id: button1MouseArea
						anchors.fill: parent
						onClicked: {
							viewLoader.source = "PaymentProcessingView.qml"
						}
					}
				}

				Rectangle {
					Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
					Layout.fillWidth: true
					Layout.fillHeight: true
					Layout.preferredWidth: parent.width / 2
					Layout.preferredHeight: parent.height
					color: button2MouseArea.containsMouse ? "gray" : "lightgray"
					radius: 5

					Text {
						text: "2 Zones Single Fare"

						anchors.left: ticketIcon2.right
						anchors.verticalCenter: ticketIcon2.verticalCenter
						anchors.margins: 7

						width: parent.width - (ticketIcon2.width + parent.width / 12) - 10
						Layout.maximumWidth: parent.width - (ticketIcon2.width + parent.width / 12) - 10
						font.pixelSize: Math.max(parent.width, parent.height * 2.5) / 20
						wrapMode: Text.WordWrap
					}

					Image {
						id: ticketIcon2
						source: "images/ticket_icon.png"

						anchors.topMargin: 1 - parent.width/20 + parent.height / 4
						anchors.bottomMargin: 1 - parent.width/20 + parent.height / 4
						anchors.leftMargin: parent.width / 20

						width: parent.height / 2 + parent.width/20
						anchors.bottom: parent.bottom
						anchors.top: parent.top
						anchors.left: parent.left
					}

					MouseArea {
						id: button2MouseArea
						anchors.fill: parent
						onClicked: {
							viewLoader.source = "PaymentProcessingView.qml"
						}
					}
				}

				Rectangle {
					Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
					Layout.fillWidth: true
					Layout.fillHeight: true
					Layout.preferredWidth: parent.width / 2
					Layout.preferredHeight: parent.height
					color: button3MouseArea.containsMouse ? "gray" : "lightgray"
					radius: 5

					Text {
						text: "3 Zones Single Fare"

						anchors.left: ticketIcon3.right
						anchors.verticalCenter: ticketIcon3.verticalCenter
						anchors.margins: 7

						width: parent.width - (ticketIcon3.width + parent.width / 12) - 10
						Layout.maximumWidth: parent.width - (ticketIcon3.width + parent.width / 12) - 10
						font.pixelSize: Math.max(parent.width, parent.height * 2.5) / 20
						wrapMode: Text.WordWrap
					}

					Image {
						id: ticketIcon3
						source: "images/ticket_icon.png"
						anchors.topMargin: 1 - parent.width/20 + parent.height / 4
						anchors.bottomMargin: 1 - parent.width/20 + parent.height / 4
						anchors.leftMargin: parent.width / 20

						width: parent.height/2 + parent.width/20
						height: parent.height
						anchors.bottom: parent.bottom
						anchors.top: parent.top
						anchors.left: parent.left
					}

					MouseArea {
						id: button3MouseArea
						anchors.fill: parent
						onClicked: {
							viewLoader.source = "PaymentProcessingView.qml"
						}
					}
				}

				Rectangle {
					Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
					Layout.fillWidth: true
					Layout.fillHeight: true
					Layout.preferredWidth: parent.width / 2
					Layout.preferredHeight: parent.height
					color: button4MouseArea.containsMouse ? "gray" : "lightgray"
					radius: 5

					Text {
						text: "All Zones Single Fare"

						anchors.left: ticketIcon4.right
						anchors.verticalCenter: ticketIcon4.verticalCenter
						anchors.margins: 7

						width: parent.width - (ticketIcon4.width + parent.width / 12) - 10
						Layout.maximumWidth: parent.width - (ticketIcon4.width + parent.width /12) - 10
						font.pixelSize: Math.max(parent.width, parent.height * 2.5) / 20
						wrapMode: Text.WordWrap
					}

					Image {
						id: ticketIcon4
						source: "images/ticket_icon.png"
						anchors.topMargin: 1 - parent.width/20 + parent.height / 4
						anchors.bottomMargin: 1 - parent.width/20 + parent.height / 4
						anchors.leftMargin: parent.width / 20

						width: parent.height/2 + parent.width/20
						height: parent.height
						anchors.bottom: parent.bottom
						anchors.top: parent.top
						anchors.left: parent.left
					}

					MouseArea {
						id: button4MouseArea
						anchors.fill: parent
						onClicked: {
							viewLoader.source = "PaymentProcessingView.qml"
						}
					}
				}

				Rectangle {
					id: dayTicketAllZones

					Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
					Layout.fillWidth: true
					Layout.fillHeight: true
					Layout.preferredWidth: parent.width / 2
					Layout.preferredHeight: parent.height
					color: button5MouseArea.containsMouse ? "gray" : "lightgray"
					radius: 5

					Text {
						text: "Day Ticket All Zones"

						anchors.left: ticketsIcon.right
						anchors.verticalCenter: ticketsIcon.verticalCenter
						anchors.margins: 10

						width: parent.width - (ticketsIcon.width + parent.width / 12) - 10
						Layout.maximumWidth: parent.width - (ticketsIcon.width + parent.width/12) - 10

						font.pixelSize: Math.max(parent.width, parent.height * 2.5) / 20
						wrapMode: Text.WordWrap
					}

					Image {
						id: ticketsIcon
						source: "images/ticket_icon.png"
						anchors.topMargin: 1 - parent.width/20 + parent.height / 4
						anchors.bottomMargin: 1 - parent.width/20 + parent.height / 4
						anchors.leftMargin: parent.width / 20

						width: parent.height/2 + parent.width/20
						height: parent.height
						anchors.bottom: parent.bottom
						anchors.top: parent.top
						anchors.left: parent.left
					}


					MouseArea {
						id: button5MouseArea
						anchors.fill: parent
						onClicked: {
							viewLoader.source = "PaymentProcessingView.qml"
						}
					}
				}

				Rectangle {
					id: monthlyTicketAllZones

					Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
					Layout.fillWidth: true
					Layout.fillHeight: true
					Layout.preferredWidth: parent.width / 2
					Layout.preferredHeight: parent.height
					color: button6MouseArea.containsMouse ? "gray" : "lightgray"
					radius: 5

					Text {
						text: "Monthly Ticket All Zones"

						anchors.left: ticketsIcon2.right
						anchors.verticalCenter: ticketsIcon2.verticalCenter
						anchors.margins: 7

						width: parent.width - (ticketsIcon2.width + parent.width / 12) - 10
						Layout.maximumWidth: parent.width - (ticketsIcon2.width + parent.width / 12) - 10
						font.pixelSize: Math.max(parent.width, parent.height * 2.5) / 20
						wrapMode: Text.WordWrap
					}

					Image {
						id: ticketsIcon2
						source: "images/ticket_icon.png"
						anchors.topMargin: 1 - parent.width/20 + parent.height / 4
						anchors.bottomMargin: 1 - parent.width/20 + parent.height / 4
						anchors.leftMargin: parent.width / 20

						width: parent.height/2 + parent.width/20
						height: parent.height
						anchors.bottom: parent.bottom
						anchors.top: parent.top
						anchors.left: parent.left
					}

					MouseArea {
						id: button6MouseArea
						anchors.fill: parent
						onClicked: {
							viewLoader.source = "PaymentProcessingView.qml"
						}
					}
				}
			}
		}
	}
}
