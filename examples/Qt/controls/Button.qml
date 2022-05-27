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

Item {
	id: root

	signal clicked()

	property bool checked: false
	property bool checkable: false
	onCheckableChanged: {
		checked = false;
	}

	activeFocusOnTab: true

	property int paddingHorizontal: 10
	property int paddingVertical: 4

	readonly property bool hovered: enabled && mouseArea.containsMouse
	readonly property bool pressed: enabled && (checked && checkable || mouseArea.pressed)

	property color backgroundColor: "#a9a9a9"
	property color backgroundColorDisabled: "#d3d3d3"
	property color backgroundColorPressed: "#808080"
	property color backgroundColorHovered: "#a9a9a9"

	property color textColor: "black"
	property color textColorDisabled: "white"
	property color textColorPressed: "white"
	property color textColorHovered: textColor

	property alias text: label.text
	property alias font: label.font

	implicitWidth: label.implicitWidth + paddingHorizontal * 2
	implicitHeight: label.implicitHeight + paddingVertical * 2

	Rectangle {
		id: background

		anchors.fill: parent

		border.color: root.backgroundColorPressed
		border.width: root.focus ? 3 : root.hovered ? 2 : 0

		color:
			!root.enabled ? root.backgroundColorDisabled :
			root.pressed  ? root.backgroundColorPressed :
			root.hovered  ? root.backgroundColorHovered :
			                root.backgroundColor

		Text {
			id: label

			anchors.left: parent.left
			anchors.leftMargin: root.paddingHorizontal
			anchors.right: parent.right
			anchors.rightMargin: root.paddingHorizontal
			anchors.verticalCenter: parent.verticalCenter

			color:
				!root.enabled ? root.textColorDisabled :
				root.pressed  ? root.textColorPressed :
				root.hovered  ? root.textColorHovered :
				                root.textColor
		}

		MouseArea {
			id: mouseArea

			hoverEnabled: true

			anchors.fill: parent

			onClicked: {
				if (root.checkable)
					root.checked = !root.checked
				root.clicked();
			}
		}
	}
}
