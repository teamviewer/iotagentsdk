import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import "controls"
import "components"

Rectangle {
	id: settings_page
	anchors.fill: parent
	color: "lightgray"
		Row {
			anchors.fill: parent
			anchors.topMargin: 10
			anchors.leftMargin: 20
			spacing: 10

			Rectangle {
				id: tabArea

				width: 160
				height: parent.height - 200
				anchors.top: parent.top
				anchors.topMargin: 10

				color: "white"
				radius: 10
				border.color: "#E0E0E0"

				Column {
					spacing: 10
					padding: 10

					RoundButton {
						text: qsTr("Access Controls")
						palette.buttonText: tabLoader.sourceComponent == accessControlsTab ? "#d80000" : "black"
						background: Rectangle{color: "white"}
						onClicked: tabLoader.sourceComponent = accessControlsTab
					}
					RoundButton {
						text: qsTr("Instant Support")
						palette.buttonText: tabLoader.sourceComponent == instantSupportTab ? "#d80000" : "black"
						background: Rectangle{color: "white"}
						onClicked: tabLoader.sourceComponent = instantSupportTab
					}
					RoundButton {
						text: qsTr("Connection Status")
						palette.buttonText: tabLoader.sourceComponent == connectionStatusTab ? "#d80000" : "black"
						background: Rectangle{color: "white"}
						onClicked: tabLoader.sourceComponent = connectionStatusTab
					}
				}
			}

			Loader {
				id: tabLoader
				width: parent.width - 160
				height: parent.height - 30
				anchors.top: parent.top
				anchors.leftMargin: 10
				anchors.rightMargin: 10
				sourceComponent: accessControlsTab
			}
		}

		Component {
			id: accessControlsTab
			Column {
				padding: 10

				Rectangle {
					id: accessControlsSettingsPanel

					width: parent.width - 40
					height: parent.height
					color: "white"
					radius: 10
					border.color: "#E0E0E0"

					AccessControlPanel {
						id: fileTransferAccessControlArea

						anchors.top: parent.top
						anchors.left: parent.left
						anchors.right: parent.right
						anchors.margins: 8

						model: appModel
						enabled: appModel.isTVCommunicationRunning
						accessControlType: appModel.accessControl_FileTransfer
						title: "[File Transfer] access"
					}

					AccessControlPanel {
						id: accessControlAreaViewScreen

						anchors.top: fileTransferAccessControlArea.bottom
						anchors.left: parent.left
						anchors.right: parent.right
						anchors.margins: 8

						model: appModel
						enabled: appModel.isTVCommunicationRunning
						accessControlType: appModel.accessControl_RemoteView
						title: "[View Screen] access"
					}

					AccessControlPanel {
						id: accessControlAreaRemoteControl

						anchors.top: accessControlAreaViewScreen.bottom
						anchors.left: parent.left
						anchors.right: parent.right
						anchors.margins: 8

						model: appModel
						enabled: appModel.isTVCommunicationRunning
						accessControlType: appModel.accessControl_RemoteControl
						hasAfterConfirmation: false
						title: "[Remote Control] access"
					}

					AccessControlPanel {
						id: accessControlAreaScreenRecording

						anchors.top: accessControlAreaRemoteControl.bottom
						anchors.left: parent.left
						anchors.right: parent.right
						anchors.margins: 8

						model: appModel
						enabled: appModel.isTVCommunicationRunning
						accessControlType: appModel.accessControl_ScreenRecoring
						title: "[Screen Recording] access"
					}
				}
			}
		}

		Component {
			id: instantSupportTab
			Column {
				padding: 10

				Rectangle {
					id: sosSettingsPanel
					width: parent.width - 40
					height: parent.height
					color: "white"
					radius: 10
					border.color: "#E0E0E0"

					RectangularGlow {
						z: -1
						anchors.fill: parent
						glowRadius: 3
						color: "#80000000"
						cornerRadius: parent.radius + glowRadius
						cached: true
					}

					Text {
						id: instantSupportTitle
						anchors.top: parent.top
						anchors.left: parent.left
						anchors.margins: 20
						text: "Get Instant Support"
						font.bold: true
						font.pixelSize: 20
					}

					GridLayout {
						id: sosField

						height: parent.height - 120
						width: parent.width
						anchors.leftMargin: 20
						anchors.topMargin: 10
						anchors.left: parent.left
						anchors.top: instantSupportTitle.bottom
						flow: GridLayout.TopToBottom

						columns:2
						rows:8
						rowSpacing: 0
						columnSpacing: 2

							Label{
								id:sosInfoToken

								text: "Input token:"
								font.pixelSize: 16
							}
							TextField {
								id: sosInfoTokenBox
								text: appModel.instantSupportAccessToken === "" ? "" : appModel.instantSupportAccessToken

								width: 300
								implicitWidth: 285
								height: Math.max(implicitHeight, parent.height)
								implicitHeight: 30

								focus:true
								clip: true
								color: "gray"
								selectByMouse: true
								layer.enabled: true
								font.italic: true
								font.pixelSize: 16
							}

							Rectangle{
								width: 250
								height: 30
								border.color: "white"
							}

							Label{
								id:sosInfoName

								text: "Input name:"
								font.pixelSize: 16
							}
							TextField {
								id: sosInfoNameBox
								text: appModel.instantSupportName === "" ? "Select Ticket Help Requested" : appModel.instantSupportName;

								anchors.margins: 6
								width: 250
								implicitWidth: 285
								height: Math.max(implicitHeight, parent.height)
								implicitHeight: 30

								focus:true
								clip: true
								color: "gray"
								selectByMouse: true
								layer.enabled: true
								font.italic: true
								font.pixelSize: 16
							}

							Rectangle{
								width: 250
								height: 30
								border.color: "white"
							}

							Text{
								id:sosInfoEmail

								text: "Input email address:"
								font.pixelSize: 16
							}
							TextField {
								id: sosInfoEmailBox
								placeholderText: "[optional]"
								text: appModel.instantSupportEmail

								anchors.margins: 6
								width: 250
								implicitWidth: 285
								height: Math.max(implicitHeight, parent.height)
								implicitHeight: 30

								focus: true
								clip: true
								color: "gray"
								selectByMouse: true
								layer.enabled: true
								font.italic: true
								font.pixelSize: 16
							}

							Text{
								id:sosInfoDescription

								text: "Input description:"
								font.pixelSize: 16
							}
							TextField {
								id: sosInfoDescriptionBox
								text: appModel.instantSupportDescription === "" ? "Help me select a ticket!" : appModel.instantSupportDescription;

								anchors.margins: 6
								width: 300
								implicitWidth: 285
								height: Math.max(implicitHeight, parent.height)
								implicitHeight: 30

								focus: true
								clip: true
								color: "gray"
								selectByMouse: true
								layer.enabled: true
								font.italic: true
								font.pixelSize: 16
							}

							Rectangle{
								width: 250
								height: 30
								border.color: "white"
							}

							Label{
								id:sosInfoGroup

								text: "Input group:"
								font.pixelSize: 16
							}
							TextField {
								id: sosInfoGroupBox
								text: appModel.instantSupportGroup === undefined ? "InstantSupportGroup" : appModel.instantSupportGroup;

								anchors.margins: 6
								width: 250
								implicitWidth: 285
								height: Math.max(implicitHeight, parent.height)
								implicitHeight:30

								focus: true
								clip: true
								color: "gray"
								selectByMouse: true
								layer.enabled: true
								font.italic: true
								font.pixelSize: 16
							}

							Rectangle{
								width: 250
								height: 30
								border.color: "white"
							}

							Text{
								id:sosInfoSessionCode

								text: "Input session code:"
								font.pixelSize: 16
							}
							TextField {
								id: sosInfoSessionCodeBox
								placeholderText: "[optional]"
								text: appModel.instantSupportSessionCode

								anchors.margins: 6
								width: 250
								implicitWidth: 285
								height: Math.max(implicitHeight, parent.height)
								implicitHeight: 30

								focus:true
								clip: true
								color: "gray"
								selectByMouse: true
								layer.enabled: true
								font.italic: true
								font.pixelSize: 16
							}
						}

						Button {
							id: sosButton

							anchors.bottom: parent.bottom
							anchors.left: parent.left
							anchors.margins: 8
							anchors.bottomMargin: 14
							anchors.leftMargin: 20

							font.bold: true
							font.pixelSize: 16
							height: 30
							text: "SOS"

							onClicked: {
								appModel.isInstantSupportRequested = true;
								appModel.requestInstantSupport(
											sosInfoTokenBox.text,
											sosInfoNameBox.text,
											sosInfoGroupBox.text,
											sosInfoDescriptionBox.text,
											sosInfoSessionCodeBox.text,
											sosInfoEmailBox.text)
							}
						}
						Button {
							id: saveButton

							anchors.bottom:parent.bottom
							anchors.left: sosButton.right
							anchors.margins: 8
							anchors.bottomMargin: 14

							font.bold: true
							font.pixelSize: 16
							height: 30
							text: "Save"
							onClicked: {
								appModel.instantSupportAccessToken = sosInfoTokenBox.text;
								appModel.instantSupportName = sosInfoNameBox.text;
								appModel.instantSupportGroup = sosInfoGroupBox.text;
								appModel.instantSupportDescription = sosInfoDescriptionBox.text;
								appModel.instantSupportSessionCode = sosInfoSessionCodeBox.text;
								appModel.instantSupportEmail = sosInfoEmailBox.text;
							}
						}
						TextInput {
							id: sosInfoResultSessionCode
							text: appModel.instantSupportResponseCode === appModel.NotInitialized ? ""
								: appModel.instantSupportResponseCode === appModel.Success ? "Session code: %1, State:  %2 \nName: %3, Description: %4"
										.arg(appModel.instantSupportSessionCode)
										.arg(appModel.instantSupportState === appModel.Open ? "Open"
											: appModel.instantSupportState === appModel.Closed ? "Closed"
											: "Undefined")
										.arg(appModel.instantSupportName)
										.arg(appModel.instantSupportDescription)
								: appModel.instantSupportResponseCode === appModel.Processing ? "Request Processing..."
								: appModel.instantSupportResponseCode === appModel.InternalError ? "Internal Error"
								: appModel.instantSupportResponseCode === appModel.InvalidToken ? "Token is not valid"
								: appModel.instantSupportResponseCode === appModel.InvalidGroup ? "Group Name is not valid"
								: appModel.instantSupportResponseCode === appModel.InvalidSessionCode ? "Session Code is not valid"
								: appModel.instantSupportResponseCode === appModel.InvalidEmail ? "Email address is not valid"
								: appModel.instantSupportResponseCode === appModel.Busy ? "Support is already requested"
								: ""
							color: appModel.instantSupportResponseCode !== appModel.Success ? "red" : "#0074CD"
							focus: true

							anchors.left: saveButton.right
							anchors.bottom: sosSettingsPanel.bottom
							anchors.margins: 8
							anchors.leftMargin: 12
							anchors.bottomMargin: 14
							width: parent.width - 150

							clip: true
							wrapMode: TextInput.WordWrap
						}
				}
			}
		}

		Component {
			id: connectionStatusTab
			Column {
				padding: 10

				Rectangle {
					width: parent.width - 40
					height: parent.height
					color: "white"
					radius: 10
					border.color: "#E0E0E0"

					RectangularGlow {
						z: -1
						anchors.fill: parent
						glowRadius: 3
						color: "#80000000"
						cornerRadius: parent.radius + glowRadius
						cached: true
					}

					Item {
						id: statusIndicator

						implicitWidth: statusLED.width
						implicitHeight: statusLED.height
						anchors.left: parent.left
						anchors.top: parent.top
						anchors.margins: 20

						Rectangle {
							id: statusLED
							width: 20
							height: 20
							color: appModel.isTVCommunicationRunning ? "green" : "red"
							radius: 10
							border.color: "#ffffff"
							anchors.centerIn: parent
						}

						Text {
							anchors.left: statusLED.right
							anchors.margins: 20
							font.pixelSize: 20
							text: appModel.isTVCommunicationRunning ? "Agent is connected" : "Agent is not connected"
						}
					}
				}
			}
		}
	}
