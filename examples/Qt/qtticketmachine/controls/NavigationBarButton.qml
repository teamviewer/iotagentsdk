import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects

TabButton {
	id: root
	width: implicitWidth
	height: implicitHeight
	display: checked ? TabButton.TextBesideIcon : TabButton.IconOnly
	topPadding: 10
	leftPadding: 14
	rightPadding: 14
	bottomPadding: 10
	spacing: 10
	clip: true
	icon.color: checked ? palette.highlightedText : palette.buttonText
	font.bold: true
	palette {
		//Should colors be defined and referenced?
		buttonText: "#969696"
		highlight: "#FBE5EE"
		highlightedText: "#EE7CA4"
	}
	contentItem: Item {
		implicitWidth: img.width + (root.spacing + label.contentWidth) * label.visible
		implicitHeight: img.height

		Image {
			id: img
			width: root.icon.width + 10
			height: root.icon.height + 10
			anchors {
				left: root.display === TabButton.IconOnly ? undefined : parent.left
				verticalCenter: parent.verticalCenter
				horizontalCenter: root.display === TabButton.IconOnly ? parent.horizontalCenter : undefined
			}
			source: root.icon.source

			ColorOverlay {
				anchors.fill: parent
				source: parent
				color: root.icon.color
				cached: true
			}
		}

		Label {
			id: label
			anchors {
				topMargin: root.spacing
				left: img.right
				leftMargin: root.spacing
				verticalCenter: parent.verticalCenter
			}
			text: root.text
			font: root.font
			color: root.checked ? root.palette.highlightedText : root.palette.buttonText
			visible: root.display !== TabButton.IconOnly
		}
	}
	background: Item {
		implicitWidth: 40
		implicitHeight: 40

		Rectangle {
			anchors.fill: parent
			color: root.palette.highlight
			radius: parent.height * 0.6
			visible: root.checked
		}
	}

	Behavior on width {
		NumberAnimation {
			duration: 250
			easing.type: Easing.OutBack
		}
	}
}
