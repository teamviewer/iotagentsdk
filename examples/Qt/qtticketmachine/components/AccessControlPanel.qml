//********************************************************************************//
// MIT License																	//
//																				//
// Copyright (c) 2021 TeamViewer Germany GmbH									 //
//																				//
// Permission is hereby granted, free of charge, to any person obtaining a copy   //
// of this software and associated documentation files (the "Software"), to deal  //
// in the Software without restriction, including without limitation the rights   //
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell	  //
// copies of the Software, and to permit persons to whom the Software is		  //
// furnished to do so, subject to the following conditions:					   //
//																				//
// The above copyright notice and this permission notice shall be included in all //
// copies or substantial portions of the Software.								//
//																				//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR	 //
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,	   //
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE	//
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER		 //
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  //
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  //
// SOFTWARE.																	  //
//********************************************************************************//
import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt5Compat.GraphicalEffects
import "../controls"

Item {
	id: root

	property QtObject model
	property int _accessMode: appModel.getAccessMode(root.accessControlType)
	property int accessControlType

	height: accessControlAreaContents.height + accessControlAreaContents.anchors.margins * 2

	property string title
	property bool hasAfterConfirmation: true

	Connections {
		target: model
		function onAccessControlConfirmationRequested(accessControl, timeOutMilliseconds) {
			if (accessControl === accessControlType) {
				accessConfirmationArea.requestConfirmation(timeOutMilliseconds)
			}
		}
		function onAccessModeChanged(accessControl) {
			if (accessControl === accessControlType) {
				_accessMode = model.getAccessMode(accessControl);
			}
		}
	}
	Item {
		id: accessControlAreaContents
		anchors.margins: 8
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		implicitHeight: accessModeArea.height

		enabled: root._accessMode != -1

		Item {
			id: accessModeArea

			anchors.verticalCenter: parent.verticalCenter
			anchors.left: parent.left
			anchors.right: parent.right
			width: childrenRect.width
			height: childrenRect.height

			Text {
				id: accessModeLabel

				anchors.left: accessModeArea.left
				anchors.verticalCenter: accessControlButtonArea.verticalCenter
				text: root.title + ":"
				font.pointSize: 12
			}

			NavigationBar {
				id: accessControlButtonArea
				anchors.right: parent.right
				anchors.rightMargin: 8
				currentIndex: _accessMode === model.access_Allowed		   ? 0 :
							  _accessMode === model.access_AfterConfirmation ? 1 :
							  _accessMode === model.access_Denied			? 2 :
																			   0 ;
				NavigationBarButton {
					text: "Allowed"
					font.pointSize: 12
					icon.source: "../images/check.png"
					palette {
						buttonText: "#969696"
						highlight: "#b0f1ab"
						highlightedText: "#28ac1c"
					}
					onClicked: {
						model.setAccessMode(root.accessControlType, model.access_Allowed);
					}
				}

				NavigationBarButton {
					text: "After Confirmation"
					font.pointSize: 12
					icon.source: "../images/check_and_x.png"
					palette {
						buttonText: "#969696"
						highlight: "#ffd999"
						highlightedText: "#e28e00"
					}
					onClicked: {
						model.setAccessMode(root.accessControlType, model.access_AfterConfirmation);
					}
				}

				NavigationBarButton {
					text: "Denied"
					font.pointSize: 12
					icon.source: "../images/x_outline.png"
					palette {
						buttonText: "#969696"
						highlight: "#e47b7b"
						highlightedText: "#960000"
					}
					onClicked: {
						model.setAccessMode(root.accessControlType, model.access_Denied);
					}
				}
			}
		}
	}

	Rectangle {
		id: accessConfirmationArea
		anchors.centerIn: parent

		width: 16 + Math.max(acceptRequestButton.width + 8 + denyRequestButton.width, accessConfirmationMessage.width) + 4
		height: 16 + accessConfirmationMessage.height + 8 + acceptRequestButton.height + 4

			RectangularGlow {
				z: -1
				anchors.fill: parent
				glowRadius: 20
				color: "#80000000"
				cached: false
			}

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

		Rectangle {
			id: headerArea

			width: parent.width
			height: 25
			color: "#7a0101"

			Rectangle{
				id: header
				anchors.centerIn: parent
				height: parent.height

				Text {
					id: accessConfirmationMessage

					color: "#ffffff"
					text: "Access Requested"
					font.pixelSize: 20
					anchors.centerIn: parent
				}
			}
		}

		Row {
			spacing: 5
			anchors.top: headerArea.bottom
			anchors.horizontalCenter: parent.horizontalCenter

			anchors.margins: 10

			RoundButton {
				id: acceptRequestButton

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

				text: "Accept"

				onClicked: {
					accessConfirmationArea.replyConfirmation(true);
				}
			}

			RoundButton {
				id: denyRequestButton

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

				text: "Deny"

				onClicked: {
					accessConfirmationArea.replyConfirmation(false);
				}
			}
		}
	}
}
