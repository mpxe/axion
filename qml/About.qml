import QtQuick 2.7
import QtQuick.Controls 2.1


Page {
  id: aboutPage

  leftPadding: 12
  rightPadding: 12
  topPadding: 12
  bottomPadding: 12

  Row {
    padding: 12
    spacing: 24

    Image {
      id: logoImage
      width: 256
      height: 256
      source: "qrc:/img/res/img/icon.png"
    }

    Rectangle {
      color: "#404244"
      width: aboutPage.width - logoImage.width - 60
      height: logoImage.height

      Label {
        padding: 12
        text: "Axion (Pre-alpha)\n\nThis program uses Qt 5.8"
        renderType: Text.NativeRendering
        font.family: "Segoe UI"
        font.bold: false
        font.italic: false
        font.pixelSize: 24
      }
    }
  }
}
