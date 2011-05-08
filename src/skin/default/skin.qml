import QtQuick 1.0
import CMPlayerSkin 0.1
import "content"

Skin {
	id: skin
	name: "default"

	function __updateMediaInfo() {media.text = "["+(currentMediaIndex+1)+"/"+mediaCount+"] " + currentMediaInfo}

	onStorageCreated: {
		storage.hide = false
	}
	Component.onCompleted: {
		hider.checked = storage.hide
	}
	Component.onDestruction: {
		storage.hide = hider.checked
	}
	onDurationChanged: {length.text = formatMSec(duration, "hh:mm:ss"); seek.maximum = duration}
	onPositionChanged: {pos.text = formatMSec(position, "hh:mm:ss"); seek.value = position}
	onVolumeChanged: {volume.value = skin.volume}
	onMutedChanged: {mute.icon = muted ? "content/speaker-off.png" : "content/speaker.png"; mute.checked = muted}
	onPlayerStateChanged: {pause.icon = (playerState == Skin.PlayingState) ? "content/pause.png" : "content/play.png"}
	onCurrentMediaIndexChanged: __updateMediaInfo()
	onMediaCountChanged: __updateMediaInfo()
	onCurrentMediaInfoChanged: __updateMediaInfo()
	onFullscreenChanged: {
		hider.visible = !fullscreen;
		if (fullscreen || hider.checked) {
			controls.visible = false
			screen.anchors.bottom = bottom
		} else {
			controls.visible = true
			screen.anchors.bottom = controls.top
		}
	}

	MouseArea {
		id: mouseTracker

		function __contains(min, val, max) {return min <= val && val <= max;}
		hoverEnabled: true
		anchors.fill: parent
		onPositionChanged: {
			if (parent.visible && parent.fullscreen) {
				controls.visible = __contains(0, mouseX-controls.pos.x, controls.width)
						&& __contains(0, mouseY-controls.pos.y, controls.height)
				hider.visible = controls.visible
			}
		}
	}

	Screen {
		id: screen
		width: parent.width
		anchors.top: parent.top
		anchors.bottom: controls.top
	}

	Rectangle {
		id: controls

		width: parent.width; height: 38; anchors.bottom: parent.bottom
		color: "#555"

		Image {
			anchors.fill: parent
			source: "content/background.svg"
			smooth: true
		}

		Rectangle {
			id: boundary

			width: parent.width; height: 4; anchors.top: parent.top
			gradient: Gradient {
				GradientStop {position: 0.0; color: "#000"}
				GradientStop {position: 0.1; color: "#000"}
				GradientStop {position: 0.5; color: "#888"}
				GradientStop {position: 0.6; color: "#000"}
				GradientStop {position: 1.0; color: "#000"}
			}
		}


		Item {
			id: leftBox

			anchors.top: boundary.bottom; anchors.bottom: parent.bottom
			anchors.left: parent.left; anchors.margins: 1
			width: 2*(height + 1) + 1

			Button {
				id: pause
				anchors.top: parent.top; anchors.bottom: parent.bottom
				anchors.left: parent.left;
				width: height
				icon: "content/play.png"
				onClicked: skin.exec("menu/play/pause")
			}

			Button {
				id: backward
				icon: "content/backward.png"
				anchors.top: parent.top; anchors.left: pause.right; anchors.leftMargin: 1
				width: pause.width/2-0.5
				height: width
				onClicked: skin.exec("menu/play/seek/backward1")
			}
			Button {
				id: forward
				icon: "content/forward.png"
				anchors.top: parent.top; anchors.left: backward.right; anchors.leftMargin: 1
				width: backward.width
				height: backward.height
				onClicked: skin.exec("menu/play/seek/forward1")
			}
			Button {
				id: prev
				icon: "content/previous.png"
				anchors.bottom: parent.bottom; anchors.left: pause.right; anchors.leftMargin: 1
				width: backward.width
				height: backward.height
				onClicked: skin.exec("menu/play/prev")
			}
			Button {
				id: next
				icon: "content/next.png"
				anchors.bottom: parent.bottom; anchors.left: prev.right; anchors.leftMargin: 1
				width: backward.width
				height: backward.height
				onClicked: skin.exec("menu/play/next")
			}
		}

		Item {
			id: rightBox

			anchors.top: boundary.bottom; anchors.topMargin: 0; anchors.bottom: parent.bottom
			anchors.left: leftBox.right
			anchors.right: parent.right; anchors.rightMargin: 2
			Rectangle {
				id: lcd

				anchors.top: parent.top
				width: parent.width; height: 22; radius: 3
				border.color: "#aaa"; border.width: 1
				gradient: Gradient {
					GradientStop {position: 0.0; color: "#111"}
					GradientStop {position: 0.1; color: "#6ad"}
					GradientStop {position: 0.8; color: "#6ad"}
					GradientStop {position: 1.0; color: "#fff"}
				}

				Text {
					id: media
					anchors {
						top: parent.top; bottom: parent.bottom
						left: parent.left; right: pos.left
						leftMargin: 3; rightMargin: 3
					}
					elide: Text.ElideRight
					font.pixelSize: 11
					verticalAlignment: "AlignVCenter"
				}

				Text {
					id: pos

					width: paintedWidth; anchors.right: slash.left; height: parent.height
					font.pixelSize: 11
					verticalAlignment: "AlignVCenter"
					text: "00:00:00"
				}
				Text {
					id: slash

					width: paintedWidth; height: parent.height; anchors.right: length.left
					text: "/"
					font.pixelSize: 11
					verticalAlignment: "AlignVCenter"
				}
				Text {
					id: length

					width: paintedWidth; height: parent.height
					anchors.right: parent.right; anchors.rightMargin: 3
					text: "00:00:00"
					font.pixelSize: 11
					verticalAlignment: "AlignVCenter"
				}
			}

			Item {
				width: parent.width
				anchors.top: lcd.bottom
				anchors.bottom: parent.bottom
				Slider {
					id: seek
					anchors.left: parent.left
					anchors.right: mute.left
					anchors.rightMargin: 2
					anchors.verticalCenter: parent.verticalCenter
					onValueChangedByUser: skin.seek(value)
				}

				Button {
					id: mute
					icon: "content/speaker.png"
					width: 10
					anchors.top: parent.top; anchors.topMargin: 1
					anchors.bottom: parent.bottom; anchors.bottomMargin: 1
					anchors.right: volume.left
					anchors.rightMargin: 2
					onClicked: skin.exec("menu/audio/mute")
				}

				Slider {
					id: volume
					maximum: 100
					anchors.right: parent.right
					anchors.verticalCenter: parent.verticalCenter
					width: 70
					onValueChangedByUser: skin.volume = value
				}

			}
		}
	}

	Rectangle {
		id: hider

		property bool checked: false

		anchors.bottom: parent.bottom; anchors.margins: 1
		anchors.left:parent.left
		width: 8
		height: 8
		radius: 3
		color: "#333"
		border.width: 1
		border.color: "#555"
		onCheckedChanged: {
			controls.visible = !checked
			if (fullscreen || checked)
				screen.anchors.bottom = parent.bottom
			else
				screen.anchors.bottom = controls.top
		}
		MouseArea {
			hoverEnabled: true
			anchors.fill: parent
			onHoveredChanged: {
				if (containsMouse)
					parent.border.color = "#6ad"
				else
					parent.border.color = "#555"
				parent.border.width = pressed ? 2 : 1

			}
			onPressed: {parent.border.width = 2; parent.checked = !parent.checked}
			onReleased: {parent.border.width = 1;}
		}
	}
}
