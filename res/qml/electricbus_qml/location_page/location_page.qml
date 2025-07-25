import QtQuick 
import QtQuick.Controls 
import QtQuick.Layouts 
import InfinityStation 1.0

ApplicationWindow {
    id: root
    visible: true
    width: 640
    height: 480
    minimumWidth: 640
    minimumHeight: 480
    maximumWidth: 640
    maximumHeight: 480
    title: qsTr("定位日志查看器")

    // 添加属性
    property int maxLogCount: 1000  // 最大日志条数
    property string currentStatus: "就绪"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        // 顶部工具栏
        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            Label {
                text: qsTr("定位日志")
                font.pixelSize: 14
                font.bold: true
            }

            Item { Layout.fillWidth: true }  // 弹簧

            Button {
                text: qsTr("清除日志")
                onClicked: clearLogs()
            }
        }

        // 日志显示区域
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            border.color: "lightgray"
            border.width: 1

            ListView {
                id: logView
                anchors.fill: parent
                anchors.margins: 5
                anchors.rightMargin: scrollBar.width
                model: logModel
                clip: true
                spacing: 5
                
                delegate: Rectangle {
                    width: ListView.view.width
                    height: logContent.implicitHeight + 10
                    color: index % 2 ? "#f8f8f8" : "white"
                    
                    // 添加鼠标悬停效果
                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: parent.color = "#e0e0e0"  // 鼠标悬停时的颜色
                        onExited: parent.color = index % 2 ? "#f8f8f8" : "white"  // 恢复原来的颜色
                    }
                    
                    ColumnLayout {
                        id: logContent
                        width: parent.width
                        anchors.centerIn: parent
                        spacing: 5
                        
                        RowLayout {
                            width: parent.width
                            spacing: 10
                            
                            Label {
                                text: timestamp
                                font.pixelSize: 12
                                color: "#666666"
                                Layout.preferredWidth: 80
                            }
                            
                            Label {
                                text: logMessage
                                font.pixelSize: 12
                                color: getLogColor(type || "info")
                                Layout.fillWidth: true
                                wrapMode: Text.WordWrap  // 确保文本换行
                                elide: Text.ElideNone    // 禁用省略
                            }
                        }
                    }
                }
            }

            
            // 添加鼠标悬停效果
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onEntered: parent.ListView.view.currentIndex = index
            }

            ScrollBar {
                id: scrollBar
                size: 0.3
                anchors {
                    top: parent.top
                    right: parent.right
                    bottom: parent.bottom
                }
                active: true
                orientation: Qt.Vertical
                policy: ScrollBar.AlwaysOn
            }
        }

        // 状态栏
        Rectangle {
            Layout.fillWidth: true
            height: 30
            color: "lightgray"
            
            Label {
                id: statusLabel
                anchors.fill: parent
                anchors.margins: 5
                text: qsTr("状态：") + root.currentStatus
                verticalAlignment: Text.AlignVCenter
            }
        }
    }

    // 日志模型
    ListModel {
        id: logModel
    }

    // 日志管理函数
    function addLog(message, type = "info") {
        if (logModel.count >= maxLogCount) {
            logModel.remove(0)  // 移除最旧的日志
        }

        logModel.append({
            timestamp: new Date().toLocaleTimeString(Qt.locale(), "hh:mm:ss"),
            logMessage: message,
            type: type
        })
        
        logView.positionViewAtEnd()
        updateStatus(message)
    }

    function clearLogs() {
        logModel.clear()
        updateStatus("日志已清除")
    }

    function updateStatus(message) {
        root.currentStatus = message
    }

    function getLogColor(type) {
        switch(type) {
            case "error": return "#FF4444"
            case "warning": return "#FFBB33"
            case "success": return "#99CC00"
            default: return "#000000"
        }
    }

    // C++信号连接
    Connections {
        target: qmlCommunication  //对象
        
        // 接收具体页面的 callFromCpp 信号
       function onLocationPageSignal(data) {
            console.log("Received data:", data)  // 添加调试信息
            
            //zdt备注：moveDistance已修改为kiloMeter
            
            // 直接处理接收到的数据对象
            if (typeof data === "object") {
                let stationName = data.stationName || ""
                let speed = data.speed || 0
                let moveDistance = data.moveDistance || 0
                
                let logMessage = `站点: ${stationName}, 速度: ${speed.toFixed(2)}, 距离: ${moveDistance.toFixed(2)}`
                addLog(logMessage)
            } else {
                // 如果收到的不是对象，直接显示
                addLog(data.toString())
            }
        }
    }


    // 组件初始化
    Component.onCompleted: {
        updateStatus("系统初始化完成")
    }
}

