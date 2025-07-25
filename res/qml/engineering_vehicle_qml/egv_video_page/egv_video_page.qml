import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.Material
import public_controls 1.0

ApplicationWindow {
    id: root
    width: 1920
    height: 1080
    visible: true
    title: "视频监控系统"
    color: "#f5f5f5"
    
    // 将frameDataModel扩展为4个摄像机
    ListModel {
        id: frameDataModel
        ListElement { camera_name: "test_baumer"; camera_id: "cam1"; status: "在线"; stream_data: "" }
        // ListElement { camera_name: "燃弧相机2"; camera_id: "cam2"; status: "在线"; stream_data: "" }
        ListElement { camera_name: "test_baumer2"; camera_id: "cam3"; status: "在线"; stream_data: "" }
        // ListElement { camera_name: "录像相机2"; camera_id: "cam4"; status: "在线"; stream_data: "" }
    }
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12
    
        // Video Grid
        GridLayout {
            id: videoGrid
            Layout.fillWidth: true
            Layout.fillHeight: true
            columns: 2
            rowSpacing: 16
            columnSpacing: 16
            
            Repeater {
                id: repeaterInstance
                model: frameDataModel
                
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.minimumHeight: 300
                    Layout.minimumWidth: 400
                    color: "#ffffff"
                    radius: 6
                    border.color: "#e0e0e0"
                    border.width: 1
                    
                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 8
                        spacing: 8
                        
                        // Camera header
                        Rectangle {
                            Layout.fillWidth: true
                            height: 40
                            color: "#f5f5f5"
                            radius: 4
                            
                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 8
                                anchors.rightMargin: 8
                                
                                Label {
                                    text: camera_name
                                    font.pixelSize: 16
                                    font.bold: true
                                }
                                
                                Rectangle {
                                    width: 8
                                    height: 8
                                    radius: 4
                                    color: "#4CAF50"
                                }
                                
                                Label {
                                    text: status
                                    font.pixelSize: 12
                                    color: "#4CAF50"
                                }
                            }
                        }
                        
                        // Video display
                        Rectangle {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            color: "#000000"

                                // 添加全局信号处理以接收视频帧数据
                            
                            VideoDisplay {
                                id: videoDisplay
                                anchors.fill: parent
                                taskName: camera_id
                                camera_name: camera_name
                                objectName: "camera_" + camera_id
                                modelIndex: index  // 添加索引来标识在ListModel中的位置
                                
                                // 当组件内部更新camera_name时，更新ListModel
                                onCameraNameUpdated: function(newName, idx) {
                                    if (newName !== frameDataModel.get(idx).camera_name) {
                                        frameDataModel.setProperty(idx, "camera_name", newName);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        
        // Footer
        Rectangle {
            Layout.fillWidth: true
            height: 40
            color: "#E0E0E0"
            radius: 4
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: 8
                
                Label {
                    text: "系统状态: 正常运行"
                    font.pixelSize: 12
                }
                
                Item { Layout.fillWidth: true }
                
                Label {
                    text: "当前时间: " + new Date().toLocaleTimeString(Qt.locale(), "hh:mm:ss")
                    font.pixelSize: 12
                    
                    Timer {
                        interval: 1000
                        running: true
                        repeat: true
                        onTriggered: parent.text = "当前时间: " + new Date().toLocaleTimeString(Qt.locale(), "hh:mm:ss")
                    }
                }
            }
        }
    }
    Connections {
        target: qmlCommunication
        function onEgvvideo_pageFromCpp(frameData) {
            console.log("egv_video_page.qml: onEgvvideo_pageFromCpp()",JSON.stringify(frameData));
            for (var i = 0; i < frameDataModel.count; ++i) {
                if (frameDataModel.get(i).camera_name === frameData["camera_name"]) {
                    var videoContainer = repeaterInstance.itemAt(i);
                    if (videoContainer) {
                        // 方法1：通过层级访问
                        var videoDisplay = videoContainer.children[0].children[1].children[0];
                        videoDisplay.outsideupdateImages(frameData["img"]);
                    }
                    break;
                }
            }
        }
    }
    
}

