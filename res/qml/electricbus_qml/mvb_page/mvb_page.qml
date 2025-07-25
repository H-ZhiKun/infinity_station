import QtQuick 
import QtQuick.Controls 
import QtQuick.Layouts 
import QtQuick.Window 
import InfinityStation 1.0

Window {
    id: mvbWindow
    width: 1500
    height: 1000
    visible: true  // 添加此行确保窗口可见
    minimumWidth: 1500
    minimumHeight: 1000
    maximumWidth: 1500
    maximumHeight: 1000
    color: "#f0f0f0"
   

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        // 顶部控制区域
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            color: "#ffffff"
            border.color: "#ffffff"

            RowLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 15

                Label {
                    text: qsTr("MVB 监控")
                    font.pixelSize: 16
                    font.bold: true
                    color: "#2c3e50"
                }

                Item { Layout.fillWidth: true }

                Button {
                    id: connectButton
                    text: qsTr("连接")
                    implicitWidth: 80
                    implicitHeight: 32

                    background: Rectangle {
                        color: parent.down ? "#1976D2" :
                               parent.hovered ? "#2196F3" : "#2196F3"
                        radius: 4
                    }

                    contentItem: Text {
                        text: parent.text
                        color: "#ffffff"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                Button {
                    id: disconnectButton
                    text: qsTr("断开连接")
                    implicitWidth: 80
                    implicitHeight: 32

                    background: Rectangle {
                        color: parent.down ? "#c62828" :
                               parent.hovered ? "#f44336" : "#f44336"
                        radius: 4
                    }

                    contentItem: Text {
                        text: parent.text
                        color: "#ffffff"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }

        // 数据显示区域
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#ffffff"
            border.color: "#ffffff"
            radius: 4

            // 使用 RowLayout 来创建两列布局
            RowLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 10

                // 左侧数据显示
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "#ffffff"
                    border.color: "#e0e0e0"
                    border.width: 1
                    radius: 4

                    ScrollView {
                        anchors.fill: parent
                        anchors.margins: 10
                        clip: true

                        ListView {
                            width: parent.width
                            model: ListModel {
                                id: mvbModelLeft
                                ListElement { name: "TCMS生命信号"; value: "0" }
                                ListElement { name: "HVAC2通信状态"; value: "0" }
                                ListElement { name: "高压1状态字"; value: "0" }
                                ListElement { name: "高压1故障字"; value: "0" }
                                ListElement { name: "高压1电压值"; value: "0" }
                                ListElement { name: "高压1电流值"; value: "0" }
                                ListElement { name: "高压1电阻值"; value: "0" }
                                ListElement { name: "高压2状态字"; value: "0" }
                                ListElement { name: "高压2故障字"; value: "0" }
                                ListElement { name: "高压2电压值"; value: "0" }
                                ListElement { name: "高压2电流值"; value: "0" }
                                ListElement { name: "高压2电阻值"; value: "0" }
                            }

                            header: Rectangle {
                                width: parent.width
                                height: 40
                                color: "#ffffff"

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: 5
                                    spacing: 10

                                    Text {
                                        Layout.preferredWidth: parent.width * 0.7
                                        text: "参数名称 (系统1)"
                                        font.bold: true
                                    }
                                    Text {
                                        Layout.preferredWidth: parent.width * 0.3
                                        text: "数值"
                                        font.bold: true
                                    }
                                }
                            }

                            delegate: Rectangle {
                                width: parent.width
                                height: 36
                                color: index % 2 ? "#ffffff" : "#f8f8f8"
                                border.color: "#ffffff"

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: 5
                                    spacing: 10

                                    Text {
                                        Layout.preferredWidth: parent.width * 0.7
                                        text: name
                                        font.pixelSize: 14
                                    }
                                    Text {
                                        Layout.preferredWidth: parent.width * 0.3
                                        text: value
                                        font.pixelSize: 14
                                    }
                                }
                            }
                        }
                    }
                }

                // 右侧数据显示
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "#ffffff"
                    border.color: "#e0e0e0"
                    border.width: 1
                    radius: 4

                    ScrollView {
                        anchors.fill: parent
                        anchors.margins: 10
                        clip: true

                        ListView {
                            width: parent.width
                            model: ListModel {
                                id: mvbModelRight
                                ListElement { name: "HVAC1通信状态"; value: "0" }
                                ListElement { name: "HVAC1状态字1"; value: "0" }
                                ListElement { name: "HVAC1故障字1"; value: "0" }
                                ListElement { name: "HVAC1故障字2"; value: "0" }
                                ListElement { name: "HVAC1干燥机状态"; value: "0" }
                                ListElement { name: "HVAC1干燥机温度1"; value: "0" }
                                ListElement { name: "HVAC1干燥机温度2"; value: "0" }
                                ListElement { name: "HVAC2状态字1"; value: "0" }
                                ListElement { name: "HVAC2故障字1"; value: "0" }
                                ListElement { name: "HVAC2故障字2"; value: "0" }
                                ListElement { name: "HVAC2干燥机状态"; value: "0" }
                                ListElement { name: "HVAC2干燥机温度1"; value: "0" }
                                ListElement { name: "HVAC2干燥机温度2"; value: "0" }
                            }

                            header: Rectangle {
                                width: parent.width
                                height: 40
                                color: "#ffffff"

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: 5
                                    spacing: 10

                                    Text {
                                        Layout.preferredWidth: parent.width * 0.7
                                        text: "参数名称 (系统2)"
                                        font.bold: true
                                    }
                                    Text {
                                        Layout.preferredWidth: parent.width * 0.3
                                        text: "数值"
                                        font.bold: true
                                    }
                                }
                            }

                            delegate: Rectangle {
                                width: parent.width
                                height: 36
                                color: index % 2 ? "#ffffff" : "#f8f8f8"
                                border.color: "#ffffff"

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: 5
                                    spacing: 10

                                    Text {
                                        Layout.preferredWidth: parent.width * 0.7
                                        text: name
                                        font.pixelSize: 14
                                    }
                                    Text {
                                        Layout.preferredWidth: parent.width * 0.3
                                        text: value
                                        font.pixelSize: 14
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        // 底部信息区域
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 200
            color: "#ffffff"
            border.color: "#ffffff"
            radius: 4

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 10

                // 刷新时间控制组
                GridLayout {
                    Layout.fillWidth: true
                    columns: 4
                    columnSpacing: 20
                    rowSpacing: 10

                    Repeater {
                        model: [
                            { label: "HvRefreshTime", value: "32" },
                            { label: "LvFreshTime", value: "128" },
                            { label: "MpuFreshTime", value: "128" },
                            { label: "PmsFreshTime", value: "256" }
                        ]

                        Rectangle {
                            Layout.preferredWidth: 150
                            Layout.preferredHeight: 70
                            color: "#ffffff"
                            radius: 4

                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 5
                                spacing: 5

                                Text {
                                    text: modelData.label
                                    font.pixelSize: 14
                                    Layout.alignment: Qt.AlignHCenter
                                }
                                TextField {
                                    Layout.preferredWidth: 100
                                    Layout.alignment: Qt.AlignHCenter
                                    text: modelData.value
                                    font.pixelSize: 14
                                }
                            }
                        }
                    }
                }

                TextArea {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 70
                    readOnly: true
                    text: "程序开始运动，版本1.0.0.20230912\nGrpcPath: C:/path/to/service\nServer listening on 0.0.0.0:21300"
                    wrapMode: TextArea.Wrap
                    font.pixelSize: 14
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 20

                    Text { 
                        text: "程序版本:1.0.0.230912"
                        font.pixelSize: 14 
                    }
                    Item { Layout.fillWidth: true }
                    Text { 
                        text: "处理时间:2025 - 01 - 15 10:29:17"
                        font.pixelSize: 14 
                    }
                    Item { Layout.fillWidth: true }
                    Text { 
                        text: "检测状态:X"
                        font.pixelSize: 14 
                    }
                }
            }
        }
    }
}