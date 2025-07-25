import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects  // 添加特效支持
import InfinityStation 1.0
import QtQuick.Window


ApplicationWindow {
    id: root
    visible: true
    width: 900
    height: 600
    minimumWidth: 900
    minimumHeight: 600
    maximumWidth: 900
    maximumHeight: 600
    title: qsTr("应用控制中心")
    //color: "transparent"  // 设置透明背景
    flags: Qt.Window | Qt.FramelessWindowHint  // 无边框设置
   
    // 添加属性来跟踪窗口状态
    property var openedWindows: ({})

    // 在主窗口中添加一个属性来跟踪当前页
    property int currentPage: 0

        // 添加主窗口容器
    Rectangle {
        id: windowFrame
        anchors.fill: parent
        //anchors.margins: 10  // 留出阴影空间
        color: "#f5f5f5"
        radius: 20  // 圆角

        Component.onCompleted: {
            //root.setMask(windowFrame);
        }

        // 自定义标题栏
        Rectangle {
            id: titleBar
            width: parent.width
            height: 32  // 降低标题栏高度
            color: "transparent"  // 改为透明以显示主背景
            radius: windowFrame.radius  // 匹配主窗口圆角
            
            // 标题栏分割线
            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: 1
                color: "#e0e0e0"
            }

            // 应用图标
            Image {
                id: appIcon
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: 16
                width: 20
                height: 20
                source: "qrc:/infinity_station/res/icon/gw.ico"
            }

            // 标题
            Text {
                anchors.left: appIcon.right
                anchors.leftMargin: 12
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("应用控制中心")
                color: "#2c3e50"
                font.pixelSize: 14
                font.weight: Font.Medium
            }

            // 拖拽区域指示器
            Row {
                anchors.right: controlButtons.left
                anchors.rightMargin: 20
                anchors.verticalCenter: parent.verticalCenter
                spacing: 4
                
                Rectangle {
                    width: 4
                    height: 4
                    radius: 2
                    color: "#2c3e50"
                    opacity: 0.3
                }
                Rectangle {
                    width: 4
                    height: 4
                    radius: 2
                    color: "#2c3e50"
                    opacity: 0.3
                }
                Rectangle {
                    width: 4
                    height: 4
                    radius: 2
                    color: "#2c3e50"
                    opacity: 0.3
                }
            }

            // 控制按钮容器
            Row {
                id: controlButtons
                anchors.right: parent.right
                anchors.rightMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                spacing: 10

                // 最小化按钮
                Rectangle {
                    width: 46
                    height: titleBar.height
                    color: minimizeArea.containsMouse ? "#20000000" : "transparent"
                    
                    Text {
                        anchors.centerIn: parent
                        text: "─"
                        color: "#2c3e50"
                        font.pixelSize: 14
                    }

                    MouseArea {
                        id: minimizeArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: root.showMinimized()
                    }
                }

                // 关闭按钮
                Rectangle {
                    width: 46
                    height: titleBar.height
                    color: closeArea.containsMouse ? "#e81123" : "transparent"
                    
                    Text {
                        anchors.centerIn: parent
                        text:  "✕"
                        color: closeArea.containsMouse ? "white" : "#2c3e50"
                        font.pixelSize: 14
                    }

                    MouseArea {
                        id: closeArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: root.close()
                    }
                }
            }
            // 窗口拖动区域 - 修改这里，排除右侧控制按钮区域
            MouseArea {
                anchors.left: parent.left
                anchors.right: controlButtons.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                property point clickPos: "0,0"
                
                onPressed: {
                    clickPos = Qt.point(mouse.x, mouse.y)
                }
                
                onPositionChanged: {
                    if (pressed) {  // 添加pressed检查
                        var delta = Qt.point(mouse.x - clickPos.x, mouse.y - clickPos.y)
                        root.x += delta.x
                        root.y += delta.y
                    }
                }
            }
        }

        StackLayout {
            id: mainStack
            anchors.fill: parent
            //anchors.topMargin: titleBar.height  // 为标题栏留出空间
            currentIndex: 0 // 0: 主界面, 1: 加载界面

            // 主界面
            ColumnLayout {
                id: mainPage
                spacing: 20

                // 主内容区域
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.topMargin: 20
                    color: "transparent"
                    clip: true  // 确保超出部分被裁剪

                    Flickable {
                        id: flickable
                        anchors.fill: parent
                        anchors.leftMargin: 50
                        anchors.rightMargin: 45
                        anchors.topMargin: 90
                        anchors.bottomMargin: 50
                        contentWidth: gridLayout.width
                        contentHeight: gridLayout.height
                        clip: true
                        interactive: false  // 禁用鼠标滚动
                        
                        // 添加平滑滚动动画
                        Behavior on contentX {
                            NumberAnimation { 
                                duration: 300
                                easing.type: Easing.OutQuad 
                            }
                        }

                        // 修改网格布局
                        GridLayout {
                            id: gridLayout
                            width: (240 + columnSpacing) * Math.ceil(repeater.count / 2)  // 动态计算总宽度
                            height: parent.height
                            columns: Math.ceil(repeater.count / 2)
                            rows: 2
                            columnSpacing: 20
                            rowSpacing: 20
                            // 确保网格在视图中居中
                            anchors.verticalCenter: parent.verticalCenter

                            // 保持原有的 Repeater 部分
                            Repeater {
                                id: repeater
                                model: [
                                    {
                                        title: "定位系统",
                                        modelName: "location_page",
                                        info: "定位系统模块提供了车辆的实时位置信息，包括经纬度、速度、方向等信息。",
                                        icon: "qrc:/infinity_station/res/icon/onlinelocation.ico",
                                        source: "location_page/location_page.qml",
                                    },
                                    {
                                        title: "视频监控",
                                        modelName: "video_page",
                                        info : "视频监控模块提供了车辆的实时视频信息，包括车辆的前后视图、车辆的周围环境等信息。",
                                        icon: "qrc:/infinity_station/res/icon/video.ico",
                                        source: "video_page/video_page_nofile.qml",
                                    },
                                    {
                                        title: "MVB监控",
                                        modelName: "mvb_page",
                                        info: "MVB监控模块提供了车辆与MVB系统的实时通信信息，包括车辆位置、速度、方向等信息。",
                                        icon: "qrc:/infinity_station/res/icon/MVB.ico",
                                        source: "mvb_page/mvb_page.qml",
                                    },
                                    {
                                        title: "任务管理",
                                        modelName: "task_page",
                                        info: "任务管理模块提供了任务的执行状态、任务的执行进度等信息，允许用户手动控制任务的执行。",
                                        icon: "qrc:/infinity_station/res/icon/task.ico",
                                        source: "task_page/task_page.qml",
                                    },
                                    {
                                        title: "设备信息",
                                        modelName: "taskcontroller_page",
                                        info: "设备信息模块提供了CPU，GPU，内存，硬盘等设备的实时信息，包括设备名称、设备类型、设备状态等信息。",
                                        icon: "qrc:/infinity_station/res/icon/taskcontroller.ico",
                                        source: "taskcontroller_page/taskcontroller_page.qml",
                                    },
                                    {
                                        title: "波磨监控",
                                        modelName: "corrugation_page",
                                        info : "波磨监控模块提供了铁轨的实时磨损信息。",
                                        icon: "qrc:/infinity_station/res/icon/corrugation.ico",
                                        source: "corrugation_page/corrugation_page.qml",
                                    },
                                    {
                                        title: "燃弧监测",
                                        modelName: "arc_page",
                                        info : "燃弧检测模块。",
                                        icon: "qrc:/infinity_station/res/icon/arc.ico",
                                        source: "arc_page/arc_page.qml",
                                    },
                                    {
                                        title: "雷达限界",
                                        modelName: "radar_page",
                                        info : "雷达限界模块。",
                                        icon: "qrc:/infinity_station/res/icon/circum.ico",
                                        source: "radar_page/radar_page.qml",
                                    },
                                    {
                                        title: "离线雷达限界",
                                        modelName: "radar_outline_page",
                                        info : "离线雷达限界模块。",
                                        icon: "qrc:/infinity_station/res/icon/radaroutline.ico",
                                        source: "radar_outline_page/radar_outline_page.qml",
                                    },
                                    {
                                        title: "未来模块",
                                        info: "to be continued",
                                        icon: "qrc:/infinity_station/res/icon/future.ico",
                                        isFutureModule: true  // 添加标识
                                    }
                                ]

                                delegate: Rectangle {
                                    Layout.preferredWidth: 240
                                    Layout.preferredHeight: 160
                                    color: "#ffffff"
                                    radius: 10
                                    border.color: "#e0e0e0"
                                    border.width: 1

                                    ColumnLayout {
                                        anchors.fill: parent
                                        anchors.margins: modelData.isFutureModule ? 0 : 12
                                        spacing: 8

                                        // 仅当不是未来模块时显示标题行
                                        RowLayout {
                                            Layout.fillWidth: true
                                            spacing: 8
                                            visible: !modelData.isFutureModule

                                            Text {
                                                text: modelData.title
                                                font.pixelSize: 18
                                                font.bold: true
                                                color: "#2c3e50"
                                            }
                                        }

                                        // 图标
                                        Image {
                                            source: modelData.icon
                                            sourceSize {
                                                width: modelData.isFutureModule ? parent.width : 24
                                                height: modelData.isFutureModule ? parent.height : 24
                                            }
                                            fillMode: Image.PreserveAspectFit
                                            Layout.alignment: Qt.AlignCenter
                                            //Layout.fillWidth: modelData.isFutureModule
                                            //Layout.fillHeight: modelData.isFutureModule
                                        }

                                        // 仅当不是未来模块时显示信息
                                        Text {
                                            Layout.fillWidth: true
                                            Layout.fillHeight: true
                                            text: modelData.info
                                            visible: !modelData.isFutureModule
                                            font.pixelSize: 12
                                            color: "#666666"
                                            wrapMode: Text.WordWrap
                                            elide: Text.ElideRight
                                        }
                                    }

                                    MouseArea {
                                        anchors.fill: parent
                                        hoverEnabled: true
                                        onEntered: parent.color = "#f5f5f5"
                                        onExited: parent.color = "#ffffff"
                                        onClicked: {
                                            if (!modelData.isFutureModule && modelData.source) {
                                                // 检查窗口是否已经打开
                                                if (openedWindows[modelData.title]) {
                                                    // 如果已打开，将其激活并置顶
                                                    openedWindows[modelData.title].raise()
                                                    openedWindows[modelData.title].requestActivate()
                                                } else {
                                                    // 如果未打开，创建新窗口
                                                    var component = Qt.createComponent(modelData.source)
                                                    if (component.status === Component.Ready) {
                                                        var window = component.createObject(null, {
                                                            "visible": true,
                                                            "width": 1680,
                                                            "height": 960,
                                                            "title": modelData.title
                                                        })
                                                        
                                                        // 注册窗口
                                                        openedWindows[modelData.title] = window
                                                        var pageData = {
                                                            "pageName": QmlCommunication.ForQmlSignals[modelData.modelName],  // 使用modelName
                                                            "pageData": true
                                                        }
                                                        qmlCommunication.behaviorFromQml(QmlCommunication.QmlActions.PageChange, pageData)
                                                        // 更新控制器状态
                                                        // if (modelData.modelName === "video_page") {
                                                        //     App.getController("VideoController").setPageActive(true);
                                                        // }
                                                        console.log("QmlPage have init ", QmlCommunication.ForQmlSignals[modelData.modelName])
                                                        // 监听窗口关闭事件
                                                        window.closing.connect(function() {
                                                        // 从跟踪对象中移除
                                                        delete openedWindows[modelData.title]
                                                        })
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    // 修改导航按钮样式
                    Rectangle {
                        id: navigationBar
                        height: 40
                        anchors {
                            left: parent.left
                            right: parent.right
                            bottom: parent.bottom
                            bottomMargin: 20
                        }
                        color: "transparent"

                        RowLayout {
                            anchors.centerIn: parent
                            spacing: 20

                            // 左箭头按钮
                            Rectangle {
                                width: 36
                                height: 36
                                radius: 18
                                color: leftButton.containsMouse ? "#e0e0e0" : "#f5f5f5"
                                //visible: flickable.contentX > 0  // 只在有内容可滚动时显示

                                Text {
                                    anchors.centerIn: parent
                                    text: "◄"
                                    color: "#2c3e50"
                                    font.pixelSize: 16
                                }

                                MouseArea {
                                    id: leftButton
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onClicked: {
                                        if (currentPage > 0) {
                                            // 滚动一页（6个卡片）
                                            var pageWidth = 3 * (240 + 20)
                                            flickable.contentX = Math.max(0, flickable.contentX - pageWidth)
                                            currentPage--;
                                            updateScrollIndicator();
                                        }
                                    }
                                }
                            }

                            // 导航指示器
                            Row {
                                spacing: 8
                                Repeater {
                                    model: Math.ceil(repeater.count / 6)  // 每页6个卡片
                                    
                                    Rectangle {
                                        width: 8
                                        height: 8
                                        radius: 4
                                        color: index === currentPage ? "#2c3e50" : "#cccccc"
                                        // color: Math.floor(flickable.contentX / (3 * (240 + 20))) === index 
                                        //        ? "#2c3e50" : "#cccccc"
                                    }
                                }
                            }

                            // 右箭头按钮
                            Rectangle {
                                width: 36
                                height: 36
                                radius: 18
                                color: rightButton.containsMouse ? "#e0e0e0" : "#f5f5f5"
                                //visible: flickable.contentX < (flickable.contentWidth - flickable.width)  // 只在有更多内容时显示

                                Text {
                                    anchors.centerIn: parent
                                    text: "►"
                                    color: "#2c3e50"
                                    font.pixelSize: 16
                                }

                                MouseArea {
                                    id: rightButton
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    onClicked: {
                                        var maxPage = Math.ceil(repeater.count / 6) - 1
                                        if (currentPage < maxPage) {
                                            // 滚动一页（6个卡片）
                                            var pageWidth = 3 * (240 + 20)
                                            flickable.contentX = Math.min(flickable.contentWidth - flickable.width, 
                                                                        flickable.contentX + pageWidth)
                                            currentPage++;
                                            updateScrollIndicator();
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

            }

            // 加载页面
            Rectangle {
                id: loaderPage
                color: "#f5f5f5"

                Loader {
                    id: loader
                    anchors.fill: parent
                    asynchronous: true
                    visible: status === Loader.Ready

                    onStatusChanged: {
                        if (status === Loader.Ready) {
                            mainStack.currentIndex = 1
                            loadingIndicator.visible = false
                        }
                    }
                }

                BusyIndicator {
                    id: loadingIndicator
                    anchors.centerIn: parent
                    visible: loader.status === Loader.Loading
                }
            }
        }

    // 关闭界面函数
    function hideInterface() {
        loader.visible = false
        loader.source = ""
    }
    // 添加一个函数来更新黑点指示器
    function updateScrollIndicator() {
            // 计算总页数
        var totalPages = Math.ceil(repeater.count / 6); // 每页6个卡片

        // 确保 currentPage 在有效范围内
        if (currentPage < 0) {
            currentPage = 0;
        } else if (currentPage >= totalPages) {
            currentPage = totalPages - 1;
        }


        // 更新左右箭头的可用性
        leftButton.enabled = currentPage > 0;
        rightButton.enabled = currentPage < totalPages - 1;

        // 更新左右箭头的样式
        if (currentPage === 0) {
            leftButton.color = "#f5f5f5"; // 灰色表示不可用
        } else {
            leftButton.color = leftButton.containsMouse ? "#e0e0e0" : "#f5f5f5";
        }

        if (currentPage === totalPages - 1) {
            rightButton.color = "#f5f5f5"; // 灰色表示不可用
        } else {
            rightButton.color = rightButton.containsMouse ? "#e0e0e0" : "#f5f5f5";
        }
    }

}
}