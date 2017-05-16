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
      readonly property bool myself: account_name === "self"

      width: parent.width
      height: 64

      Row {
        anchors.left: parent.left
        spacing: 12

        Image {
          id: userImage
          width: 64
          height: 64
          source: "qrc:/img/res/img/" + account_name + ".png"
        }

        Rectangle {
          width: memberView.width / 2
          height: userImage.height
          color: myself ? "steelblue" : "grey"

          Column {
            anchors.fill: parent
            anchors.margins: 4
            anchors.leftMargin: 8

            Label {
              text: display_name
              renderType: Text.NativeRendering
              font.family: "Segoe UI"
              font.bold: false
              font.italic: false
              font.pixelSize: 24
            }

            Label {
              text: user_id
              renderType: Text.NativeRendering
              font.family: "Segoe UI"
              font.bold: false
              font.italic: false
              font.pixelSize: 16
            }
          }
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
