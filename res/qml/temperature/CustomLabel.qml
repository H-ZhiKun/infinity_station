import QtQuick 
import QtQuick.Controls 
import QtQuick.Layouts 
import Qt.labs.platform

Item { 
    id: root
    visible: true
    //anchors.fill: parent
    property string fronttext: "max:"
    property string unit: "°C"
    property real  value: 1.0
    
    RowLayout {
        anchors.fill: parent
        spacing: 10
        Item {
            Layout.fillWidth: true
        }
        Label {
            font.pixelSize: 26
            color: "#cb3b49"
            text: root.fronttext  + Number(value).toFixed(1) + root.unit
        }
        Item {
            Layout.fillWidth: true
        }
    }
}
// }
