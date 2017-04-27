import QtQuick 2.7
import QtQuick.Controls 2.1

import im.axion 1.0


Page {
  ListView {
    id: memberView
    anchors.fill: parent
    topMargin: 12
    bottomMargin: 12
    leftMargin: 12
    rightMargin: 12
    spacing: 12
    model: MembersModel {}
    delegate: Row {
      anchors.left: parent.left
      spacing: 6

      Image {
        id: userImage
        width: 64
        height: 64
        source: "qrc:/img/res/img/" + model.account_name + ".png"
      }

      Label {
        text: model.display_name
        height: userImage.height
      }
    }
  }
}
