import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtCharts
import InfinityStation 1.0

Window {
    id: root
    width: 800
    height: 600
    title: "燃弧监测"
    visible: true

    // 数据模型
    property var voltageData: []
    property int maxDataPoints: 100
    property double threshold: 7.5 // 默认阈值

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        // 标题和实时数据显示区域
        Rectangle {
            Layout.fillWidth: true
            height: 60
            color: "#f5f5f5"
            radius: 5

            RowLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 20

                Label {
                    text: "实时电压值:"
                    font.pixelSize: 16
                }

                Label {
                    id: voltageLabel
                    text: "0.00 V"
                    font.pixelSize: 20
                    font.bold: true
                    color: Number(text.split(" ")[0]) > threshold ? "red" : "green"
                }

                Rectangle {
                    Layout.alignment: Qt.AlignRight
                    width: 12
                    height: 12
                    radius: 6
                    color: Number(voltageLabel.text.split(" ")[0]) > threshold ? "red" : "green"
                }
                // 燃弧测试按钮
                Button {
                    id: arcTestButton
                    text: "燃弧测试"
                    Layout.alignment: Qt.AlignLeft
                    onClicked: {
                        qmlCommunication.behaviorFromQml(QmlCommunication.QmlActions.TestButton, "test")
                    }
                }
            }
        }

        // 图表显示区域
        ChartView {
            id: chartView
            Layout.fillWidth: true
            Layout.fillHeight: true
            antialiasing: true
            theme: ChartView.ChartThemeDark
            animationOptions: ChartView.NoAnimation  // 禁用动画以便及时更新
            
            ValueAxis {
                id: axisY
                min: 0
                max: threshold * 1.5
                tickCount: 5
                labelFormat: "%.2f"
                titleText: "电压 (V)"
            }

            ValueAxis {
                id: axisX
                min: 0
                max: maxDataPoints
                tickCount: 5
                labelFormat: "%.0f"
                titleText: "采样点"
            }

            LineSeries {
                id: voltageSeries
                name: "电压曲线"
                axisX: axisX
                axisY: axisY
                width: 2
                color: "white"  // 设置为白色以确保在暗色主题下可见
                visible: true   // 确保系列可见
            }

            LineSeries {
                id: thresholdLine
                name: "阈值线"
                axisX: axisX
                axisY: axisY
                color: "red"
                width: 1
                style: Qt.DashLine
            }
        }
    }

    Connections {
        target: qmlCommunication
        function onarc_pageFromCpp(data) {
            try {
                // 解析接收到的数据
                let voltage = data.voltage
                //let device = pageData.device
                let timestamp = data.timestamp

                // 更新电压标签显示
                voltageLabel.text = voltage.toFixed(2) + " V"

                // 更新电压数据数组
                voltageData.push(voltage)
                if (voltageData.length > maxDataPoints) {
                    voltageData.shift() // 移除最老的数据点
                }

                // 更新图表
                updateChart()

            } catch (e) {

            }
        }
    }

    // 添加更新图表的函数
    function updateChart() {
  
        // 清除现有数据点
        voltageSeries.clear()
        
        // 添加新的数据点
        for (let i = 0; i < voltageData.length; i++) {
            voltageSeries.append(i, voltageData[i])
        }

        // 更新阈值线
        thresholdLine.clear()
        thresholdLine.append(0, threshold)
        thresholdLine.append(maxDataPoints, threshold)

        // 根据当前电压值调整Y轴范围
        let maxVoltage = Math.max(...voltageData, threshold)
        axisY.max = maxVoltage * 1.2 // 留出20%的余量
        
        // 强制更新图表
        chartView.update()
    }

    Component.onCompleted: {
        // 初始化阈值线
        thresholdLine.clear()
        thresholdLine.append(0, threshold)
        thresholdLine.append(maxDataPoints, threshold)

        // 初始化电压数据数组
        voltageData = []
    }
}