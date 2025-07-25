import QtQuick 
import QtQuick.Controls 
import QtQuick.Layouts 
import QtQuick.Window 
import Qt.labs.platform
import InfinityStation 1.0

Window {
    visible: true
    width: 1024
    height: 600
    minimumWidth: 1024
    minimumHeight: 600
    maximumWidth: 1024
    maximumHeight: 600
    title: "视频预览"

    property real frameRate: 0.0
    property bool isStreamMode: true

    Rectangle {
        anchors.fill: parent
        color: "#404040"

        Rectangle {
            id: videoArea
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                bottom: bottomBar.top
            }
            color: "#2a2a2a"

            Item {
                anchors.fill: parent

                // 流模式视频显示
                ImagePainter {
                    id: videoPainter
                    anchors.fill: parent
                    visible: showVideoCheckbox.checked

                    Connections {
                        target: qmlCommunication
                        function onvideo_pageFromCpp(frameData) {
                            try {
                                if (videoPainter.visible) {
                                    if (frameData) {
                                        let image = frameData["img"]
                                        let cameraName = frameData["camera_name"]
                                        videoPainter.updateImage(image)
                                        cameraLabel.text = cameraName
                                        fpsTimer.frameCount++
                                        console.info("更新视频帧, 相机:", cameraName)
                                    } else {
                                        console.error("收到空的视频帧数据")
                                    }
                                }
                            } catch (e) {
                                console.error("处理视频帧错误:", e)
                            }
                        }
                    }
                }

                Label {
                    id: cameraLabel
                    anchors.left: parent.left
                    anchors.leftMargin: 20
                    anchors.top: parent.top
                    anchors.topMargin: 20
                    color: "white"
                    font.pixelSize: 18
                    text: ""
                    visible: showVideoCheckbox.checked
                }
            }

            Timer {
                id: fpsTimer
                interval: 1000
                repeat: true
                running: showVideoCheckbox.checked
                property int frameCount: 0
                onTriggered: {
                    frameRate = frameCount
                    frameCount = 0
                }
            }

            Text {
                anchors.centerIn: parent
                color: "white"
                text: "视频显示错误"
                visible: false
                font.pixelSize: 16
                property bool hasError: false

                Connections {
                    target: videoPainter
                    function onError(errorString) {
                        parent.visible = true
                        parent.text = errorString
                        parent.hasError = true
                    }
                }
            }
        }

        Rectangle {
            id: bottomBar
            height: 80
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            color: "#f0f0f0"

            RowLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 10

                Button {
                    text: "流模式"
                    implicitWidth: 120
                    implicitHeight: 30
                    enabled: false
                }

                Rectangle {
                    width: 1
                    height: 30
                    color: "#cccccc"
                }

                CheckBox {
                    id: showVideoCheckbox
                    text: "显示"
                    checked: true
                    onCheckedChanged: {
                        console.log("视频显示状态:", checked ? "显示" : "隐藏")
                    }
                }

                Item { Layout.fillWidth: true }

                Label {
                    text: "帧率: " + Math.round(frameRate) + " fps"
                }

                Button {
                    text: "保存图片"
                    implicitWidth: 80
                    implicitHeight: 30
                }

                Button {
                    text: "参数配置"
                    implicitWidth: 80
                    implicitHeight: 30
                }

                CheckBox {
                    text: "录像"
                }
            }
        }
    }

    Rectangle {
        id: statusBar
        height: 25
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        color: "#e0e0e0"

        RowLayout {
            anchors.fill: parent
            anchors.margins: 5
            spacing: 10

            Label {
                text: "状态: " + (videoPainter.visible ? "正在播放流" : "流已暂停")
            }

            Item { Layout.fillWidth: true }

            Label {
                text: "分辨率: 1920x1080"
            }
        }
    }
}
