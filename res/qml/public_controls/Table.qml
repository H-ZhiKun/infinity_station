import QtQuick 
import QtQuick.Controls 

Rectangle {
    id: tableRoot
    property var headers: ["列1", "列2", "列3"]
    property var modelData: [] // [{col1:..., col2:..., col3:...}, ...]
    property int highlightIndex: -1

    width: parent ? parent.width : 400
    height: parent ? parent.height * 0.35 : 300
    color: "#f5f5f5"
    border.color: "#dddddd"
    radius: 4

    // 表头
    Rectangle {
        id: tableHeader
        width: parent.width
        height: 30
        color: "#2980b9"
        radius: 4

        Row {
            anchors.fill: parent
            Repeater {
                model: tableRoot.headers
                Rectangle {
                    width: parent.width / tableRoot.headers.length
                    height: parent.height
                    color: "transparent"
                    Text {
                        anchors.centerIn: parent
                        text: modelData
                        color: "white"
                        font.pixelSize: 14
                        font.bold: true
                    }
                }
            }
        }
    }

    // 数据列表
    ListView {
        id: dataListView
        anchors.top: tableHeader.bottom
        anchors.topMargin: 2
        anchors.left: parent.left
        anchors.leftMargin: 5
        anchors.right: parent.right
        anchors.rightMargin: 5
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        clip: true

        model: tableRoot.modelData

        delegate: Rectangle {
            width: dataListView.width - 10
            height: 28
            color: index === tableRoot.highlightIndex ? "#eaf6ff" : (index % 2 === 0 ? "#ffffff" : "#f5f5f5")
            radius: 2

            Row {
                anchors.fill: parent
                Repeater {
                    model: tableRoot.headers
                    Text {
                        width: parent.width / tableRoot.headers.length
                        anchors.verticalCenter: parent.verticalCenter
                        text: model[index][modelData]
                        font.pixelSize: 12
                        color: index === tableRoot.highlightIndex ? "#2980b9" : "#333333"
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: tableRoot.highlightIndex = index
            }
        }
    }
}