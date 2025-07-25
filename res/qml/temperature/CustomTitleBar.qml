// CustomTitleBar.qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/infinity_station/res/version.js" as VersionJS

Rectangle {
    id: titleBar
    
    // 公共属性
    property string titleName: qsTr("弓网红外温度")
    property string iconSource: "qrc:/infinity_station/res/icon/gw.ico"
    property color titleColor: "#2c3e50"
    property color infoColor: "#666"
    property alias radius: titleBar.radius
    property bool bMaximized: true
    
    // 信号
    signal minimizeClicked()
    signal maximizeClicked()
    signal closeClicked()
  
    color: "transparent"
    
    // 标题栏分割线
    Rectangle {
        anchors.bottom: parent.bottom
        width: parent.width
        height: 1
        color: "#e0e0e0"
    }

    // 主布局
    RowLayout {
        anchors.fill: parent
            
        // 应用图标
        // Image {
        //     id: appIcon
        //     Layout.leftMargin: 2
        //     Layout.alignment: Qt.AlignTop
        //     //anchors.top: parent.top    
        //     width: 18
        //     height: 20
        //     source: iconSource
        // }
        Item { Layout.fillWidth: true }
        // 标题文本
        Text {
            Layout.leftMargin: 5
            Layout.alignment: Qt.AlignVCenter
            text: titleName
            color: titleColor
            font.pixelSize: 14
            font.weight: Font.Medium
        }

        // // 弹性占位
        Item { Layout.fillWidth: true }

        // 版本信息区域
        RowLayout {
            Layout.alignment: Qt.AlignVCenter
            Text { text: "版本: " + VersionJS.getVersionInfo().version; color: "#666"; font.pixelSize: 12 }
            Text { text: "分支: " + VersionJS.getVersionInfo().branch; color: "#666"; font.pixelSize: 12 }
            Text { text: "提交Hash: " + VersionJS.getVersionInfo().commitHash; color: "#666"; font.pixelSize: 12 }
            Text { text: "编译时间: " + VersionJS.getVersionInfo().compileTime; color: "#666"; font.pixelSize: 12 }
        }

        // 弹性占位
        Item { Layout.fillWidth: true }


        Row {
            id: controlButtons
            Layout.rightMargin: 10
            height: parent.height

            // 最小化按钮
            Rectangle {
                width: 46
                height:  parent.height
                color: minimizeArea.containsMouse ? "#20000000" : "transparent"
                Text { anchors.centerIn: parent; text: "─"; color: "#2c3e50"; font.pixelSize: 14 }
                MouseArea {id: minimizeArea; anchors.fill: parent; hoverEnabled: true; 
                    onClicked: minimizeClicked() 
                }
            }

            // 最大化按钮
            Rectangle {
                width: 46
                height:  parent.height
                color: maximizeArea.containsMouse ? "#20000000" : "transparent"
                Text {anchors.centerIn: parent; text: bMaximized ? "❐" : "□" ;color: "#2c3e50"; font.pixelSize: 14 }
                MouseArea {id: maximizeArea; anchors.fill: parent; hoverEnabled: true
                    onClicked: maximizeClicked()
                }
            }

            // 关闭按钮
            Rectangle {
                width: 46
                height:  parent.height
                color: closeArea.containsMouse ? "#e81123" : "transparent"
                Text {anchors.centerIn: parent; text: "✕"; color: closeArea.containsMouse ? "white" : "#2c3e50"; font.pixelSize: 14 }
                MouseArea {id: closeArea; anchors.fill: parent; hoverEnabled: true
                    onClicked: closeClicked()
                }
            }
        
        }

        // 窗口拖动区域
        MouseArea {
            anchors.left: parent.left
            anchors.right: controlButtons.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            property point clickPos: "0,0"
            onPressed: (event) => {
                    clickPos = Qt.point(event.x, event.y)
                }
            onPositionChanged: (event) => {
                if (pressed) {
                    var delta = Qt.point(event.x - clickPos.x, event.y - clickPos.y)
                    var window = titleBar.Window.window
                    window.x += delta.x
                    window.y += delta.y
                }
            }
        }
    }
}