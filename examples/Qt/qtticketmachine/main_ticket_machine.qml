import QtQuick 2.3
import QtQuick.Controls 2.15
import QtQuick.Window 2.0
import Qt5Compat.GraphicalEffects

import "controls"
import "components"

Window {
	id: window
	visible: true
	width: 900
	height: 550
	color: "#fafafa"
	minimumWidth: 640
	minimumHeight: 480
	title: "Ticket Machine RemoteScreen SDK Demo App"

	Component.onCompleted: {
		appModel.requestControlMode(appModel.controlMode_FullControl);
		appModel.setAccessMode(appModel.AccessControl_RemoteView, appModel.Access_Allowed);
		appModel.setAccessMode(appModel.AccessControl_RemoteControl, appModel.Access_Allowed);
		welcomePopUp.open();
	}

	Rectangle {
		id: headerArea

		height: parent.height * 0.08
		color: "#7a0101"
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right

		Button {
			id: backButton
			height: parent.height
			width: height
			anchors.left: parent.left
			visible: viewLoader.source.toString() !== "TicketSelectionView.qml"

			background: Rectangle {
				color: "#7a0101"
			}

			contentItem: Image {
				height: parent.height
				width: parent.width
				source: "images/back_arrow.svg"
			}

			onClicked: {
				viewLoader.source = "TicketSelectionView.qml"
			}
		}

		Rectangle{
			id: header
			anchors.centerIn: parent
			height: parent.height

			Text {
				id: headerText

				color: "#ffffff"
				text: "TicketMachine"
				font.pixelSize: 30
				anchors.centerIn: parent
			}
		}

		Button {
			id: settingsButton
			width: height
			height: parent.height
			anchors.right: parent.right
			anchors.margins: 4

			background: Rectangle {
				color: headerArea.color
			}

			contentItem: Image {
				id: wrench
				source: "images/wrench.svg"
				anchors.centerIn: parent
				height: parent.height - 4
				width: parent.width
			}

			onClicked: {
				viewLoader.source = "Settings.qml"
			}
		}
	}

	Loader {
		id: viewLoader
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: headerArea.bottom
		anchors.bottom: parent.bottom
		source: "TicketSelectionView.qml"
	}

	Button {
		id: chatButton

		width: 35
		height: appModel.isTVSessionRunning ? (parent.height - headerArea.height) * 0.08 - 5 : (parent.height - headerArea.height) * 0.08
		visible: chatPopUp.contacts.count >= 1

		anchors.right: appModel.isTVSessionRunning ? closeSessionButton.left : parent.right
		anchors.bottom: parent.bottom
		anchors.margins: appModel.isTVSessionRunning ? 5 : 0

		background: Rectangle {
			color: "lightgray"
			border.color: "lightgray"
			border.width: 5
			radius: 8
		}

		Image {
			id: chatIcon
			source: "images/chat.svg"
			anchors.centerIn: parent
			height: parent.height
			width: parent.width
		}

		onClicked: {
			chatAreaPopUp.open()
		}
	}

	Button {
		id: closeSessionButton
		visible: appModel.isTVSessionRunning

		width: 35
		height: (openSessionBorder.height - headerArea.height) * 0.08

		anchors.right: openSessionBorder.right
		anchors.bottom: openSessionBorder.bottom

		background: Rectangle {
			color: "#d80000"
			border.color: "#d80000"
			border.width: 5
			radius: 8
		}

		contentItem: Image {
			id: terminateX
			source: "images/x_square.svg"
			anchors.fill: parent
			height: parent.height
			width: parent.width
		}

		onClicked: {
			appModel.terminateTeamViewerSession();
		}
	}

	Rectangle {
		id: openSessionBorder

		visible: appModel.isTVSessionRunning
		border.color: "#d80000"
		color: "transparent"
		border.width: 5

		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: parent.top
		anchors.bottom: parent.bottom
	}

	Popup{
		id: chatAreaPopUp
		width: 700
		height: 450
		focus: true
		modal: true
		anchors.centerIn: parent

		contentItem: Rectangle {
			id: chatArea

			color: "lightgray"
			anchors.fill: parent

			RectangularGlow {
				z: -1
				anchors.fill: parent
				glowRadius: 20
				color: "#80000000"
				cached: false
			}

			Rectangle {
				id: conversations
				anchors.left: parent.left
				anchors.top: parent.top
				anchors.margins: 8
				anchors.topMargin: 16
				border.color: "#e4e4e4"
				height: chatMessagesArea.height - 100
				width: 125
				radius: 10

				Text {
					id: conversationsTitle
					text: "Conversations"
					font.bold: true
					font.pixelSize: 16
					anchors.margins: 8
					anchors.top: parent.top
					anchors.left: parent.left
					anchors.right: parent.right
				}

				ListModel {
					id: contacts
				}

				ListView {
					id: contactList

					anchors.top: conversationsTitle.bottom
					anchors.left: parent.left
					anchors.right: parent.right
					anchors.bottom: parent.bottom
					anchors.topMargin: 8

					model: contacts
					currentIndex: -1
					delegate: Component {
						Item {
							width: parent.width
							height: 25

							MouseArea {
								anchors.fill: parent
								onClicked: {
									contactList.currentIndex = index
								}

								Text {
									text: title

									anchors.bottom: parent.bottom
									anchors.top: parent.top
									anchors.left: parent.left

									anchors.margins: 4
									anchors.leftMargin: 8
								}
								Image{
									id: deleteChatButton
									source: "../images/x.svg"

									width: parent.width/6
									height: parent.height

									anchors.bottom: parent.bottom
									anchors.top: parent.top
									anchors.right: parent.right
									anchors.margins: 4
								}

								MouseArea{
									anchors.fill: deleteChatButton
									onClicked: {
										//ADD Warning PopUp
										appModel.deleteChat()
									}
								}
							}
						}
					}
					highlight: Rectangle {
						id: listElement
						color: '#C0C0C0'
						radius: 10
						Text {
							anchors.centerIn: parent
							anchors.margins: 8
							color: 'white'
						}

					}
					focus: true
					clip: true
					onCurrentIndexChanged: {
						chatMessagesContent.clear()
						newMessageText.clear()
						if (contactList.currentIndex !== -1) {
							chatTitle.text = model.get(contactList.currentIndex).title + ' ' + model.get(contactList.currentIndex).chatTypeId
							if (model.get(contactList.currentIndex).chatState) {
								chatTitle.text += ' ' + "(Closed)"
							}
							appModel.selectChat(model.get(contactList.currentIndex).chatId)
							appModel.loadMessages()
							newMessageTextArea.enabled = appModel.isChatSelected && contactList.currentIndex != -1 && !contacts.get(contactList.currentIndex).chatState
						}
					}
				}
			}

			Rectangle {
				id: chatMessagesArea
				color: "#ffffff"
				anchors.top: parent.top
				anchors.bottom: newMessageArea.top
				anchors.left: conversations.right
				anchors.right: parent.right
				anchors.margins: 8
				anchors.topMargin: 16
				radius: 10
				border.color: "#e4e4e4"

				Text {
					id: chatTitle
					text: appModel.chatTitle
					font.bold: true
					color: "#0064c8"
					anchors.top: parent.top
					anchors.left: chatMessagesArea.left
					anchors.right: parent.right
					anchors.margins: 8
					font.pixelSize: 16
				}

				Flickable {
					id: chatMessagesFlick
					anchors.fill: parent
					anchors.margins: 8
					clip: true

					contentHeight: chatMessagesContent.paintedHeight
					onContentHeightChanged: scrollToEnd()

					function scrollToEnd()
					{
						contentY = contentHeight - height
					}

					function ensureVisible(r) {
						if (contentX >= r.x)
							contentX = r.x;
						else if (contentX+width <= r.x+r.width)
							contentX = r.x+r.width-width;
						if (contentY >= r.y)
							contentY = r.y;
						else if (contentY+height <= r.y+r.height)
							contentY = r.y+r.height-height;
					}

					TextEdit {
						id: chatMessagesContent
						readOnly: true
						anchors.top: parent.top
						anchors.left: parent.left
						anchors.right: parent.right
						wrapMode: TextEdit.Wrap
						height: Math.max(paintedHeight, parent.height)
						font.pixelSize: 16

						onCursorRectangleChanged: chatMessagesFlick.ensureVisible(cursorRectangle)
						onTextChanged: chatMessagesFlick.scrollToEnd()
					}
				}
			}

			Rectangle {
				id: newMessageArea
				anchors.bottom: parent.bottom
				anchors.left: chatMessagesArea.left
				anchors.right:parent.right
				anchors.bottomMargin: 18
				anchors.rightMargin: 8
				height: newMessageTextArea.height + msgMaxLengthWarning.height + 7
				radius: 10

				Rectangle {
					id: newMessageTextArea
					color: "#ffffff"
					border.color: "#e4e4e4"
					anchors.top: parent.top
					anchors.left: parent.left
					anchors.right: sendChatMessageButton.left
					anchors.rightMargin: 4
					anchors.leftMargin: 4
					anchors.topMargin: 4

					height: 22
					radius: 10
					enabled: appModel.isChatSelected && contactList.currentIndex != -1 && !contacts.get(contactList.currentIndex).chatState

					Flickable {
						id: newMessageFlick
						anchors.fill: parent
						clip: true

						TextEdit {
							id: newMessageText
							anchors.top: parent.top
							anchors.left: parent.left
							anchors.right: parent.right
							anchors.margins: 4
							height: Math.max(implicitHeight, parent.height)
							font.pixelSize: 16

							property int maximumLength: 1000
							property string previousText: text
							onTextChanged: {
								if (text.length > maximumLength) {
									var cursor = cursorPosition;
									text = previousText;
									if (cursor > text.length) {
										cursorPosition = text.length;
									} else {
										cursorPosition = cursor - 1;
									}
								}
								previousText = text
							}
						}
					}
				}

				RoundButton {
					id: sendChatMessageButton
					enabled: newMessageArea.enabled && newMessageText.text.length !== 0

					width: sendText.width + 8
					height: newMessageTextArea.height

					anchors.right: deleteHistoryButton.left
					anchors.top: parent.top
					anchors.topMargin: 4
					anchors.bottomMargin: 4
					anchors.rightMargin: 2

					Text{
						id: sendText
						anchors.centerIn: parent

						text: "Send"
					}
					onClicked: {
						appModel.sendChatMessage(newMessageText.text);
						newMessageText.clear();
					}
				}

				Image{
					id: deleteHistoryButton

					source: "../images/trash.svg"
					height: newMessageTextArea.height
					width: newMessageArea.height/2

					anchors.top: parent.top
					anchors.right: parent.right
					anchors.margins: 4

					MouseArea{
						anchors.fill: deleteHistoryButton
						onClicked: {
							//ADD in warning popup
							appModel.deleteHistory()
						}
					}

				}

				Text {
					id: msgMaxLengthWarning
					text: "Message is limited to 1000 characters"
					color: "gray"
					anchors.top: newMessageTextArea.bottom
					anchors.left: parent.left
					anchors.right:parent.right
					anchors.leftMargin: 6
				}
			}

			Connections {
				target: appModel
				function onTvCommunicationRunningChanged() {
					if (!appModel.isTVCommunicationRunning) {
						contacts.clear()
						chatMessagesContent.clear()
						newMessageText.clear()
						chatTitle.text = appModel.chatTitle
					}
				}
				function onNewContactAdded(chatInfo) {
					contacts.append(chatInfo)
					if (contactList.currentIndex == -1) {
						contactList.currentIndex++
					}
				}
				function onChatRemoved(chatId) {
					for (var i = 0; i < contacts.count; ++i) {
						if (contacts.get(i).chatId === chatId) {
							if (i === contactList.currentIndex) {
								contactList.currentIndex = -1
								chatTitle.text = "Chat"
								chatMessagesContent.clear()
								newMessageText.clear()
							}
							newMessageTextArea.enabled = contactList.currentIndex != -1 && !contacts.get(contactList.currentIndex).chatState
							contacts.remove(i)
						}
					}
					if(contacts.count < 1) chatAreaPopUp.close()
				}
				function onNewChatMessageAdded(message, timestamp, sender) {
					chatMessagesContent.append('\n' + sender + ": " + timestamp + '\n' + message)
				}
				function onLoadedMessagesAdded(message, timestamp, sender) {
					chatMessagesContent.insert(0, '\n\n' + sender + ": " + timestamp + '\n' + message)
				}
				function onChatClosed(chatId) {
					for (var i = 0; i < contacts.count; ++i) {
						if (contacts.get(i).chatId === chatId) {
							contacts.get(i).chatState = 1
							if (i === contactList.currentIndex) {
								chatTitle.text += ' ' + "(Closed)"
								newMessageTextArea.enabled = !contacts.get(contactList.currentIndex).chatState
								newMessageText.clear()
							}
						}
					}
				}
				function onHistoryDeleted(chatId) {
					for (var i = 0; i < contacts.count; ++i) {
						if (contacts.get(i).chatId === chatId) {
							if (i === contactList.currentIndex) {
								chatMessagesContent.clear()
							}
						}
					}
				}
			}
		}
	}

	WelcomePopUp{
		id: welcomePopUp
	}

	SosPopUp{
		id: sosPopUp
	}
}
