import QtQuick 
import QtQuick.Controls 
import QtQuick.Layouts 
import Qt.labs.platform

Item { 
    id: stateroot
    property bool bState: true
    property string stateName: "test"
    RowLayout {
        anchors.fill: parent
        //spacing: 20
        Label {
            Layout.leftMargin: 5
            text: stateroot.stateName
            font.pixelSize: 16
        }
        Item {
            Layout.fillWidth: true
        }
        Label {
            // Layout.rightMargin: 5
            text: stateroot.bState ? "√" :  "×"; 
            color: stateroot.bState ? "#21f07e" : "#f02121"
            font.pixelSize: 16
        }
        Item {
            Layout.fillWidth: true
        }
    }
    Rectangle {
        anchors.right: parent.right
        width: 1
        height: parent.height
        //Layout.rightMargin: 1
        color: "#e0e0e0"
    }

}