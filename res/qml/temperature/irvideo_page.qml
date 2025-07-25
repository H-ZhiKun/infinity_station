import QtQuick 
import QtQuick.Controls 
import QtQuick.Layouts 
// import QtQuick.Window 
import Qt.labs.platform
import InfinityStation 1.0

Rectangle {
    visible: true
    anchors.fill: parent
    property real frameRate: 0.0
    property bool isStreamMode: true
    property string taskName: "无任务"

    Rectangle {
        anchors.fill: parent
        color: "#404040"
        // 视频显示区域
        Rectangle {
            id: videoArea
            anchors.fill: parent
            color: "#2a2a2a"
            // 视频帧显示
            ImagePainter {
                id: videoPainter
                anchors.fill: parent
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
              //  visible: showVideoCheckbox.checked
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
    }






}
