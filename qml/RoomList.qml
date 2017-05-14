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
      width: parent.width
      height: 48

      Row {
        anchors.left: parent.left
        spacing: 6

        Label {
          text: room_name
          font.family: "Segoe UI"
          font.bold: false
          font.italic: false
          font.pixelSize: 18
        }
      }

      MouseArea {
        anchors.fill: parent
        onClicked: {
          roomModel.room = room_id
          memberListModel.room = room_id
          menuBar.state = "room"
          roomListPage.StackView.view.pop();
        }
      }
    }
  }
}
