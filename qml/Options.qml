import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.11


Page {
  id: optionsPage

  ColumnLayout {
    anchors.top: parent.top
    spacing: 8

    RowLayout {
      Layout.margins: 8
      spacing: 8

      Label {
        text: "UI scale"
        font.pixelSize: config.textSize
      }

      ComboBox {
        property int startIndex: config.uiScale
        id: uiScaleBox
        model: ["Small", "Medium", "Large"]
        font.pixelSize: config.textSize
        currentIndex: startIndex
        onCurrentIndexChanged: config.set_ui_scale(currentIndex)
      }
    }
  }
}
