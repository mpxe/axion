import QtQuick 2.7
import QtQuick.Controls 2.1


Page {
  id: roomListPage

  ListView {
    id: roomListView
    anchors.fill: parent
    topMargin: 12
    bottomMargin: 12
    leftMargin: 12
    rightMargin: 12
    spacing: 12
    model: roomListModel

    delegate: Item {
      width: roomListView.width
      height: 64

      Row {
        width: parent.width
        anchors.left: parent.left
        spacing: 12

        Rectangle {
          width: parent.width - 24
          height: 64
          color: "grey"

          Column {
            anchors.left: parent.left
            anchors.margins: 4
            anchors.leftMargin: 8

            Label {
              text: room_name
              renderType: Text.NativeRendering
              font.family: "Segoe UI"
              font.bold: false
              font.italic: false
              font.pixelSize: 24
            }

            Label {
              text: room_id
              renderType: Text.NativeRendering
              font.family: "Segoe UI"
              font.bold: false
              font.italic: false
              font.pixelSize: 16
              color: "lightgrey"
            }
          }
        }
      }

      MouseArea {
        anchors.fill: parent
        onClicked: {
          roomModel.room = room_id
          memberListModel.room = room_id
          menuBar.room_name = room_name
          menuBar.state = "room"
          roomListPage.StackView.view.pop();
        }
      }
    }
  }
}
