import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Controls.Universal 2.1
import QtQuick.Layouts 1.3

import im.axion 1.0


Page {
  ColumnLayout {
    anchors.fill: parent
    spacing: 0

    ListView {
      id: chatView
      Layout.fillHeight: true
      Layout.fillWidth: true
      Layout.leftMargin: 12
      Layout.rightMargin: 12
      displayMarginBeginning: 48
      displayMarginEnd: 48
      verticalLayoutDirection: ListView.BottomToTop
      spacing: 12
      model: RoomModel {
        room: "!Xq3620DUiqCaoxq:example.com"
      }

      delegate: Row {
        readonly property bool sentByMe: model.user_id === "@self:example.com"
        id: messageRow
        anchors.right: sentByMe ? parent.right : undefined
        spacing: 6

        Image {
          id: userImage
          width: 64
          height: 64
          source: !sentByMe ? "qrc:/img/res/img/miku.png" : ""
        }

        Rectangle {
          width: Math.min(messageText.implicitWidth + 16,
                 chatView.width - (!sentByMe ? userImage.width + messageRow.spacing : 0))
          height: messageText.implicitHeight + 16
          color: sentByMe ? "lightgrey" : "steelblue"

          Label {
            id: messageText
            renderType: Text.NativeRendering
            anchors.fill: parent
            anchors.margins: 8
            text: model.message
            font.family: "Segoe UI"
            font.bold: false
            font.italic: false
            font.pixelSize: 14
            wrapMode: Label.Wrap
            color: sentByMe ? "black" : "white"
          }
        }
      }
    }

    Pane {
      id: inputPane
      Layout.fillWidth: true

      RowLayout {
        width: parent.width
        spacing: 6

        TextArea {
          id: messageField
          Layout.fillWidth: true
          Layout.rightMargin: 6
          placeholderText: "Say something"
          wrapMode: TextArea.Wrap
        }
        Button {
          id: sendButton
          text: "Send"
          Material.background: Material.LightBlue
          Universal.background: Material.LightBlue
          enabled: messageField.length > 0
          onClicked: {
            chatView.model.sendMessage(messageField.text);
            messageField.text = "";
          }
        }
      }
    }
  }
}
