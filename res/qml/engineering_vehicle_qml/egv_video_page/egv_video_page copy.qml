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
    property string taskName: "无任务"

    Rectangle {
        anchors.fill: parent
        color: "#404040"

        // 视频显示区域
        Rectangle {
            id: videoArea
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                bottom: bottomBar.top
            }
            color: "#2a2a2a"

            // 视频帧显示
            ImagePainter {
                id: videoPainter
                anchors.fill: parent
                visible: showVideoCheckbox.checked

                Connections {
                    target: qmlCommunication
                    function onEgvvideo_pageFromCpp(frameData) {
                        if (!videoPainter.visible) return;
                        if (frameData && frameData["img"]) {
                            videoPainter.updateImage(frameData["img"])
                            cameraLabel.text = frameData["camera_name"] || ""
                            //fpsTimer.frameCount++
                        } else {
                            errorOverlay.showError("未收到有效视频帧")
                        }
                    }
                }
            }

            // 相机名称标签
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

            // 错误提示悬浮层
            Rectangle {
                id: errorOverlay
                anchors.centerIn: parent
                color: "#80000000"
                radius: 8
                visible: false
                width: 220; height: 40

                property alias text: errorText.text
                Timer { id: errorTimer; interval: 2000; running: false; onTriggered: errorOverlay.visible = false }
                function showError(msg) {
                    errorOverlay.text = msg
                    errorOverlay.visible = true
                    errorTimer.restart()
                }

                Text {
                    id: errorText
                    anchors.centerIn: parent
                    color: "white"
                    font.pixelSize: 16
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
                    id: recordCheckbox
                    text: "录像"
                    onCheckedChanged: {
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
                text: "任务名："  + taskName
            }

            Item { Layout.fillWidth: true }

            Label {
                text: "分辨率: 1920x1080"
            }
        }
    }

    Connections {
        target: qmlCommunication
        function onRecord_stateFromCpp(Data) {
            if (typeof Data !== "boolean") {
                console.error("接收到的录像状态不是布尔值:", Data);
                return;
            }
            console.log("接收到录像状态:", Data);
            recordCheckbox.checked = Data;

            if (Data) {
                console.log("开始录像");
            } else {
                console.log("结束录像");
            }
        }
    }

    Connections {
        target: qmlCommunication
        function onTaskname_recvFromCpp(data) {
            console.log("VIDEO收到任务名:", data)
            taskName = data
        }
    }
}
