import QtQuick 2.7
import QtQuick.Controls 2.1


ToolButton {
  property int size: config.iconSize * 2
  property string icon: ""
  implicitHeight: size
  implicitWidth: size

  contentItem: Item {
    Rectangle {
      anchors.centerIn: parent
      width: size / 2
      height: size / 2
      color: "transparent"

      Image {
        anchors.fill: parent
        source: "qrc:/img/res/img/tool_icons/" + icon + ".png"
      }
    }
  }
}
