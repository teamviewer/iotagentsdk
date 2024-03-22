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

import "../controls"

Item {
	id: root

	property QtObject model
	property int _accessMode: -1
	property int accessControlType

	height: accessControlAreaContents.height + accessControlAreaContents.anchors.margins * 2

	property string title
	property bool hasAfterConfirmation: true

	Connections {
		target: model
		onAccessControlConfirmationRequested: {
			if (accessControl === accessControlType) {
				accessConfirmationArea.requestConfirmation(timeOutMilliseconds)
			}
		}
		onAccessModeChanged: {
			if (accessControl === accessControlType) {
				_accessMode = model.getAccessMode(accessControl);
			}
		}
	}

	Item {
		id: accessControlAreaContents
		anchors.margins: 4
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		implicitHeight: accessModeArea.height

		enabled: root._accessMode != -1

		Item {
			id: accessModeArea

			anchors.verticalCenter: parent.verticalCenter
			anchors.left: parent.left
			width: childrenRect.width
			height: childrenRect.height

			Text {
				id: accessModeLabel
				anchors.top: accessModeArea.top
				text: root.title + ":"
			}

			Text {
				id: accessModeValue
				anchors.top: accessModeArea.top
				anchors.left: accessModeLabel.right
				anchors.leftMargin: 8

				text: _accessMode === model.access_Allowed           ? "Allowed" :
					  _accessMode === model.access_AfterConfirmation ? "After confirmation" :
					  _accessMode === model.access_Denied            ? "Denied" :
					                                                   "Unknown";
			}

			Button {
				id: setAccessModeToAllowedButton

				anchors.top: accessModeValue.bottom
				anchors.margins: 8

				textColor: "white"
				backgroundColor: "green"
				text: "Allowed"
				onClicked: {
					model.setAccessMode(root.accessControlType, model.access_Allowed);
				}
			}

			Button {
				id: setAccessModeToAfterConfirmationButton

				visible: root.hasAfterConfirmation

				anchors.top: accessModeValue.bottom
				anchors.left: setAccessModeToAllowedButton.right
				anchors.margins: 8

				textColor: "white"
				backgroundColor: "#ff7a00"
				text: "After confirmation"
				onClicked: {
					model.setAccessMode(root.accessControlType, model.access_AfterConfirmation);
				}
			}

			Button {
				id: setAccessModeToDeniedButton

				anchors.top: accessModeValue.bottom
				anchors.left:
					setAccessModeToAfterConfirmationButton.visible ?
					setAccessModeToAfterConfirmationButton.right :
					setAccessModeToAllowedButton.right
				anchors.margins: 8

				textColor: "white"
				backgroundColor: "#b40000"
				text: "Denied"
				onClicked: {
					model.setAccessMode(root.accessControlType, model.access_Denied);
				}
			}
		}
	}

	Rectangle {
		id: accessConfirmationArea
		color: "#d9dde2"
		border.color: "#888888"
		border.width: 1

		anchors.right: parent.right
		width: 4 + Math.max(acceptRequestButton.width + 8 + denyRequestButton.width, accessConfirmationMessage.width) + 4
		height: 4 + accessConfirmationMessage.height + 8 + acceptRequestButton.height + 4

		function requestConfirmation(timeout)
		{
			confirmationTimer.stop();
			confirmationTimer.interval = timeout * 1000;
			isConfirmationPending = true;
			confirmationTimer.start();
		}

		function replyConfirmation(confirmed)
		{
			confirmationTimer.stop();
			isConfirmationPending = false;
			model.replyAccessControlRequest(root.accessControlType, confirmed);
		}

		function cancelConfirmation()
		{
			isConfirmationPending = false;
		}

		property bool isConfirmationPending: false

		property alias message: accessConfirmationMessage.text

		visible: isConfirmationPending

		Timer {
			id: confirmationTimer
			repeat: false
			onTriggered: {
				accessConfirmationArea.cancelConfirmation()
			}
		}

		Text {
			id: accessConfirmationMessage

			anchors.top: parent.top
			anchors.left: parent.left
			anchors.margins: 4

			text: "access requested"
		}

		Button {
			id: acceptRequestButton

			anchors.margins: 8
			anchors.leftMargin: 4
			anchors.top: accessConfirmationMessage.bottom
			anchors.left: parent.left

			backgroundColor: "green"
			textColor: "white"

			text: "Accept"

			onClicked: {
				accessConfirmationArea.replyConfirmation(true);
			}
		}

		Button {
			id: denyRequestButton

			anchors.rightMargin: 4
			anchors.margins: 8
			anchors.left: acceptRequestButton.right
			anchors.top: accessConfirmationMessage.bottom

			backgroundColor: "#b40000"
			textColor: "white"

			text: "Deny"

			onClicked: {
				accessConfirmationArea.replyConfirmation(false);
			}
		}
	}
}
