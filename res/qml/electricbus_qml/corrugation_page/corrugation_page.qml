import QtQuick 
import QtQuick.Controls 
import QtQuick.Layouts 
import QtCharts 
import InfinityStation 1.0

Window {
    visible: true
    width: 800
    height: 600
    title: "曲线监控"

    Rectangle {
        anchors.fill: parent
        color: "#f0f0f0"

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 10

            // 顶部按钮布局
            Rectangle {
                Layout.fillWidth: true
                height: 50
                color: "white"
                radius: 4

                RowLayout {
                    anchors.centerIn: parent
                    spacing: 20

                    Button {
                        text: "开始"
                        implicitWidth: 100
                        implicitHeight: 40
                        onClicked: {
                            // 开始逻辑
                        }
                    }

                    Button {
                        text: "暂停"
                        implicitWidth: 100
                        implicitHeight: 40
                        onClicked: {
                            // 暂停逻辑
                        }
                    }

                    Button {
                        text: "结束"
                        implicitWidth: 100
                        implicitHeight: 40
                        onClicked: {
                            // 结束逻辑
                        }
                    }
                }
            }

            // 第一个曲线图
            ChartView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                title: "曲线图 1"
                antialiasing: true
                legend.visible: true

                LineSeries {
                    name: "数据曲线1"
                    // 示例数据点
                    XYPoint { x: 0; y: 0 }
                    XYPoint { x: 1; y: 2 }
                    XYPoint { x: 2; y: 3 }
                    XYPoint { x: 3; y: 1 }
                }
            }

            // 第二个曲线图
            ChartView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                title: "曲线图 2"
                antialiasing: true
                legend.visible: true

                LineSeries {
                    name: "数据曲线2"
                    // 示例数据点
                    XYPoint { x: 0; y: 1 }
                    XYPoint { x: 1; y: 1 }
                    XYPoint { x: 2; y: 2 }
                    XYPoint { x: 3; y: 3 }
                }
            }
        }
    }
}
