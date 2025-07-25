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
                    visible: isStreamMode && showVideoCheckbox.checked

                    Connections {
                        target: qmlCommunication
                        function onvideo_pageFromCpp(frameData) {
                            try {
                                if (isStreamMode && videoPainter.visible) {
                                    if (frameData) {

                                        
                                        // 解析 QVariantMap
                                        let image = frameData["img"]
                                        let cameraName = frameData["camera_name"]
                                        
                                        // 更新图像显示
                                        videoPainter.updateImage(image)
                                        
                                        // 更新相机名称标签（如果需要显示）
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

                // 文件模式视频显示
                VideoOutput {
                    id: fileVideoOutput
                    anchors.fill: parent
                    visible: !isStreamMode && showVideoCheckbox.checked
                }
            }

            Timer {
                id: fpsTimer
                interval: 1000
                repeat: true
                running: isStreamMode && showVideoCheckbox.checked

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
                anchors.leftMargin: 0
                anchors.rightMargin: 20
                anchors.topMargin: 0
                anchors.bottomMargin: 20
                anchors {
                    fill: parent
                    margins: 10
                }
                spacing: 10

                RowLayout {
                    spacing: 5

                    Button {
                        text: isStreamMode ? "切换到文件模式" : "切换到流模式"
                        implicitWidth: 120
                        implicitHeight: 30
                        onClicked: {
                            isStreamMode = !isStreamMode
                            if (isStreamMode) {
                                // 切换到流模式
                                mediaPlayer.stop()
                                videoPainter.visible = showVideoCheckbox.checked
                                fileVideoOutput.visible = false
                            } else {
                                // 切换到文件模式
                                videoPainter.stopPainting()
                                videoPainter.visible = false
                                fileVideoOutput.visible = showVideoCheckbox.checked
                            }
                            console.log("模式切换:", isStreamMode ? "流模式" : "文件模式")
                        }
                    }

                    Button {
                        text: "打开录像"
                        implicitWidth: 80
                        implicitHeight: 30
                        enabled: !isStreamMode
                        onClicked: fileDialog.open()
                    }
                }

                Rectangle {
                    width: 1
                    height: 30
                    color: "#cccccc"
                }

                RowLayout {
                    spacing: 10

                    Button {
                        text: "上一帧"
                        implicitWidth: 60
                        implicitHeight: 30
                    }

                    Button {
                        text: "暂停"
                        implicitWidth: 60
                        implicitHeight: 30
                        enabled: !isStreamMode
                        onClicked: mediaPlayer.pause()
                    }

                    Button {
                        text: "播放"
                        implicitWidth: 60
                        implicitHeight: 30
                        enabled: !isStreamMode
                        onClicked: mediaPlayer.play()
                    }

                    Button {
                        text: "下一帧"
                        implicitWidth: 60
                        implicitHeight: 30
                    }

                    CheckBox {
                        id: showVideoCheckbox
                        text: "显示"
                        checked: true
                        onCheckedChanged: {
                            console.log("视频显示状态:", checked ? "显示" : "隐藏")
                        }
                    }
                }

                RowLayout {
                    spacing: 5
                    Label {
                        text: "曝光:"
                    }
                    Slider {
                        Layout.preferredWidth: 100
                        from: 0
                        to: 100
                        value: 60
                    }
                    SpinBox {
                        value: 60
                        from: 0
                        to: 100
                    }
                }

                RowLayout {
                    spacing: 5
                    Label {
                        text: "照相:"
                    }
                    ComboBox {
                        model: ["1"]
                        implicitWidth: 80
                    }
                }

                RowLayout {
                    Layout.alignment: Qt.AlignRight
                    spacing: 10

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
                text: "状态: " + (isStreamMode ? 
                      (videoPainter.visible ? "正在播放流" : "流已暂停") : 
                      (mediaPlayer.playbackState === MediaPlayer.PlayingState ? "正在播放文件" : "已停止"))
            }

            Item { Layout.fillWidth: true }

            Label {
                text: "分辨率: 1920x1080"
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: "选择视频文件"
        nameFilters: ["视频文件 (*.mp4 *.avi *.mkv *.mov *.wmv)"]
        onAccepted: {
            try {
                mediaPlayer.stop()
                mediaPlayer.source = fileDialog.currentFile
                mediaPlayer.play()
            } catch (e) {
                console.error("Video loading error:", e)
            }
        }
    }
}
