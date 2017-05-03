import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Controls.Universal 2.1
import QtQuick.Layouts 1.3


ApplicationWindow {
  visible: true
  width: 1024
  height: 1024
  title: qsTr("Axion")

  header: ToolBar {
    id: menuPane
    background: Rectangle {
      color: "#404244"
    }

    RowLayout {
      anchors.right: parent.right
      width: parent.width

      ToolButton {
        id: backButton
        contentItem: Item {
          implicitWidth: 32
          implicitHeight: 32

          Image {
            anchors.centerIn: parent
            source: "qrc:/img/res/img/tool_icons/arrow-left.png"
          }
        }
        onClicked: { mainView.pop() }
      }

/*      ToolButton {
        id: roomButton
        contentItem: Item {
          implicitWidth: 32 + roomName.implicitWidth + 30
          implicitHeight: 32

          RowLayout {
            height: parent.height
            width: parent.width

            Image {
              Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
              Layout.leftMargin: 12
              source: "qrc:/img/res/img/tool_icons/hashtag.png"
            }

            Label {
              id: roomName
              Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
              Layout.rightMargin: 18
              text: "room"
              font.pixelSize: 22
              font.bold: true
            }
          }
        }
        onClicked: { }
      }*/

      Item { Layout.fillWidth: true }

      ToolButton {
        id: searchButton
        contentItem: Item {
          implicitWidth: 32
          implicitHeight: 32

          Image {
            anchors.centerIn: parent
            source: "qrc:/img/res/img/tool_icons/search.png"
          }
        }
        onClicked: { searchPopup.open() }
      }

      Popup {
        id: searchPopup
        x: searchButton.x - width
        y: menuPane.height
        width: 300
        height: 60
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
      }

      ToolButton {
        id: membersButton

        contentItem: Item {
          implicitWidth: 32
          implicitHeight: 32

          Image {
            anchors.centerIn: parent
            source: "qrc:/img/res/img/tool_icons/user.png"
          }
        }
        onClicked: { mainView.push(Qt.resolvedUrl("qrc:/Members.qml")) }
      }

      ToolButton {
        id: inviteButton
        contentItem: Item {
          implicitWidth: 32
          implicitHeight: 32

          Image {
            anchors.centerIn: parent
            source: "qrc:/img/res/img/tool_icons/user-plus.png"
          }
        }
        onClicked: { }
      }

      ToolButton {
        id: notificationsButton
        contentItem: Item {
          implicitWidth: 32
          implicitHeight: 32

          Image {
            anchors.centerIn: parent
            source: "qrc:/img/res/img/tool_icons/envelope.png"
          }
        }
        onClicked: { }
      }

      ToolButton {
        id: settingsButton
        contentItem: Item {
          implicitWidth: 32
          implicitHeight: 32

          Image {
            anchors.centerIn: parent
            source: "qrc:/img/res/img/tool_icons/wrench.png"
          }
        }
        onClicked: { mainView.push(Qt.resolvedUrl("qrc:/Settings.qml")) }
      }

      ToolSeparator {}

      ToolButton {
        id: optionsButton
        contentItem: Item {
          implicitWidth: 32
          implicitHeight: 32

          Image {
            anchors.centerIn: parent
            source: "qrc:/img/res/img/tool_icons/cog.png"
          }
        }
        onClicked: { mainView.push(Qt.resolvedUrl("qrc:/Settings.qml")) }
      }
    }
  }

  StackView {
    id: mainView
    anchors.fill: parent
    initialItem: Login {}
  }
}
