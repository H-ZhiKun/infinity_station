import QtQuick 
import QtQuick.Controls 
import QtQuick.Layouts 
import QtQuick.Window 
import Qt.labs.platform
import InfinityStation 1.0

Window {
    visible: true
    width: 1920
    height: 1080
    minimumWidth: 1920
    minimumHeight: 1080
    maximumWidth: 1920
    maximumHeight: 1920
    title: "双视频预览"

    property real frameRate1: 0.0
    property real frameRate2: 0.0
    property bool isStreamMode1: true
    property bool isStreamMode2: true
    property string camera1Name: ""  // 左侧相机名称
    property string camera2Name: ""  // 右侧相机名称
    property bool camera1Set: false  // 左侧相机是否设置
    property bool camera2Set: false  // 右侧相机是否设置
    
    MediaPlayer {
        id: mediaPlayer1
        videoOutput: fileVideoOutput1
    }

    MediaPlayer {
        id: mediaPlayer2
        videoOutput: fileVideoOutput2
    }

    Rectangle {
        anchors.fill: parent
        color: "#404040"

        RowLayout {
            anchors.fill: parent
            spacing: 2

            // 左侧视频区域
            Rectangle {
                Layout.fillHeight: true
                Layout.preferredWidth: parent.width / 2 - 1
                color: "#404040"

                Rectangle {
                    id: videoArea1
                    anchors {
                        left: parent.left
                        right: parent.right
                        top: parent.top
                        bottom: bottomBar1.top
                    }
                    color: "#2a2a2a"

                    Item {
                        anchors.fill: parent

                        ImagePainter {
                            id: videoPainter1
                            anchors.fill: parent
                            visible: isStreamMode1 && showVideoCheckbox1.checked
                            
                            // 左侧视频处理
                            Connections {
                                target: qmlCommunication
                                function onVideo_pageFromCpp(frameData) {
                                    try {
                                        let imgMap = frameData
                                        let image = imgMap["img"]
                                        let cameraName = imgMap["camera_name"]
                                        
                                        // 调试输出
                                        console.log("收到视频帧:", {
                                            cameraName: cameraName,
                                            camera1Name: camera1Name,
                                            camera2Name: camera2Name,
                                            camera1Set: camera1Set,
                                            camera2Set: camera2Set
                                        })

                                        // 第一次收到数据时，设置为左侧相机
                                        if (!camera1Set && !camera2Set && cameraName) {
                                            camera1Name = cameraName
                                            camera1Set = true
                                            console.log("设置第一个相机为左侧:", camera1Name)
                                        }
                                        // 收到不同于左侧相机的数据时，设置为右侧相机
                                        else if (camera1Set && !camera2Set && cameraName && cameraName !== camera1Name) {
                                            camera2Name = cameraName
                                            camera2Set = true
                                            console.log("设置第二个相机为右侧:", camera2Name)
                                        }

                                        // 根据相机名称更新对应的视频窗口
                                        if (image && cameraName) {
                                            if (cameraName === camera1Name && isStreamMode1 && videoPainter1.visible) {
                                                videoPainter1.updateImage(image)
                                                fpsTimer1.frameCount++
                                            }
                                            else if (cameraName === camera2Name && isStreamMode2 && videoPainter2.visible) {
                                                videoPainter2.updateImage(image)
                                                fpsTimer2.frameCount++
                                            }
                                        }
                                    } catch (e) {
                                        console.error("视频处理错误:", e)
                                    }
                                }
                            }
                        }

                        VideoOutput {
                            id: fileVideoOutput1
                            anchors.fill: parent
                            visible: !isStreamMode1 && showVideoCheckbox1.checked
                        }
                    }

                    Timer {
                        id: fpsTimer1
                        interval: 1000
                        repeat: true
                        running: isStreamMode1 && showVideoCheckbox1.checked
                        property int frameCount: 0
                        onTriggered: {
                            frameRate1 = frameCount
                            frameCount = 0
                        }
                    }
                }

                Rectangle {
                    id: bottomBar1
                    height: 80
                    anchors {
                        left: parent.left
                        right: parent.right
                        bottom: parent.bottom
                    }
                    color: "#f0f0f0"

                    RowLayout {
                        anchors {
                            fill: parent
                            margins: 10
                        }
                        spacing: 10

                        Button {
                            text: isStreamMode1 ? "切换到文件模式" : "切换到流模式"
                            implicitWidth: 120
                            implicitHeight: 30
                            onClicked: {
                                isStreamMode1 = !isStreamMode1
                                if (isStreamMode1) {
                                    resetCameraConfig()  // 切换模式时重置相机配置
                                    mediaPlayer1.stop()
                                    videoPainter1.visible = showVideoCheckbox1.checked
                                    fileVideoOutput1.visible = false
                                } else {
                                    videoPainter1.stopPainting()
                                    videoPainter1.visible = false
                                    fileVideoOutput1.visible = showVideoCheckbox1.checked
                                }
                            }
                        }

                        Button {
                            text: "打开录像"
                            implicitWidth: 80
                            implicitHeight: 30
                            enabled: !isStreamMode1
                            onClicked: fileDialog1.open()
                        }

                        RowLayout {
                            spacing: 5
                            Button {
                                text: "暂停"
                                implicitWidth: 60
                                implicitHeight: 30
                                enabled: !isStreamMode1
                                onClicked: mediaPlayer1.pause()
                            }

                            Button {
                                text: "播放"
                                implicitWidth: 60
                                implicitHeight: 30
                                enabled: !isStreamMode1
                                onClicked: mediaPlayer1.play()
                            }

                            CheckBox {
                                id: showVideoCheckbox1
                                text: "显示"
                                checked: true
                            }
                        }

                        Label {
                            text: "帧率: " + Math.round(frameRate1) + " fps"
                        }
                    }
                }
            }

            // 右侧视频区域
            Rectangle {
                Layout.fillHeight: true
                Layout.preferredWidth: parent.width / 2 - 1
                color: "#404040"

                Rectangle {
                    id: videoArea2
                    anchors {
                        left: parent.left
                        right: parent.right
                        top: parent.top
                        bottom: bottomBar2.top
                    }
                    color: "#2a2a2a"

                    Item {
                        anchors.fill: parent

                        ImagePainter {
                            id: videoPainter2
                            anchors.fill: parent
                            visible: isStreamMode2 && showVideoCheckbox2.checked
                        }

                        VideoOutput {
                            id: fileVideoOutput2
                            anchors.fill: parent
                            visible: !isStreamMode2 && showVideoCheckbox2.checked
                        }
                    }

                    Timer {
                        id: fpsTimer2
                        interval: 1000
                        repeat: true
                        running: isStreamMode2 && showVideoCheckbox2.checked
                        property int frameCount: 0
                        onTriggered: {
                            frameRate2 = frameCount
                            frameCount = 0
                        }
                    }
                }

                Rectangle {
                    id: bottomBar2
                    height: 80
                    anchors {
                        left: parent.left
                        right: parent.right
                        bottom: parent.bottom
                    }
                    color: "#f0f0f0"

                    RowLayout {
                        anchors {
                            fill: parent
                            margins: 10
                        }
                        spacing: 10

                        Button {
                            text: isStreamMode2 ? "切换到文件模式" : "切换到流模式"
                            implicitWidth: 120
                            implicitHeight: 30
                            onClicked: {
                                isStreamMode2 = !isStreamMode2
                                if (isStreamMode2) {
                                    mediaPlayer2.stop()
                                    videoPainter2.visible = showVideoCheckbox2.checked
                                    fileVideoOutput2.visible = false
                                } else {
                                    videoPainter2.stopPainting()
                                    videoPainter2.visible = false
                                    fileVideoOutput2.visible = showVideoCheckbox2.checked
                                }
                            }
                        }

                        Button {
                            text: "打开录像"
                            implicitWidth: 80
                            implicitHeight: 30
                            enabled: !isStreamMode2
                            onClicked: fileDialog2.open()
                        }

                        RowLayout {
                            spacing: 5
                            Button {
                                text: "暂停"
                                implicitWidth: 60
                                implicitHeight: 30
                                enabled: !isStreamMode2
                                onClicked: mediaPlayer2.pause()
                            }

                            Button {
                                text: "播放"
                                implicitWidth: 60
                                implicitHeight: 30
                                enabled: !isStreamMode2
                                onClicked: mediaPlayer2.play()
                            }

                            CheckBox {
                                id: showVideoCheckbox2
                                text: "显示"
                                checked: true
                            }
                        }

                        Label {
                            text: "帧率: " + Math.round(frameRate2) + " fps"
                        }
                    }
                }
            }
        }
    }

    FileDialog {
        id: fileDialog1
        title: "选择左侧视频文件"
        nameFilters: ["视频文件 (*.mp4 *.avi *.mkv *.mov *.wmv)"]
        onAccepted: {
            try {
                mediaPlayer1.stop()
                mediaPlayer1.source = fileDialog1.currentFile
                mediaPlayer1.play()
            } catch (e) {
                console.error("左侧视频加载错误:", e)
            }
        }
    }

    FileDialog {
        id: fileDialog2
        title: "选择右侧视频文件"
        nameFilters: ["视频文件 (*.mp4 *.avi *.mkv *.mov *.wmv)"]
        onAccepted: {
            try {
                mediaPlayer2.stop()
                mediaPlayer2.source = fileDialog2.currentFile
                mediaPlayer2.play()
            } catch (e) {
                console.error("右侧视频加载错误:", e)
            }
        }
    }

    function resetCameraConfig() {
        console.log("重置前状态:", {
            camera1: camera1Name,
            camera2: camera2Name,
            isSet1: camera1Set,
            isSet2: camera2Set
        })
        
        camera1Name = ""
        camera2Name = ""
        camera1Set = false
        camera2Set = false
        
        console.log("相机配置已重置")
    }
}
