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
    model: ["!Xq3620DUiqCaoxq:example.com", "test"]
    delegate: ItemDelegate {
      text: modelData
      width: roomListView.width - roomListView.leftMargin - roomListView.rightMargin
      height: 48
    }
  }
}
