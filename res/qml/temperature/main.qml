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
    width: 800
    height: 1000
    minimumWidth: 800
    minimumHeight: 1000
    maximumWidth: 1280
    maximumHeight: 820
    title: qsTr("应用控制中心")
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
            titleName: qsTr("弓网红外温度")
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
   
    Loader {
        anchors.fill: parent
        anchors {
            topMargin: 37     // 上边距37像素
            bottomMargin: 300 // 下边距100像素
            leftMargin: 1     // 左边距1像素
            rightMargin: 1    // 右边距1像素
        }

        id: ir_video_page
        source: "irvideo_page.qml"
    }
    CustomLabel
    {
        id: maxtemplabel
        anchors {
            top: ir_video_page.bottom
            //bottom: parent.bottom
            left: parent.left
            right: parent.right
            //height: 20
            topMargin: 4     // 上边距38像素
            //bottomMargin: 200 // 下边距100像素
            leftMargin: 1     // 左边距1像素
            rightMargin: 1    // 右边距1像素
        }
        height: 40
        fronttext: "max:"
        unit: "°C"
        value: 1.2
    }

    Rectangle {
        width: parent.width
        anchors.top: maxtemplabel.bottom
        height: 150
        color: "white"              // 背景色
        border.color: "#888888"     // 边框颜色
        border.width: 1
        radius: 4                   // 可选：圆角

        TextEdit {
            id: noteInput
            anchors.fill: parent
            anchors.margins: 6
            // width: parent.width
            // anchors.top: maxtemplabel.bottom
            // height: 20
            text: logInfo
            wrapMode: TextEdit.WordWrap
            font.pixelSize: 14
            color: "black"
            selectByMouse: true
            focus: true
        }
    }

    Connections {
        target: qmlCommunication
        function onMaxtemp_recvFromCpp(frameData) {
           maxtemplabel.value = Number(frameData)
        }
    }

    Connections {
            target: qmlCommunication
            function onDisplaylog_sendFromCpp(frameData) {
                //console.log("QmlPage have init ", QmlCommunication.ForQmlSignals[modelData.modelName])
                logInfo = frameData.toString()
            }
        }
    
    // Rectangle {
    //         id: statusBar 
    //         anchors.bottom: parent.bottom
    //         anchors.bottomMargin: 32 
    //         width: parent.width
    //         height: 32
    //     Rectangle {
    //         anchors.top: parent.top
    //         width: parent.width
    //         height: 1
    //         color: "#e0e0e0"
    //     }
    //     RowLayout
    //     {
    //         //anchors.fill: parent
    //         width: parent.width
    //         height: parent.height
    //         spacing: 1
    //         CustomState  {
    //             id: stateLabel
    //             width: 120
    //             height: parent.height
    //             stateName: "State: " 
    //             bState: true
    //         }
    //         CustomState  {
    //             id: stateLabel2
    //             width: 120
    //             height: parent.height
    //             stateName: "State: " 
    //             bState: false
    //         }
    //         CustomState  {
    //             id: stateLabel3
    //             width: 120
    //             height: parent.height
    //             stateName: "State: " 
    //             bState: true
    //         }        
    //         CustomState  {
    //             id: stateLabel4
    //             width: 120
    //             height: parent.height
    //             stateName: "State: " 
    //             bState: false
    //         }
    //         Item {
    //             Layout.fillWidth: true
    //         }
    //     }

    // }
}
