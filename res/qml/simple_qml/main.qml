import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects
import InfinityStation 1.0
import QtQuick.Window
import QtCharts 
import "qrc:/infinity_station/res/version.js" as VersionJS

ApplicationWindow {
    id: root
    visible: true
    width: 480
    height: 640
    // minimumWidth: 800
    // minimumHeight: 1000
    // maximumWidth: 1280
    // maximumHeight: 820
    title: qsTr("衍衍数据采集")
    flags: Qt.Window | Qt.FramelessWindowHint  // 无边框设置

    property string logInfo: ""

    // 添加主窗口容器
    Rectangle {
        id: windowFrame
        anchors.fill: parent
        color: "#f5f7fa"
        radius: 20  // 圆角
            
        CustomTitleBar {
            id: customTitleBar
            anchors.top: parent.top  
            width: parent.width
            height: 36
            titleName: qsTr("衍衍数据采集")
            radius: windowFrame.radius
            
            onMinimizeClicked: root.showMinimized()
            onMaximizeClicked: {
                if (root.visibility === Window.Maximized) {
                    root.showNormal()
                    bMaximized = true
                } else {
                    root.showMaximized()
                    bMaximized = false
                }
            }
            onCloseClicked: Qt.quit()
        }

    }
   
}
