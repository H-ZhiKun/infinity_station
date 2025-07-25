import QtQuick 
import QtQuick.Controls 
import QtQuick.Layouts 
import QtQuick.Window 
import Qt.labs.platform
import InfinityStation 1.0


Item {
    id: videoPreviewComponent
    
    // 暴露的属性
    property real frameRate: 0.0
    property bool isStreamMode: true
    property string taskName: "无任务"
    property string savepath: "E:\\弓网科技\\"
    property alias showVideo: showVideoCheckbox.checked
    property alias recording: recordCheckbox.checked
    property bool recordStateFromCpp: false         
    property string camera_name: ""     
    property int modelIndex: -1  // 用于标识在ListModel中的位置                           
    
    // 暴露的信号
    signal saveImageRequested()
    signal configRequested()
    signal recordStateChanged(bool isRecording)
    signal cameraNameUpdated(string newName, int index)
    

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
                
                // 添加用于更新图像的函数
                function updateImages(imageData) {
                    videoPainter.updateImage(imageData);
                    
                    // 更新帧率（假设每次更新都计算）
                    updateFrameRate(calculateFrameRate());
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
                text: camera_name
                visible: showVideoCheckbox.checked
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

                 // 曝光调整条
                Rectangle {
                    width: 180
                    height: 40
                    color: "transparent"
                    RowLayout {
                        anchors.fill: parent
                        spacing: 8
                        Label { 
                            text: "曝光:" 
                            font.pixelSize: 12
                            color: "#333"
                        }
                        Slider {
                            id: exposureSlider
                            from: 0
                            to: 100
                            value: 50
                            stepSize: 1
                            Layout.preferredWidth: 120
                            Layout.fillWidth: true
                            
                            onValueChanged: {
                                qmlCommunication.behaviorFromQml(
                                    QmlCommunication.QmlActions.Parameter_modification,
                                    { "camera_id": camera_name, "exposure": value }
                                )
                            }
                            
                            background: Rectangle {
                                implicitHeight: 6
                                radius: 3
                                color: "#e0e0e0"
                                Rectangle {
                                    width: exposureSlider.visualPosition * parent.width
                                    height: parent.height
                                    radius: 3
                                    gradient: Gradient {
                                        GradientStop { position: 0.0; color: "#42a5f5" }
                                        GradientStop { position: 1.0; color: "#1976d2" }
                                    }
                                }
                            }
                            
                            handle: Rectangle {
                                x: exposureSlider.visualPosition * (parent.width - width)
                                y: (parent.height - height) / 2
                                width: 16
                                height: 16
                                radius: 8
                                color: "#ffffff"
                                border.color: "#1976d2"
                                border.width: 2
                                
                            }
                        }
                        Label { 
                            text: Math.round(exposureSlider.value) 
                            font.pixelSize: 12
                            font.bold: true
                            color: "#1976d2"
                            Layout.minimumWidth: 30
                            horizontalAlignment: Text.AlignRight
                        }
                    }
                }

                // 增益可编辑框
                Rectangle {
                    width: 120
                    height: 40
                    color: "transparent"
                    RowLayout {
                        anchors.fill: parent
                        spacing: 5
                        Label { text: "增益:" }
                        TextField {
                            id: gainField
                            text: "1.0"
                            inputMethodHints: Qt.ImhFormattedNumbersOnly
                            validator: DoubleValidator { bottom: 0.0; top: 100.0 }
                            Layout.preferredWidth: 50
                            onEditingFinished: {
                                // 这里可以调用C++接口或信号
                                qmlCommunication.behaviorFromQml(
                                    QmlCommunication.QmlActions.Parameter_modification,
                                    { "camera_id": camera_name, "gain": Number(text) }
                                )
                            }
                        }
                    }
                }

Button {
    text: "云台控制"
    implicitWidth: 80
    implicitHeight: 30
    onClicked: yuntaiDialog.open()
}

// 云台弹窗
Popup {
    id: yuntaiDialog
    modal: true
    width: 420
    height: 650

    anchors.centerIn: Overlay.overlay  // 确保在窗口中央
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    
    background: Rectangle {
        color: "#ffffff"
        radius: 8
    }
    
    Rectangle {
        anchors.fill: parent
        color: "#ffffff"
        border.color: "#1976d2"
        border.width: 2
        radius: 8
        
        ColumnLayout {
            anchors.fill: parent
            spacing: 0
            
            // 标题栏
            Rectangle {
                Layout.fillWidth: true
                height: 40
                color: "#1976d2"
                radius: 6  // 顶部圆角与父项匹配
                
                Label {
                    anchors.centerIn: parent
                    text: "云台控制"
                    color: "#fff"
                    font.pixelSize: 18
                    font.bold: true
                }
                
                // 关闭按钮
                Button {
                    anchors {
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                        margins: 5
                    }
                    width: 30
                    height: 30
                    flat: true
                    text: "×"
                    font.pixelSize: 20
                    onClicked: yuntaiDialog.close()
                }
            }
            
            // 云台控制内容
            YunTai {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.margins: 10
            }
        }
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
                    onClicked: saveImageRequested()
                }

                Button {
                    text: "参数配置"
                    implicitWidth: 80
                    implicitHeight: 30
                    onClicked: configRequested()
                }

                CheckBox {
                    id: recordCheckbox
                    text: "录像"
                    enabled: !videoPreviewComponent.recordStateFromCpp 
                    onCheckedChanged: {
                        qmlCommunication.behaviorFromQml(
                            QmlCommunication.QmlActions.VideoRecordButton,
                            {
                                "camera_id" : camera_name,
                                "state" : checked
                            }
                        )
                        if (!videoPreviewComponent.recordStateFromCpp) {
                            recordStateChanged(checked)
                        }
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

            // 只包裹Label，不要让MouseArea被挤没
            MouseArea {
                id: savePathArea
                anchors.verticalCenter: parent.verticalCenter
                width: savePathLabel.implicitWidth
                height: savePathLabel.implicitHeight
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    if (savepath && savepath.length > 0) {
                        var dir = savepath
                        if (savepath.indexOf("/") !== -1 || savepath.indexOf("\\") !== -1) {
                            dir = savepath.replace(/\\/g, "/")
                            if (dir.lastIndexOf("/") !== -1)
                                dir = dir.substring(0, dir.lastIndexOf("/"))
                        }
                        Qt.openUrlExternally("file:///" + dir)
                    }
                }
                Label {
                    id: savePathLabel
                    text: "视频保存路径: " + savepath
                    color: savePathArea.containsMouse ? "#1976d2" : "#333"
                    font.underline: savePathArea.containsMouse
                }
            }

            Item { Layout.fillWidth: true }
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

            videoPreviewComponent.recordStateFromCpp = Data; // true时禁用，false时可用
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

        function onSave_path_sendFromCpp(data) {
            console.log("保存路径:", data)
            savepath = data
        }
    }
    
    // 添加直接更新图像的方法，供外部调用
    function outsideupdateImages(imageData) {
        if (videoPainter && typeof videoPainter.updateImage === "function") {
            //console.log("outsideupdateImages:", imageData)
            videoPainter.updateImages(imageData);
        }
    }

    
    
    function updateFrameRate(fps) {
        frameRate = fps
    }
    
    // 用于计算帧率的帮助函数（示例实现）
    property var lastFrameTime: new Date().getTime()
    property var frameRates: []
    
    function calculateFrameRate() {
        var currentTime = new Date().getTime();
        var timeDiff = currentTime - lastFrameTime;
        lastFrameTime = currentTime;
        
        var instantFps = 1000 / timeDiff;
        
        // 保持最近10个帧率值用于平滑计算
        frameRates.push(instantFps);
        if (frameRates.length > 10) {
            frameRates.shift();
        }
        
        // 计算平均帧率
        var sum = 0;
        for (var i = 0; i < frameRates.length; i++) {
            sum += frameRates[i];
        }
        
        return sum / frameRates.length;
    }
    
    // 当camera_name更改时发出信号
    onCamera_nameChanged: {
        cameraNameUpdated(camera_name, modelIndex);
    }
}