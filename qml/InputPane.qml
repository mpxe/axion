import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3


Pane {
  id: inputPane
  Layout.fillWidth: true
  contentHeight: messageField.implicitHeight
  
  function sendMessage() {
    matrix.send_message(roomModel.room, messageField.text);
    messageField.text = "";
  }
  
  RowLayout {
    anchors.fill: parent
    spacing: 8
      
    TextArea {
      id: messageField
      Layout.fillWidth: true
      color: "white"
      font.family: "Segoe UI"
      font.bold: false
      font.italic: false
      font.pixelSize: config.textSize
      background: Rectangle {
        color: "teal"
        radius: 8
      }
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
      font.pixelSize: config.textSize
      Layout.fillHeight: true
      enabled: messageField.length > 0
      onClicked: { inputPane.sendMessage(); }
    }
  }
}
