import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Controls.Universal 2.1
import QtQuick.Layouts 1.3


ApplicationWindow {
  visible: true
  width: 800
  height: 768
  title: qsTr("Axion")

  header: ToolBar {
    id: menuBar
    property string prevState: "login"
    property string room_name: ""
    state: "login"
    height: 64
    background: Rectangle {
      color: "#404244"
    }

    RowLayout {
      anchors.right: parent.right
      width: parent.width

      MenuBarButton {
        id: roomListButton
        icon: "th-list"
        onClicked: {
          menuBar.prevState = menuBar.state
          menuBar.state = "roomList"
          mainView.push(Qt.resolvedUrl("qrc:/qml/RoomList.qml"))
        }
      }

      MenuBarButton {
        id: backButton
        icon: "arrow-left"
        onClicked: {
          menuBar.state = menuBar.prevState
          mainView.pop()
        }
      }

      Label {
        id: titleLabel
        text: "Login"
        font.family: "Segoe UI"
        font.pixelSize: 32
        font.bold: true
        Layout.topMargin: 6
        Layout.leftMargin: 12
        Layout.alignment: Qt.AlignTop
      }

      Item { Layout.fillWidth: true }

      MenuBarButton {
        id: searchButton
        icon: "search"
        onClicked: { searchPopup.open() }
      }

      Popup {
        id: searchPopup
        x: Math.max(0, searchButton.x - width + searchButton.width)
        y: menuBar.height
        width: searchRow.width + padding * 2
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

        Row {
          id: searchRow
          spacing: searchPopup.padding

          TextField {
            width: 300
          }

          Button {
            text: "Search"
          }
        }
      }

      MenuBarButton {
        id: membersButton
        icon: "user"
        onClicked: {
          menuBar.prevState = menuBar.state
          menuBar.state = "members"
          mainView.push(Qt.resolvedUrl("qrc:/qml/Members.qml"))
        }
      }

      MenuBarButton {
        id: inviteButton
        icon: "user-plus"
        onClicked: { }
      }

      MenuBarButton {
        id: notificationsButton
        icon: "envelope"
        onClicked: { }
      }

      MenuBarButton {
        id: settingsButton
        icon: "wrench"
        onClicked: {
          menuBar.prevState = menuBar.state
          menuBar.state = "settings"
          mainView.push(Qt.resolvedUrl("qrc:/qml/Settings.qml"))
        }
      }

      ToolSeparator {
        implicitHeight: menuBar.height
        visible: optionsButton.visible &&
            (searchButton.visible || membersButton.visible || inviteButton.visible ||
             notificationsButton.visible || settingsButton.visible)
      }

      MenuBarButton {
        id: optionsButton
        icon: "cog"
        onClicked: {
          menuBar.prevState = menuBar.state
          menuBar.state = "options"
          mainView.push(Qt.resolvedUrl("qrc:/qml/Options.qml"))
        }
      }

      MenuBarButton {
        id: aboutButton
        icon: "axion"
        onClicked: {
          menuBar.prevState = menuBar.state
          menuBar.state = "about"
          mainView.push(Qt.resolvedUrl("qrc:/qml/About.qml"))
        }
      }
    }

    states: [
      State {
        name: "login"
        PropertyChanges { target: titleLabel; text: "Login" }
        PropertyChanges { target: roomListButton; visible: false }
        PropertyChanges { target: backButton; visible: false }
        PropertyChanges { target: searchButton; visible: false }
        PropertyChanges { target: membersButton; visible: false }
        PropertyChanges { target: inviteButton; visible: false }
        PropertyChanges { target: notificationsButton; visible: false }
        PropertyChanges { target: settingsButton; visible: false }
        PropertyChanges { target: optionsButton; visible: true }
        PropertyChanges { target: aboutButton; visible: true }
      },
      State {
        name: "options";
        PropertyChanges { target: titleLabel; text: "Options" }
        PropertyChanges { target: roomListButton; visible: false }
        PropertyChanges { target: backButton; visible: true }
        PropertyChanges { target: searchButton; visible: false }
        PropertyChanges { target: membersButton; visible: false }
        PropertyChanges { target: inviteButton; visible: false }
        PropertyChanges { target: notificationsButton; visible: false }
        PropertyChanges { target: settingsButton; visible: false }
        PropertyChanges { target: optionsButton; visible: false }
        PropertyChanges { target: aboutButton; visible: false }
      },
      State {
        name: "about";
        PropertyChanges { target: titleLabel; text: "About" }
        PropertyChanges { target: roomListButton; visible: false }
        PropertyChanges { target: backButton; visible: true }
        PropertyChanges { target: searchButton; visible: false }
        PropertyChanges { target: membersButton; visible: false }
        PropertyChanges { target: inviteButton; visible: false }
        PropertyChanges { target: notificationsButton; visible: false }
        PropertyChanges { target: settingsButton; visible: false }
        PropertyChanges { target: optionsButton; visible: false }
        PropertyChanges { target: aboutButton; visible: false }
      },
      State {
        name: "members";
        PropertyChanges { target: titleLabel; text: "Members of " + menuBar.room_name }
        PropertyChanges { target: roomListButton; visible: false }
        PropertyChanges { target: backButton; visible: true }
        PropertyChanges { target: searchButton; visible: true }
        PropertyChanges { target: membersButton; visible: false }
        PropertyChanges { target: inviteButton; visible: true }
        PropertyChanges { target: notificationsButton; visible: false }
        PropertyChanges { target: settingsButton; visible: false }
        PropertyChanges { target: optionsButton; visible: false }
        PropertyChanges { target: aboutButton; visible: false }
      },
      State {
        name: "settings";
        PropertyChanges { target: titleLabel; text: "Settings of " + menuBar.room_name }
        PropertyChanges { target: roomListButton; visible: false }
        PropertyChanges { target: backButton; visible: true }
        PropertyChanges { target: searchButton; visible: false }
        PropertyChanges { target: membersButton; visible: false }
        PropertyChanges { target: inviteButton; visible: false }
        PropertyChanges { target: notificationsButton; visible: false }
        PropertyChanges { target: settingsButton; visible: false }
        PropertyChanges { target: optionsButton; visible: false }
        PropertyChanges { target: aboutButton; visible: false }
      },
      State {
        name: "room";
        PropertyChanges { target: titleLabel; text: menuBar.room_name }
        PropertyChanges { target: roomListButton; visible: true }
        PropertyChanges { target: backButton; visible: false }
        PropertyChanges { target: searchButton; visible: true }
        PropertyChanges { target: membersButton; visible: true }
        PropertyChanges { target: inviteButton; visible: false }
        PropertyChanges { target: notificationsButton; visible: true }
        PropertyChanges { target: settingsButton; visible: true }
        PropertyChanges { target: optionsButton; visible: true }
        PropertyChanges { target: aboutButton; visible: true }

      },
      State {
        name: "roomList";
        PropertyChanges { target: titleLabel; text: "Rooms" }
        PropertyChanges { target: roomListButton; visible: false }
        PropertyChanges { target: backButton; visible: true }
        PropertyChanges { target: searchButton; visible: true }
        PropertyChanges { target: membersButton; visible: false }
        PropertyChanges { target: inviteButton; visible: false }
        PropertyChanges { target: notificationsButton; visible: false }
        PropertyChanges { target: settingsButton; visible: false }
        PropertyChanges { target: optionsButton; visible: false }
        PropertyChanges { target: aboutButton; visible: false }
      }
    ]
  }

  StackView {
    id: mainView
    anchors.fill: parent
    initialItem: Login {}
  }
}
