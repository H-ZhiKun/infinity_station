// CustomTitleBar.qml
import QtQuick 
import QtQuick.Controls 
import QtQuick.Layouts 
import "qrc:/infinity_station/res/version.js" as VersionJS

Rectangle {
    id: titleBar
    
    // 公共属性
    property string titleName: qsTr("")
    property string iconSource: "qrc:/infinity_station/res/icon/tggw.png"
    property color titleColor: "#ffffff"
    property color infoColor: "#ffffff"
    property alias radius: titleBar.radius
    property bool bMaximized: true
    
    // 信号
    signal minimizeClicked()
    signal maximizeClicked()
    signal closeClicked()
  
    color: "#1a2b3c"  // 深墨蓝色
    
    // 标题栏分割线
    Rectangle {
        anchors.bottom: parent.bottom
        width: parent.width
        height: 1
        color: "#2c3e50"
    }

    // 主布局
    RowLayout {
        anchors.fill: parent
            
        // 应用图标
        Image {
            Layout.preferredWidth: 150  // 适当放大图标
            Layout.preferredHeight: 40
            Layout.leftMargin: 12
            Layout.alignment: Qt.AlignVCenter
            source: iconSource
            sourceSize: Qt.size(150, 40)  // 确保清晰显示
            fillMode: Image.PreserveAspectFit
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: console.log("App icon clicked")
            }
        }

        // 标题文本
        Text {
            text: titleName
            color: titleColor
            font { pixelSize: 14; weight: Font.Medium }
        }

        // 弹性占位 + 拖动区域
        Item {
            Layout.fillWidth: true
            MouseArea {
                anchors.fill: parent
                property point clickPos
                onPressed: (mouse) => clickPos = Qt.point(mouse.x, mouse.y)
                onPositionChanged: (mouse) => {
                    if (pressed) {
                        var delta = Qt.point(mouse.x - clickPos.x, mouse.y - clickPos.y)
                        titleBar.Window.window.x += delta.x
                        titleBar.Window.window.y += delta.y
                    }
                }
            }
        }

        // 版本信息区域
        RowLayout {
            Layout.alignment: Qt.AlignVCenter
            Text { text: "版本: " + VersionJS.getVersionInfo().version; color: "#ffffff"; font.pixelSize: 12 }
            Text { text: "分支: " + VersionJS.getVersionInfo().branch; color: "#ffffff"; font.pixelSize: 12 }
            Text { text: "提交Hash: " + VersionJS.getVersionInfo().commitHash; color: "#ffffff"; font.pixelSize: 12 }
            Text { text: "编译时间: " + VersionJS.getVersionInfo().compileTime; color: "#ffffff"; font.pixelSize: 12 }
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
                color: minimizeArea.containsMouse ? "#20e0dddd" : "transparent"
                Text { anchors.centerIn: parent; text: "─"; color: "#ffffff"; font.pixelSize: 14 }
                MouseArea {id: minimizeArea; anchors.fill: parent; hoverEnabled: true; 
                    onClicked: minimizeClicked() 
                }
            }

            // 最大化按钮
            Rectangle {
                width: 46
                height:  parent.height
                color: maximizeArea.containsMouse ? "#20c4c0c0" : "transparent"
                Text {anchors.centerIn: parent; text: bMaximized ? "❐" : "□" ;color: "#ffffff"; font.pixelSize: 14 }
                MouseArea {id: maximizeArea; anchors.fill: parent; hoverEnabled: true
                    onClicked: maximizeClicked()
                }
            }

            // 关闭按钮
            Rectangle {
                width: 46
                height:  parent.height
                color: closeArea.containsMouse ? "#e81123" : "transparent"
                Text {anchors.centerIn: parent; text: "✕"; color: closeArea.containsMouse ? "white" : "#ffffff"; font.pixelSize: 14 }
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