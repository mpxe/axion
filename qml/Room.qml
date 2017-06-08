import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Controls.Universal 2.1
import QtQuick.Layouts 1.3


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
      model: roomModel

      delegate: Row {
        readonly property bool sentByMe: user_id === roomModel.user
        readonly property bool dummyRoom: roomModel.room_name === "music" ||
            roomModel.room_name === "physics"

        id: messageRow
        anchors.right: sentByMe ? parent.right : undefined
        //width: parent.width
        spacing: 12

        Image {
          id: userImage
          width: 64
          height: 64
          smooth: true
          asynchronous: true
          source: !sentByMe ? dummyRoom ? "qrc:/img/res/img/" + account_name + ".png" :
              "image://matrix_media/" + avatar_id : ""
          visible: !sentByMe
        }

        Rectangle {
          width: Math.min(messageText.implicitWidth + 16,
                 chatView.width - (!sentByMe ? userImage.width + 12 : 0))
          height: messageText.implicitHeight + 16
          radius: 8
          function deduceStateColor()
          {
            var stateColor = "grey"
            if (sentByMe) {
              if (message_type == 1) {
                if (transmit_confirmed) {
                  stateColor = "darkorchid"
                }
                else if (transmit_failed) {
                  stateColor = "indianred"
                }
              }
              else {
                if (transmit_confirmed) {
                  stateColor = "teal"
                }
                else if (transmit_failed) {
                  stateColor = "indianred"
                }
              }
            }
            else {
              if (message_type == 1) {
                stateColor = "slateblue"
              }
              else {
                stateColor = "steelblue"
              }
            }

            return stateColor
          }
          color: deduceStateColor()

          Label {
            id: messageText
            renderType: Text.NativeRendering
            anchors.fill: parent
            anchors.margins: 8
            text: sentByMe ? message_type == 1 ? "<i>" + display_name + " " + message_text + "</i>"
                                               : message_text
                           : message_type == 1 ? "<i>" + display_name + " " + message_text + "</i>"
                                               : "<b><font color=\"lightgrey\">" + display_name +
                                                 "</font></b><br>" + message_text
            font.family: "Segoe UI"
            font.bold: false
            font.italic: false
            font.pixelSize: 18
            wrapMode: Label.Wrap
            color: "white"
          }
        }
      }
    }

    Pane {
      id: inputPane
      Layout.fillWidth: true

      function sendMessage() {
        matrix.send_message(roomModel.room, messageField.text);
        messageField.text = "";
      }

      RowLayout {
        width: parent.width
        spacing: 6

        TextArea {
          id: messageField
          Layout.fillWidth: true
          Layout.rightMargin: 6
          placeholderText: "Say something"
          wrapMode: TextArea.Wrap
          Keys.onPressed: {
            if (event.key === Qt.Key_Return) {
              inputPane.sendMessage();
              event.accepted = true;
            }
          }
        }
        Button {
          id: sendButton
          text: "Send"
          Material.background: Material.LightBlue
          Universal.background: Material.LightBlue
          enabled: messageField.length > 0
          onClicked: { inputPane.sendMessage(); }
        }
      }
    }
  }
}
