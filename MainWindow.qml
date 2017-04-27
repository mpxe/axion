import QtQuick 2.7
import QtQuick.Controls 2.1


Page {
  header: TabBar {
    id: headerBar
    currentIndex: swipeView.currentIndex
    TabButton {
      text: qsTr("Room list")
    }
    TabButton {
      text: qsTr("Room")
    }
    TabButton {
      text: qsTr("Members")
    }
  }

  SwipeView {
    id: swipeView
    anchors.fill: parent
    currentIndex: headerBar.currentIndex

    RoomList {}
    Room {}
    Members {}
  }
}
