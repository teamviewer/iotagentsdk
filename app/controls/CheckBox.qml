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

import "."

Item {
	id: root

	signal clicked()

	property bool checkedStatedFromModel: false
	readonly property bool checked: mouseArea.__checked

	property int spacing: 2
	property int padding: 6

	readonly property bool hovered: enabled && mouseArea.containsMouse
	readonly property bool pressed: enabled && mouseArea.pressed

	property color backgroundColor: "transparent"
	property color backgroundColorPressed: "grey"
	property color backgroundColorHovered: "lightgrey"

	property color textColor: "black"
	property color textColorPressed: "white"
	property color textColorHovered: textColor

	property alias text: label.text
	property alias font: label.font

	implicitWidth: label.implicitWidth + padding * 2 + (imageContainer.visible ? imageContainer.width + root.spacing : 0)
	implicitHeight: label.implicitHeight + padding * 2

	Rectangle {
		id: background

		anchors.fill: parent

		color:
			root.pressed ? root.backgroundColorPressed :
			root.hovered ? root.backgroundColorHovered :
			               root.backgroundColor

		Rectangle
		{
			anchors.leftMargin: root.spacing
			anchors.left: parent.left
			anchors.verticalCenter: parent.verticalCenter

			id: imageContainer
			color: "white"
			border.width: 1
			radius: 4

			width: Math.max(checkMark.width, checkMark.height) + 6
			height: width

			Rectangle {
				id: checkMark
				visible: root.checked
				anchors.centerIn: parent
				height: 10
				width: 10

				radius: 2

				color: "black"
			}
//			Text {
//				id: checkMark
//				text: "✓";
//				anchors.centerIn: parent
//			}
		}

		Text {
			id: label

			anchors.leftMargin: root.spacing
			anchors.left: imageContainer.right
			anchors.verticalCenter: parent.verticalCenter
			anchors.right: parent.right

			color:
				root.pressed ? root.textColorPressed :
				root.hovered ? root.textColorHovered :
				               root.textColor
		}

		MouseArea {
			id: mouseArea

			property bool __checked: root.checkedStatedFromModel

			hoverEnabled: true

			anchors.fill: parent

			onClicked: {
				__checked = !__checked;
				root.clicked();
			}
		}
	}
}
