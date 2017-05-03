import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Controls.Material 2.1
import QtQuick.Controls.Universal 2.1
import QtQuick.Layouts 1.3


Page {
  id: loginPage
  objectName: "loginPage"

  property string errorMessage: ""
  property bool success : false

  onSuccessChanged: {
    if (success)
      loginPage.StackView.view.replace(Qt.resolvedUrl("qrc:/Room.qml"))
  }

  Rectangle {
    anchors.fill: parent
    color: "#2E2F30"
  }

  Image {
    id: logoImage
    anchors.centerIn: parent
    source: "qrc:/img/res/img/icon.png"
  }

  Rectangle {
    x: parent.width / 2 - width / 2
    y: loginLayout.y - height - 12
    width: errorLabel.implicitWidth + 16
    height: errorLabel.implicitHeight + 16
    color: "white"
    visible: errorLabel.text !== ""

    Label {
      id: errorLabel
      anchors.fill: parent
      anchors.margins: 8
      color: "#E02D70"
      text: errorMessage
      wrapMode: Label.Wrap
    }
  }

  ColumnLayout {
    id: loginLayout
    anchors.centerIn: parent
    spacing: 12

    Row {
      Layout.alignment: Qt.AlignLeft | Qt.AlignTop
      spacing: 12

      TextField {
        id: userField
        anchors.verticalCenter: parent.verticalCenter
        width: 300
        placeholderText: qsTr("User")
      }
    }

    Row {
      Layout.alignment: Qt.AlignLeft | Qt.AlignTop
      spacing: 12

      TextField {
        id: passwordField
        anchors.verticalCenter: parent.verticalCenter
        width: 300
        placeholderText: qsTr("Password")
        echoMode: TextInput.Password
      }
    }

    Row {
      Layout.alignment: Qt.AlignLeft | Qt.AlignTop
      Layout.leftMargin: 100
      spacing: 12

      ComboBox {
        id: serverBox
        model: ["matrix.org", "Custom"]
        width: 200
      }
    }

    Row {
      Layout.alignment: Qt.AlignLeft | Qt.AlignTop
      Layout.leftMargin: 300 - loginButton.width
      spacing: 12

      Button {
        id: loginButton
        anchors.verticalCenter: parent.verticalCenter
        text: qsTr("Login")
        Material.background: Material.LightBlue
        highlighted: true
        Universal.accent: Universal.Cyan
        enabled: userField.text !== "" && passwordField.text !== ""
        onClicked: {
          serverBox.currentIndex === 0 ? matrix.login(userField.text, passwordField.text) :
              success = true;
        }
      }
    }
  }
}
