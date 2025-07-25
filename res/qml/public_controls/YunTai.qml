import QtQuick 
import QtQuick.Controls 
import QtQuick.Layouts 
import Qt.labs.platform 1.1

Item {
    id: root
    width: 420
    height: 650
    
    signal commandSent(string command, int speed)

    Rectangle {
        anchors.fill: parent
        color: "#ffffff"
        radius: 10
        border.color: "#ddd"
        border.width: 1

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 15
            spacing: 20

            // 方向盘控制区
            GroupBox {
                title: "方向控制"
                Layout.fillWidth: true
                background: Rectangle {
                    color: "transparent"
                    border.color: "#ddd"
                    radius: 5
                }

                GridLayout {
                    anchors.fill: parent
                    columns: 3
                    rows: 3
                    columnSpacing: 5
                    rowSpacing: 5

                    // 上
                    DirectionButton {
                        Layout.row: 0; Layout.column: 1
                        text: "↑"
                        onClicked: sendCommand("up")
                    }
                    
                    // 左
                    DirectionButton {
                        Layout.row: 1; Layout.column: 0
                        text: "←"
                        onClicked: sendCommand("left")
                    }
                    
                    // 停止
                    DirectionButton {
                        Layout.row: 1; Layout.column: 1
                        text: "■"
                        backgroundColor: "#e74c3c"
                        onClicked: sendCommand("stop")
                    }
                    
                    // 右
                    DirectionButton {
                        Layout.row: 1; Layout.column: 2
                        text: "→"
                        onClicked: sendCommand("right")
                    }
                    
                    // 下
                    DirectionButton {
                        Layout.row: 2; Layout.column: 1
                        text: "↓"
                        onClicked: sendCommand("down")
                    }
                    
                    // 左上
                    DirectionButton {
                        Layout.row: 0; Layout.column: 0
                        text: "↖"
                        onClicked: sendCommand("up-left")
                    }
                    
                    // 右上
                    DirectionButton {
                        Layout.row: 0; Layout.column: 2
                        text: "↗"
                        onClicked: sendCommand("up-right")
                    }
                    
                    // 左下
                    DirectionButton {
                        Layout.row: 2; Layout.column: 0
                        text: "↙"
                        onClicked: sendCommand("down-left")
                    }
                    
                    // 右下
                    DirectionButton {
                        Layout.row: 2; Layout.column: 2
                        text: "↘"
                        onClicked: sendCommand("down-right")
                    }
                }
            }

            // 镜头控制区
            GroupBox {
                title: "镜头控制"
                Layout.fillWidth: true
                background: Rectangle {
                    color: "transparent"
                    border.color: "#ddd"
                    radius: 5
                }

                RowLayout {
                    anchors.fill: parent
                    spacing: 10

                    LensControlButton {
                        text: "聚焦+"
                        icon: StandardIcon.Add
                        onClicked: sendCommand("focus-plus")
                    }

                    LensControlButton {
                        text: "调焦+"
                        icon: StandardIcon.ZoomIn
                        onClicked: sendCommand("zoom-plus")
                    }

                    LensControlButton {
                        text: "自动聚焦"
                        icon: StandardIcon.Refresh
                        backgroundColor: "#2ecc71"
                        onClicked: sendCommand("auto-focus")
                    }

                    LensControlButton {
                        text: "聚焦-"
                        icon: StandardIcon.Remove
                        onClicked: sendCommand("focus-minus")
                    }

                    LensControlButton {
                        text: "调焦-"
                        icon: StandardIcon.ZoomOut
                        onClicked: sendCommand("zoom-minus")
                    }
                }
            }

            // 速度等级
            GroupBox {
                title: "速度等级"
                Layout.fillWidth: true
                background: Rectangle {
                    color: "transparent"
                    border.color: "#ddd"
                    radius: 5
                }

                ButtonGroup {
                    id: speedGroup
                }

                RowLayout {
                    anchors.fill: parent
                    spacing: 10

                    Repeater {
                        model: 3
                        RadioButton {
                            text: qsTr("%1级".arg(index + 1))
                            checked: index === 0
                            ButtonGroup.group: speedGroup
                            
                            contentItem: Text {
                                text: parent.text
                                font.pixelSize: 12
                                color: parent.checked ? "#3498db" : "#333"
                                verticalAlignment: Text.AlignVCenter
                                leftPadding: parent.indicator.width + parent.spacing
                            }
                            
                            indicator: Rectangle {
                                implicitWidth: 16
                                implicitHeight: 16
                                radius: 8
                                border.color: parent.checked ? "#3498db" : "#999"
                                border.width: 1
                                
                                Rectangle {
                                    anchors.fill: parent
                                    anchors.margins: 4
                                    radius: 4
                                    color: parent.parent.checked ? "#3498db" : "transparent"
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // 自定义方向按钮组件
    component DirectionButton: Rectangle {
        property alias text: label.text
        property color backgroundColor: "#3498db"
        signal clicked

        width: 60
        height: 60
        radius: 30
        color: mouseArea.containsPress ? Qt.darker(backgroundColor, 1.2) : backgroundColor
        
        Label {
            id: label
            anchors.centerIn: parent
            font.pixelSize: 24
            color: "white"
        }
        
        MouseArea {
            id: mouseArea
            anchors.fill: parent
            onClicked: parent.clicked()
        }
        
        layer.enabled: true
    }

    // 自定义镜头控制按钮组件
component LensControlButton: Rectangle {
    property string text
    property alias icon: btn.icon
    property color backgroundColor: "#3498db"
    signal clicked

    implicitWidth: 55
    implicitHeight: 30
    radius: 5
    color: mouseArea.containsPress ? Qt.darker(backgroundColor, 1.2) : backgroundColor

Button {
    id: btn
    anchors.fill: parent
    flat: true
    text: parent.text
    onClicked: parent.clicked()
    background: Rectangle {
        color: "transparent"
        radius: 5
    }
    contentItem: Column {
        width: parent.width
        height: parent.height
        spacing: 5

        Item {
            width: 25
            height: 20
            anchors.horizontalCenter: parent.horizontalCenter

            Image {
                anchors.centerIn: parent
                source: btn.icon ? btn.icon.source : ""
                sourceSize.width: 16
                sourceSize.height: 16
            }
        }

        Text {
            text: btn.text
            font.pixelSize: 10
            color: "white"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width
            elide: Text.ElideRight
        }
    }
}

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: parent.clicked()
    }
}

    function sendCommand(command) {
        var speed = speedGroup.checkedButton ? speedGroup.checkedButton.text.replace("级", "") : 1;
        console.log("发送命令:", command, "速度:", speed);
        root.commandSent(command, speed);
    }
}