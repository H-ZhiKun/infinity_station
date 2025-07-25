// CustomTitleBar.qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "qrc:/infinity_station/res/version.js" as VersionJS

Rectangle {
    id: titleBar
    
    // 公共属性
    property string titleName: qsTr("衍衍数据采集")
    property string iconSource: "qrc:/infinity_station/res/icon/gw.ico"
    property color titleColor: "#2c3e50"
    property color infoColor: "#666"
    property alias radius: titleBar.radius
    property bool bMaximized: true
    
    property int breakpointWidth: 800
    // 信号
    signal minimizeClicked()
    signal maximizeClicked()
    signal closeClicked()
  
    color: "transparent"

    // 标题栏分割线
    // Rectangle {
    //     anchors.bottom: titlebottom
    //     width: parent.width
    //     height: 1
    //     color: "#e0e0e0"
    // }

    Rectangle {
        id: divider
        width: parent.width
        height: 1
        color: "#e0e0e0"
        
        // 动态计算位置
        property real targetY: {
            if (titleBar.width >= breakpointWidth) {
                return titleBar.height - height;
            } else {
                // 计算第二行内容底部位置
                // const firstRowHeight = layout.children[0].height;
                // return firstRowHeight + (layout.spacing || 0);
                return titleBar.height * 2 - height;
            }
        }
        
        y: targetY
        Behavior on y { NumberAnimation { duration: 150 } }
    }
    


    // 主布局
    ColumnLayout {
        id: layout
        width: parent.width
        spacing: 5
            
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
               // 第一行：标题和控制按钮
        RowLayout {
            Layout.fillWidth: true
            Layout.topMargin: 5
            
            // 标题文本
            Text {
                id: titleText
                Layout.leftMargin: 15
                Layout.alignment: Qt.AlignVCenter
                text: titleName
                color: titleColor
                font.pixelSize: 18
                font.weight: Font.Bold
                elide: Text.ElideRight
                Layout.maximumWidth: parent.width * 0.5
            }
            
            // 弹性占位
            Item { Layout.fillWidth: true }
            
            // 版本信息区域 - 仅在宽屏时显示
            RowLayout {
                id: versionInfoRow
                Layout.alignment: Qt.AlignVCenter
                visible: titleBar.width >= breakpointWidth
                
                Text { 
                    text: "版本: " + VersionJS.getVersionInfo().version
                    color: infoColor
                    font.pixelSize: 12 
                }
                Text { 
                    text: "分支: " + VersionJS.getVersionInfo().branch
                    color: infoColor
                    font.pixelSize: 12 
                }
                Text { 
                    text: "提交Hash: " + VersionJS.getVersionInfo().commitHash
                    color: infoColor
                    font.pixelSize: 12 
                }
                Text { 
                    text: "编译时间: " + VersionJS.getVersionInfo().compileTime
                    color: infoColor
                    font.pixelSize: 12 
                }
            }
            
            // 弹性占位
            Item { Layout.fillWidth: titleBar.width >= breakpointWidth }
            
            // 控制按钮
            Row {
                id: controlButtons
                Layout.rightMargin: 10
                height: 30
                spacing: 1

                // 最小化按钮
                Rectangle {
                    width: 46
                    height: parent.height
                    color: minimizeArea.containsMouse ? "#e0e0e0" : "transparent"
                    radius: 3
                    Text { 
                        anchors.centerIn: parent
                        text: "─"
                        color: "#2c3e50"
                        font.pixelSize: 14
                        font.bold: true
                    }
                    MouseArea {
                        id: minimizeArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: minimizeClicked() 
                    }
                }

                // 最大化按钮
                Rectangle {
                    width: 46
                    height: parent.height
                    color: maximizeArea.containsMouse ? "#e0e0e0" : "transparent"
                    radius: 3
                    Text {
                        anchors.centerIn: parent
                        text: bMaximized ? "❐" : "□"
                        color: "#2c3e50"
                        font.pixelSize: 14
                    }
                    MouseArea {
                        id: maximizeArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: maximizeClicked()
                    }
                }

                // 关闭按钮
                Rectangle {
                    width: 46
                    height: parent.height
                    color: closeArea.containsMouse ? "#e81123" : "transparent"
                    radius: 3
                    Text {
                        anchors.centerIn: parent
                        text: "✕"
                        color: closeArea.containsMouse ? "white" : "#2c3e50"
                        font.pixelSize: 14
                        font.bold: true
                    }
                    MouseArea {
                        id: closeArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: closeClicked()
                    }
                }
            }
        }
        
        // 第二行：版本信息（窄屏时显示）
        RowLayout {
            id: versionInfoSecondRow
            Layout.fillWidth: true
            Layout.bottomMargin: 5
            Layout.leftMargin: 10
            visible: titleBar.width < breakpointWidth
            // spacing: 15
            
            Text { 
                text: "版本: " + VersionJS.getVersionInfo().version
                color: infoColor
                font.pixelSize: 12 
            }
            Text { 
                text: "分支: " + VersionJS.getVersionInfo().branch
                color: infoColor
                font.pixelSize: 12 
            }
            Text { 
                text: "提交: " + VersionJS.getVersionInfo().commitHash.substring(0, 7)
                color: infoColor
                font.pixelSize: 12 
            }
            Text { 
                text: "编译: " + VersionJS.getVersionInfo().compileTime.substring(0, 10)
                color: infoColor
                font.pixelSize: 12 
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