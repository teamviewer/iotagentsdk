//********************************************************************************//
// MIT License                                                                    //
//                                                                                //
// Copyright (c) 2021 TeamViewer Germany GmbH                                     //
//                                                                                //
// Permission is hereby granted, free of charge, to any person obtaining a copy   //
// of this software and associated documentation files (the "Software"), to deal  //
// in the Software without restriction, including without limitation the rights   //
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      //
// copies of the Software, and to permit persons to whom the Software is          //
// furnished to do so, subject to the following conditions:                       //
//                                                                                //
// The above copyright notice and this permission notice shall be included in all //
// copies or substantial portions of the Software.                                //
//                                                                                //
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     //
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       //
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    //
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         //
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  //
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  //
// SOFTWARE.                                                                      //
//********************************************************************************//
import QtQuick 2.3
import QtQuick.Window 2.0

import "controls"
import "components"

Window {
	id: window
	visible: true
	width: 640
	height: 480
	color: "#fafafa"
	minimumWidth: 640
	minimumHeight: 480
	title: "TeamViewer RemoteScreen SDK Demo App"

	Rectangle {
		id: headerArea

		height: headerAreaContents.height + headerAreaContents.anchors.margins * 2
		color: "#0064c8"
		anchors.left: parent.left
		anchors.right: parent.right

		Item {
			id: headerAreaContents
			height: Math.max(statusLED.height, headerText.height)

			anchors.margins: 4
			anchors.top: parent.top
			anchors.left: parent.left
			anchors.right: parent.right

			Item {
				id: statusIndicator

				implicitWidth: statusLED.width
				implicitHeight: statusLED.height
				anchors.left: parent.left
				anchors.top: parent.top
				height: parent.height
				width: height

				Rectangle {
					id: statusLED
					width: 20
					height: 20
					color: appModel.isTVCommunicationRunning ? "green" : "red"
					radius: 10
					border.color: "#ffffff"
					anchors.centerIn: parent
				}
			}

			Text {
				id: headerText
				color: "#ffffff"
				text: "TeamViewer Agent SDK Demo App"
				anchors.right: parent.right
				anchors.verticalCenter: parent.verticalCenter
				anchors.left: statusIndicator.right
				anchors.leftMargin: 4
				font.pixelSize: 25
			}
		}
	}

	Rectangle {
		id: sessionControlArea

		color: "#e9edf2"
		anchors.right: parent.right
		anchors.left: parent.left
		anchors.leftMargin: 0
		anchors.top: headerArea.bottom

		height: controlAreaContents.height + controlAreaContents.anchors.margins * 2

		Item {
			id: controlAreaContents
			anchors.margins: 4
			anchors.top: parent.top
			anchors.left: parent.left
			anchors.right: parent.right
			implicitHeight: sessionControlTitle.height + 8 + Math.max(controlModeArea.height, terminateTvSession.height)

			Text {
				id: sessionControlTitle
				text: "RC Session Control"
				font.bold: true

				anchors.top: parent.top
				anchors.left: parent.left
			}

			Text {
				id: rcStateLabel
				visible: appModel.isTVSessionRunning
				text: "[Remote Screen session is running]"

				anchors.top: parent.top
				anchors.right: parent.right
			}

			Item {
				id: controlModeArea

				anchors.top: sessionControlTitle.bottom
				anchors.topMargin: 8
				anchors.left: parent.left
				width: childrenRect.width
				height: childrenRect.height

				Text {
					id: controlModeLabel
					anchors.top: parent.top
					text: "Remote Screen Control mode:"
				}

				Text {
					id: controlModeValue
					anchors.top: parent.top
					anchors.left: controlModeLabel.right
					anchors.leftMargin: 8

					text: appModel.controlMode === appModel.controlMode_Disabled    ? "Disabled" :
						  appModel.controlMode === appModel.controlMode_ViewOnly    ? "View-Only" :
						  appModel.controlMode === appModel.controlMode_FullControl ? "Full control" :
																					  "Unknown";
				}

				Button {
					id: setControlModeToFullControlButton

					anchors.top: controlModeValue.bottom
					anchors.margins: 8

					text: "Set Full control"
					onClicked: {
						appModel.requestControlMode(appModel.controlMode_FullControl);
					}
				}

				Button {
					id: setControlModeToViewOnlyButton

					anchors.top: controlModeValue.bottom
					anchors.left: setControlModeToFullControlButton.right
					anchors.margins: 8

					text: "Set View Only"
					onClicked: {
						appModel.requestControlMode(appModel.controlMode_ViewOnly);
					}
				}

				Button {
					id: setControlModeToDisabledButton

					anchors.top: controlModeValue.bottom
					anchors.left: setControlModeToViewOnlyButton.right
					anchors.margins: 8

					text: "Disable Remote Control"
					onClicked: {
						appModel.requestControlMode(appModel.controlMode_Disabled);
					}
				}
			}

			Button {
				id: terminateTvSession

				anchors.right: parent.right
				anchors.bottom: parent.bottom

				text: "Terminate remote sessions"
				onClicked: {
					appModel.terminateTeamViewerSession();
				}
			}
		}
	}

	Rectangle {
		id: tabBar

		anchors.top: sessionControlArea.bottom
		anchors.topMargin: 8
		anchors.left: parent.left
		anchors.right: parent.right

		height: childrenRect.height
		color: "#fafafa"

		property int currentTab
		onCurrentTabChanged: {
			for (var i in tabBar.children) {
				tabBar.children[i].checked = tabBar.currentTab === tabBar.children[i].tabId;
			}
		}

		Button {
			id: showAccessControls

			anchors.top: parent.top
			anchors.left: parent.left
			anchors.leftMargin: 8

			text: "Access Controls"
			backgroundColor: tabBar.color

			property int tabId: 0
			checkable: true
			onClicked: { tabBar.currentTab = tabId; }
		}

		Button {
			id: showSos

			anchors.top: parent.top
			anchors.left: showAccessControls.right

			text: "Instant Support"
			backgroundColor: tabBar.color

			property int tabId: 1
			checkable: true
			onClicked: { tabBar.currentTab = tabId; }
		}

		Button {
			id: showInputTest

			anchors.top: parent.top
			anchors.left: showSos.right

			text: "Input Tester"
			backgroundColor: tabBar.color

			property int tabId: 2
			checkable: true
			onClicked: { tabBar.currentTab = tabId; }
		}

		Button {
			id: showChat

			anchors.top: parent.top
			anchors.left: showInputTest.right

			text: "Chat"
			backgroundColor: tabBar.color

			property int tabId: 3
			checkable: true
			onClicked: { tabBar.currentTab = tabId; }
		}

		Component.onCompleted: {
			for (var i in tabBar.children) {
				tabBar.children[i].checked = tabBar.currentTab === tabBar.children[i].tabId;
			}
		}
	}

	Rectangle {
		id: splitter
		height: 1
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.top: tabBar.bottom

		color: "#808080"
	}

	Rectangle {
		id: accessControlsSettingsPanel

		visible: showAccessControls.checked

		anchors.top: splitter.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.margins: 0

		color: "#fafafa"

		AccessControlPanel {
			id: fileTransferAccessControlArea

			anchors.top: parent.top
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.margins: 8

			model: appModel
			enabled: appModel.isTVCommunicationRunning
			accessControlType: appModel.accessControl_FileTransfer
			title: "[File transfer] access"
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

	Rectangle {
		id: inputTestArea

		visible: showInputTest.checked

		color: "#fafafa"

		anchors.top: splitter.bottom
		anchors.left: parent.left
		anchors.bottom: parent.bottom
		anchors.right: parent.right

		Rectangle {
			id: mouseTestArea
			color: "#ffffff"
			anchors.right: parent.right
			anchors.left: parent.left
			anchors.top: parent.top
			anchors.bottom: textTestArea.top
			border.color: "#e4e4e4"
			anchors.margins: 8

			Item {
				id: movableTV
				width: tvLogo.width
				height: tvLogo.height

				property bool untouched: true

				x: (parent.width / 2 - width) / 2
				y: (parent.height - height) / 2

				Image {
					id: tvLogo
					source: "qrc:///main_logo.png"
				}

				MouseArea {
					anchors.fill: parent
					property int oldX: -1
					property int oldY: -1

					onPressed: {
						oldX = mouse.x;
						oldY = mouse.y;
					}
					onReleased: {
						oldX = -1;
						oldY = -1;
					}

					onPositionChanged: {
						var newX = parent.x + mouse.x - oldX;
						var newY = parent.y + mouse.y - oldY;

						parent.x = Math.min(Math.max(0, newX), mouseTestArea.width  - parent.width);
						parent.y = Math.min(Math.max(0, newY), mouseTestArea.height - parent.height);

						parent.untouched = false;
					}
				}
			}

			Item {
				id: movableAP
				width: apLogo.width
				height: apLogo.height

				property bool untouched: true

				x: parent.width / 2 + (parent.width / 2 - width) / 2
				y: (parent.height - height) / 2

				Image {
					id: apLogo
					source: "qrc:///Qt_logo_2016.png"
				}

				MouseArea {
					anchors.fill: parent
					property int oldX: -1
					property int oldY: -1

					onPressed: {
						oldX = mouse.x;
						oldY = mouse.y;
					}
					onReleased: {
						oldX = -1;
						oldY = -1;
					}

					onPositionChanged: {
						var newX = parent.x + mouse.x - oldX;
						var newY = parent.y + mouse.y - oldY;

						parent.x = Math.min(Math.max(0, newX), mouseTestArea.width  - parent.width);
						parent.y = Math.min(Math.max(0, newY), mouseTestArea.height - parent.height);

						parent.untouched = false;
					}
				}
			}

			Rectangle {
				id: tooltip

				color: "#AAe0e0e0"
				width: toolTipText.width + 8
				height: toolTipText.height + 8

				visible: movableTV.untouched && movableAP.untouched
				anchors.top: parent.top
				anchors.topMargin: 8
				anchors.horizontalCenter: parent.horizontalCenter

				Text {
					id: toolTipText
					text: "the logos can be moved around"
					anchors.centerIn: parent
				}
			}
		}

		Rectangle {
			id: textTestArea
			color: "#ffffff"
			anchors.bottom: controlsTestArea.top
			anchors.left: parent.left
			anchors.right: parent.right
			height: 50
			anchors.margins: 8
			border.color: "#e4e4e4"

			clip: true

			Flickable {
				id: flickable
				anchors.fill: parent
				anchors.margins: 2
				clip: true

				TextEdit {
					id: textEdit
					text: "Text input test"
					anchors.right: parent.right
					anchors.left: parent.left
					anchors.top: parent.top
					height: Math.max(implicitHeight, parent.height)
				}
			}
		}

		Rectangle {
			id: animationArea
			anchors.right: parent.right
			anchors.bottom: parent.bottom
			anchors.margins: 8
			radius: 10
			color: "white"
			border.color: "#e4e4e4"

			property int runLength: 100
			height: animationLED.height + 4
			width: animationLED.width + 4 + runLength

			Item {
				x: 2
				y: 2
				Rectangle {
					id: animationLED
					width: 20
					height: 20
					color: headerArea.color
					radius: 10
					border.color: "#ffffff"

					SequentialAnimation {
						loops: Animation.Infinite
						running: controlsEnabler.checked
						NumberAnimation { target: animationLED; property: "x"; duration: 1000; from: 0; to: animationArea.runLength; easing.type: Easing.InOutQuad }
						NumberAnimation { target: animationLED; property: "x"; duration: 1000; from: animationArea.runLength; to: 0; easing.type: Easing.InOutQuad }
					}
				}
			}
		}

		Item {
			id: controlsTestArea

			anchors.right: parent.right
			anchors.left: parent.left
			anchors.bottom: parent.bottom
			anchors.margins: 8

			height: controlsEnabler.height + buttonTestArea.anchors.topMargin + buttonTestArea.height

			CheckBox {
				id: controlsEnabler

				text: "enable controls and animation testing"
			}

			Item {
				id: buttonTestArea
				enabled: controlsEnabler.checked

				anchors.topMargin: 8
				anchors.top: controlsEnabler.bottom
				anchors.right: parent.right
				anchors.left: parent.left
				height: Math.max(Math.max(button1.height, button2.height), button3.height)

				Button {
					id: button1
					anchors.left: parent.left
					anchors.margins: 8

					text: "first"

					onClicked: {
						button3.enabled = !button3.enabled
					}
				}

				Button {
					id: button2
					anchors.left: button1.right
					anchors.margins: 8

					text: "second"

					onClicked: {
						button2.visible = false
					}
				}

				Button {
					id: button3
					anchors.left: button2.right
					anchors.margins: 8

					text: "third"

					onClicked: {
						button3.visible = false
					}
				}

				Button {
					id: resetButton
					anchors.left: button3.right
					anchors.margins: 8

					text: "reset test buttons"

					onClicked: {
						button1.visible = true
						button2.visible = true
						button3.visible = true
						button1.enabled = true
						button2.enabled = true
						button3.enabled = true
					}
				}
			}
		}
	}

	Rectangle {
		id: chatArea

		visible: showChat.checked

		color: "#fafafa"

		anchors.top: splitter.bottom
		anchors.left: parent.left
		anchors.bottom: parent.bottom
		anchors.right: parent.right
		anchors.margins: 8

		Text {
			id: chatTitle
			text: appModel.chatTitle
			font.bold: true
			color: "#0064c8"
			anchors.top: parent.top
			anchors.left: chatMessagesArea.left
			anchors.right: parent.right
		}

		Rectangle {
			id: conversations
			anchors.left: parent.left
			anchors.top: parent.top
			anchors.topMargin: 4
			border.color: "#e4e4e4"
			height: chatMessagesArea.height + chatTitle.height
			width: 125

			ListModel {
				id: contacts
			}

			ListView {
				id: contactList
				anchors.fill: parent
				model: contacts
				currentIndex: -1
				delegate: Component {
					Item {
						width: parent.width
						height: 25

						Row {
							Text { text: title }
						}
						MouseArea {
							anchors.fill: parent
							onClicked: {
								contactList.currentIndex = index
							}
						}
					}
				}
				highlight: Rectangle {
					id: listElement
					color: '#C0C0C0'
					Text {
						anchors.centerIn: parent
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
			id: deleteArea
			anchors.left: parent.left
			anchors.top: conversations.bottom
			anchors.bottom: parent.bottom
			width: conversations.width
			enabled: appModel.isChatSelected && contactList.currentIndex != -1

			Text {
				id: deleteChatAndHistory
				text: "Delete"
				font.bold: true
				anchors.top: parent.top
				anchors.left: parent.left
				anchors.right: parent.right
			}

			Button {
				id: deleteHistory
				anchors.left: parent.left
				anchors.top: deleteChatAndHistory.bottom
				anchors.topMargin: 4
				anchors.rightMargin: 4

				text: "History"

				onClicked: {
					appModel.deleteHistory()
				}
			}

			Button {
				id: deleteChat
				anchors.top: deleteChatAndHistory.bottom
				anchors.left: deleteHistory.right
				anchors.right: parent.right
				anchors.topMargin: 4
				anchors.leftMargin: 4

				text: "Chat"

				onClicked: {
					appModel.deleteChat()
				}
			}
		}

		Rectangle {
			id: chatMessagesArea
			color: "#ffffff"
			anchors.top: chatTitle.bottom
			anchors.bottom: newMessageArea.top
			anchors.left: conversations.right
			anchors.right: parent.right
			anchors.leftMargin: 8
			anchors.topMargin: 4
			anchors.bottomMargin: 4
			border.color: "#e4e4e4"

			Flickable {
				id: chatMessagesFlick
				anchors.fill: parent
				anchors.margins: 2
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
			height: newMessageTextArea.height + msgMaxLengthWarning.height

			Rectangle {
				id: newMessageTextArea
				color: "#ffffff"
				border.color: "#e4e4e4"
				anchors.top: parent.top
				anchors.left: parent.left
				anchors.right: sendMessageButton.left
				anchors.rightMargin: 4
				anchors.topMargin: 2
				anchors.bottomMargin: 8
				height: 22
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
						height: Math.max(implicitHeight, parent.height)

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

			Button {
				id: sendMessageButton
				enabled: newMessageArea.enabled && newMessageText.text.length !== 0
				anchors.right: parent.right
				anchors.top: parent.top
				anchors.topMargin: 2
				anchors.bottomMargin: 4
				height: newMessageTextArea.height
				text: "Send"

				onClicked: {
					appModel.sendChatMessage(newMessageText.text);
					newMessageText.clear();
				}
			}

			Text {
				id: msgMaxLengthWarning
				text: "Message is limited to 1000 characters"
				color: "gray"
				anchors.top: newMessageTextArea.bottom
				anchors.left: parent.left
				anchors.right:parent.right
			}
		}

		Connections {
			target: appModel
			onTvCommunicationRunningChanged: {
				if (!appModel.isTVCommunicationRunning) {
					contacts.clear()
					chatMessagesContent.clear()
					newMessageText.clear()
					chatTitle.text = appModel.chatTitle
				}
			}
			onNewContactAdded: {
				contacts.append(chatInfo)
				if (contactList.currentIndex == -1) {
					contactList.currentIndex++
				}
			}
			onChatRemoved: {
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
			}
			onNewChatMessageAdded: {
				chatMessagesContent.append('\n' + sender + ": " + timestamp + '\n' + message)
			}
			onLoadedMessagesAdded: {
				chatMessagesContent.insert(0, '\n\n' + sender + ": " + timestamp + '\n' + message)
			}
			onChatClosed: {
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
			onHistoryDeleted: {
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

	Rectangle {
		id: sos

		visible: showSos.checked

		color: "#fafafa"

		anchors.top: splitter.bottom
		anchors.left: parent.left
		anchors.bottom: parent.bottom
		anchors.right: parent.right

		Item {
			id: sosArea
			anchors.margins: 4
			anchors.top: parent.top
			anchors.left: parent.left
			anchors.right: parent.right
			implicitHeight: sosField.height
			height: 290
			clip: true
			Item {
				id: sosField
				anchors.left: parent.left
				width: childrenRect.width
				height: 25
				Text {
					id: instantSupport
					anchors.top: parent.top
					anchors.left: parent.left
					anchors.margins: 8
					anchors.bottomMargin: 20
					text: "Get Instant Support"
					font.bold: true
					font.pixelSize: 16
				}

				TextInput {
					id: sosInfoToken
					text: "Input token..."
					focus: true
					anchors.top: instantSupport.bottom
					anchors.left: parent.left
					anchors.margins: 6
					width: 250
					clip: true
					color: "gray"
					selectByMouse: true
					layer.enabled: true
					font.italic: true

					height: Math.max(implicitHeight, parent.height)
				}

				TextInput {
					id: sosInfoName
					text: "Input name..."
					focus: true
					anchors.top: sosInfoToken.bottom
					anchors.left: parent.left
					anchors.margins: 6
					width: 250
					clip: true
					color: "gray"
					selectByMouse: true
					layer.enabled: true
					font.italic: true

					height: Math.max(implicitHeight, parent.height)
				}

				TextInput {
					id: sosInfoGroup
					text: "Input group..."
					focus: true
					anchors.top: sosInfoName.bottom
					anchors.left: parent.left
					anchors.margins: 6
					width: 250
					clip: true
					color: "gray"
					selectByMouse: true
					layer.enabled: true
					font.italic: true
					height: Math.max(implicitHeight, parent.height)
				}

				TextInput {
					id: sosInfoDescription
					text: "Input description..."
					focus: true
					anchors.top: sosInfoGroup.bottom
					anchors.left: parent.left
					anchors.margins: 6
					width: 250
					clip: true
					color: "gray"
					selectByMouse: true
					layer.enabled: true
					font.italic: true
					height: Math.max(implicitHeight, parent.height)
				}
				TextInput {
					id: sosInfoEmail
					text: "[optional] email address..."
					focus: true
					anchors.top: sosInfoDescription.bottom
					anchors.left: parent.left
					anchors.margins: 6
					width: 250
					clip: true
					color: "gray"
					selectByMouse: true
					layer.enabled: true
					font.italic: true
					height: Math.max(implicitHeight, parent.height)
				}
				TextInput {
					id: sosInfoSessionCode
					text: "[optional] session code..."
					focus: true
					anchors.top: sosInfoEmail.bottom
					anchors.left: parent.left
					anchors.margins: 6
					width: 250
					clip: true
					color: "gray"
					selectByMouse: true
					layer.enabled: true
					font.italic: true
					height: Math.max(implicitHeight, parent.height)
				}
				Button {
					id: sosButton
					anchors.top: sosInfoSessionCode.bottom
					anchors.left: parent.left
					anchors.margins: 8
					textColor: "white"
					font.bold: true
					font.pixelSize: 16
					backgroundColor: "red"
					height: 30
					text: "SOS"
					onClicked: {
						appModel.requestInstantSupport(
									sosInfoToken.text,
									sosInfoName.text,
									sosInfoGroup.text,
									sosInfoDescription.text,
									sosInfoSessionCode.text,
									sosInfoEmail.text)
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
					anchors.left: sosButton.right
					anchors.top: sosInfoSessionCode.bottom
					verticalAlignment: TextInput.AlignVCenter
					anchors.margins: 8
					anchors.leftMargin: 12
					clip: true
					height: Math.max(implicitHeight, parent.height)
				}

				Button {
					id: acceptInstantSupportRequest
					enabled: appModel.isInstantSupportConfirmationRequested && appModel.isTVCommunicationRunning

					anchors.margins: 8
					anchors.top: sosButton.bottom

					text: qsTr("Accept")
					onClicked: {
						appModel.acceptInstantSupportConfirmationRequest();
					}
				}

				Button {
					id: denyInstantSupportRequest
					enabled: appModel.isInstantSupportConfirmationRequested && appModel.isTVCommunicationRunning

					anchors.margins: 8
					anchors.left: acceptInstantSupportRequest.right
					anchors.top: sosButton.bottom

					text: qsTr("Deny")
					onClicked: {
						appModel.denyInstantSupportConfirmationRequest();
					}
				}
			}
		}
	}

}
