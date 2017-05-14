import QtQuick 2.7
import QtQuick.Controls 2.1


Page {
  ListView {
    id: memberView
    anchors.fill: parent
    topMargin: 12
    bottomMargin: 12
    leftMargin: 12
    rightMargin: 12
    spacing: 12
    model: memberListModel

    delegate: Item {
      width: parent.width
      height: 64

      Row {
        anchors.left: parent.left
        spacing: 6

        Image {
          id: userImage
          width: 64
          height: 64
          source: "qrc:/img/res/img/" + account_name + ".png"
        }

        Label {
          text: display_name
          font.family: "Segoe UI"
          font.bold: false
          font.italic: false
          font.pixelSize: 18
          height: userImage.height
        }
      }

      MouseArea {
        anchors.fill: parent
        onClicked: {
          console.log(display_name);
        }
      }
    }
  }
}
