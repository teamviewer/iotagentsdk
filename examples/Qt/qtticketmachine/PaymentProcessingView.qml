import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt5Compat.GraphicalEffects

Rectangle {
	color: "white"
	Column {
		anchors.centerIn: parent
		height: parent.height / 2
		width: parent.width
		spacing: 10

		Text {
			anchors.horizontalCenter: parent.horizontalCenter
			text: "Ticket price: 2,50€"
			font.pixelSize: 24
		}

		Text {
			anchors.horizontalCenter: parent.horizontalCenter
			text: "Please insert your card into the card reader"
			font.pixelSize: 24
		}

		AnimatedImage {
			anchors.horizontalCenter: parent.horizontalCenter
			source: "images/card_payment.gif"
			height: parent.height * 0.8
			fillMode: Image.PreserveAspectFit
		}

		RoundButton {
			visible: !appModel.isTVSessionRunning
			anchors.horizontalCenter: parent.horizontalCenter
			width: 650
			height: 30
			Text {
				text: "Problems processing your payment? Press here to request help"
				font.pixelSize: 20
				anchors.centerIn: parent
			}

			RectangularGlow {
				z: -1
				anchors.fill: parent
				glowRadius: 3
				color: "#80000000"
				cornerRadius: parent.radius + glowRadius
				cached: true
			}

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

		RoundButton {
			visible: appModel.isTVSessionRunning
			anchors.horizontalCenter: parent.horizontalCenter
			width: 750
			height: 30
			Text {
				anchors.centerIn: parent
				text: "Hardware problems with card reader? Press here to start an Augmented Session"
				font.pixelSize: 20
			}
			RectangularGlow {
				z: -1
				anchors.fill: parent
				glowRadius: 3
				color: "#80000000"
				cornerRadius: parent.radius + glowRadius
				cached: true
			}
			onClicked: {
				augmentRCPopUp.open()
				appModel.isListeningForAugmentRCSessionInvite = true
			}
		}
	}

	Popup {
		id: augmentRCPopUp
		x: 100
		y: 100
		width: 520
		height: 300
		modal: true
		focus: true
		anchors.centerIn: parent
		closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

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
			id: augmentRCSessionInvitationArea

			visible: appModel.isAugmentRCSessionFeatureAvailable

			anchors.top: parent.top
			anchors.bottom: parent.bottom
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.margins: 10
			Text {
				id: supportConnectionConfirmationText

				anchors.centerIn: parent
				horizontalAlignment: Text.AlignHCenter
				verticalAlignment: Text.AlignVCenter
				visible: augmentRCSessionUrl.url.length == 0
				text: "Let's have a look together! Ask your supporter to start an AugmentRC session"
				color: "black"

				width: parent.width
				font.pixelSize: 24
				wrapMode: Text.WordWrap
			}

			Text {
				id: augmentRCSessionUrlLabel
				text: "Augment RC Session URL:"
				font.bold: true
				color: "#0064c8"
				anchors.top: parent.top
				anchors.left: parent.left
				anchors.margins: 8
			}

			TextInput {
				id: augmentRCSessionUrl
				readOnly: true
				selectByMouse: true
				persistentSelection: true
				property string url
				text: url.length > 0 ? url :
					"Waiting for supporter to start AugmentRC session..."
				font.bold: true
				anchors.top: parent.top
				anchors.left: augmentRCSessionUrlLabel.right
				anchors.right: parent.right
				anchors.margins: 8

				Connections {
					target: appModel
					function onAugmentRCSessionInvitationReceived(url) {
						augmentRCSessionUrl.url = url
					}
				}
			}

			Image {
				id: augmentRCSessionQrCode
				visible: augmentRCSessionUrl.url.length > 0
				fillMode: Image.PreserveAspectFit
				smooth: false
				anchors.top: augmentRCSessionUrlLabel.bottom
				anchors.bottom: parent.bottom
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.margins: 15
				source: "image://qrcode/" + encodeURIComponent(augmentRCSessionUrl.url)
			}
		}
		}
	}
}
