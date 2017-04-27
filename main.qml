import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3


ApplicationWindow {
  visible: true
  width: 640
  height: 760
  title: qsTr("Axion")


  StackView {
    id: mainView
    anchors.fill: parent
    initialItem: Login {}
  }
}
