import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Controls.Material 2.4
import QtQuick.Controls.Universal 2.4
import QtQuick.Layouts 1.11


ApplicationWindow {
  visible: true
  width: 768
  height: 800
  title: qsTr("Axion")

  header: ToolBar {
    id: menuBar
    property string prevState: "login"
    property string room_name: ""
    state: "login"
    height: config.iconSize * 2;
    background: Rectangle {
      color: "#404244"
    }

    RowLayout {
      anchors.right: parent.right
      width: parent.width
      spacing: 0

      MenuBarButton {
        id: roomListButton
        iconName: "th-list"
        onClicked: {
          menuBar.prevState = menuBar.state
          menuBar.state = "roomList"
          mainView.push(Qt.resolvedUrl("qrc:/qml/RoomList.qml"))
        }
      }

      MenuBarButton {
        id: backButton
        iconName: "arrow-left"
        onClicked: {
          menuBar.state = menuBar.prevState
          mainView.pop()
        }
      }

      Label {
        id: titleLabel
        text: "Login"
        font.family: "Segoe UI"
        font.pixelSize: config.iconSize;
        font.bold: true
        Layout.topMargin: font.pixelSize / 4  // AlignHCenter looks bad
        Layout.leftMargin: font.pixelSize / 4
        Layout.alignment: Qt.AlignLeft | Qt.AlignTop
      }

      Item { Layout.fillWidth: true }

      MenuBarButton {
        id: searchButton
        iconName: "search"
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
        iconName: "user"
        onClicked: {
          menuBar.prevState = menuBar.state
          menuBar.state = "members"
          mainView.push(Qt.resolvedUrl("qrc:/qml/Members.qml"))
        }
      }

      MenuBarButton {
        id: inviteButton
        iconName: "user-plus"
        onClicked: { }
      }

      MenuBarButton {
        id: notificationsButton
        iconName: "envelope"
        onClicked: { }
      }

      MenuBarButton {
        id: settingsButton
        iconName: "wrench"
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
        iconName: "cog"
        onClicked: {
          menuBar.prevState = menuBar.state
          menuBar.state = "options"
          mainView.push(Qt.resolvedUrl("qrc:/qml/Options.qml"))
        }
      }

      MenuBarButton {
        id: aboutButton
        iconName: "axion"
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
