import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects
import InfinityStation 1.0
import QtQuick.Window
import QtCharts 
import "qrc:/infinity_station/res/version.js" as VersionJS
import public_controls 1.0


ApplicationWindow {
    id: root
    visible: true
    width: 1280
    height: 820
    minimumWidth: 1280
    minimumHeight: 820
    maximumWidth: 1280
    maximumHeight: 820
    title: qsTr("应用控制中心")
    flags: Qt.Window | Qt.FramelessWindowHint  // 无边框设置
    // 添加一个对象来缓存每个页面的数据状态
    property var pageDataCache: {}

    // 添加属性来跟踪窗口状态
    property var openedWindows: ({})
    // 跟踪当前选中的应用索引
    property int currentAppIndex: 0

    property var windowPositions: []


    // 添加主窗口容器
    Rectangle {
        id: windowFrame
        anchors.fill: parent
        color: "#f5f7fa"
        radius: 20  // 圆角

        // 自定义标题栏
        Rectangle {
            id: titleBar
            width: parent.width
            height: 32  // 降低标题栏高度
            color: "transparent"
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
                text: qsTr("工程车综合一体程序")
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
                
            Item { Layout.fillWidth: true } // 弹性占位

            // 版本信息区域
            RowLayout {
                spacing: 30
                Text { text: "版本: " + VersionJS.getVersionInfo().version; color: "#666"; font.pixelSize: 12 }
                Text { text: "分支: " + VersionJS.getVersionInfo().branch; color: "#666"; font.pixelSize: 12 }
                Text { text: "提交Hash: " + VersionJS.getVersionInfo().commitHash; color: "#666"; font.pixelSize: 12 }
                Text { text: "编译时间: " + VersionJS.getVersionInfo().compileTime; color: "#666"; font.pixelSize: 12 }
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

                // 最大化按钮
                Rectangle {
                    width: 46
                    height: titleBar.height
                    color: maximizeArea.containsMouse ? "#20000000" : "transparent"
                    Text {
                        anchors.centerIn: parent
                        text: root.visibility === Window.Maximized ? "❐" : "□"
                        color: "#2c3e50"
                        font.pixelSize: 14
                    }
                    MouseArea {
                        id: maximizeArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            if (root.visibility === Window.Maximized) {
                                root.showNormal();
                            } else {
                                root.showMaximized();
                            }
                        }
                    }
                }

                // 关闭按钮
                Rectangle {
                    width: 46
                    height: titleBar.height
                    color: closeArea.containsMouse ? "#e81123" : "transparent"

                    Text {
                        anchors.centerIn: parent
                        text: "✕"
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
                    clickPos = Qt.point(mouse.x, mouse.y);
                }

                onPositionChanged: {
                    if (pressed) {
                        // 添加pressed检查
                        var delta = Qt.point(mouse.x - clickPos.x, mouse.y - clickPos.y);
                        root.x += delta.x;
                        root.y += delta.y;
                    }
                }
            }
        }

        // 主内容区域 - 现在分为左侧导航和右侧详情
        Rectangle {
            id: mainContent
            anchors.top: titleBar.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            color: "transparent"

            // 左侧导航栏
            Rectangle {
                id: sidebar
                width: parent.width * 0.2
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.bottom: parent.bottom
                color: "#252a31"
                radius: 0

                // 左下角圆角处理
                Rectangle {
                    width: 20
                    height: 20
                    anchors.left: parent.left
                    anchors.bottom: parent.bottom
                    color: parent.color
                }
                // 左上角弧度修正
                Rectangle {
                    width: 20
                    height: 20
                    anchors.left: parent.left
                    anchors.top: parent.top
                    color: parent.color
                }

                // 应用列表
                ListView {
                    id: appListView
                    anchors.fill: parent
                    anchors.topMargin: 15
                    anchors.leftMargin: 8
                    anchors.rightMargin: 8
                    spacing: 2
                    clip: true
                    model: Modules.Modules.appData

                    delegate: Rectangle {
                        id: appItem
                        width: appListView.width - 16
                        height: 44
                        color: index === currentAppIndex ? "#37414f" : "transparent"
                        radius: 6

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 12
                            anchors.rightMargin: 8
                            spacing: 10

                            // 应用图标
                            Image {
                                source: modelData.icon
                                sourceSize.width: 20
                                sourceSize.height: 20
                                Layout.alignment: Qt.AlignVCenter
                            }

                            // 应用名称
                            Text {
                                text: modelData.title
                                font.pixelSize: 14
                                color: "white"
                                Layout.fillWidth: true
                                elide: Text.ElideRight
                            }
                        }

                        // 点击切换详情视图
                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            onEntered: {
                                if (index !== currentAppIndex) {
                                    parent.color = "#323a45";
                                }
                            }
                            onExited: {
                                if (index !== currentAppIndex) {
                                    parent.color = "transparent";
                                }
                            }
                            onClicked: {
                                currentAppIndex = index;
                            }
                        }
                    }

                    // 加载等待指示
                    BusyIndicator {
                        anchors.centerIn: parent
                        running: Modules.appData.length === 0
                        visible: Modules.appData.length === 0
                    }
                }
            }

            // 右侧详情区域
            Rectangle {
                id: detailsArea
                anchors.top: parent.top
                anchors.left: sidebar.right
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                color: "#f5f7fa"
                radius: windowFrame.radius

                // 右下角圆角处理
                Rectangle {
                    width: 20
                    height: 20
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    color: parent.color
                    radius: windowFrame.radius
                }

                // 右上角圆角处理
                Rectangle {
                    width: 20
                    height: 20
                    anchors.right: parent.right
                    anchors.top: parent.top
                    color: parent.color
                }

                // 内容堆栈 - 系统状态和应用详情
                StackLayout {
                    id: contentStack
                    anchors.fill: parent
                    currentIndex: Modules.appData[currentAppIndex].isBuiltIn ? 0 : 1

                    // 系统状态监控界面
                    Item {
                        id: systemStatusPage

                        ScrollView {
                            id: systemStatusScrollView
                            anchors.fill: parent
                            contentWidth: parent.width
                            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

                            ColumnLayout {
                                width: systemStatusScrollView.width
                                height: systemStatusScrollView.height
                                spacing: 15

                                // ======= 上方并排两个卡片 =======
                                RowLayout {
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: parent.height * 0.6  // 上半部分占60%
                                    Layout.leftMargin: 20
                                    Layout.rightMargin: 20
                                    spacing: 20

                                    // 模块/程序监控
                                    Rectangle {
                                        Layout.fillWidth: true
                                        Layout.fillHeight: true        // 添加高度填充
                                        Layout.preferredWidth: parent.width * 0.7  // 左右各占50%
                                        color: "white"
                                        radius: 8

                                        ColumnLayout {
                                            anchors.fill: parent
                                            spacing: 10

                                            // 标题栏
                                            RowLayout {
                                                Layout.fillWidth: true
                                                Layout.topMargin: 10
                                                Layout.leftMargin: 20
                                                Layout.rightMargin: 20

                                                Text {
                                                    text: "模块/程序监控"
                                                    font.pixelSize: 18
                                                    font.weight: Font.DemiBold
                                                    color: "#333"
                                                }
                                                Item {
                                                    Layout.fillWidth: true
                                                }
                                                Button {
                                                    text: "一键启动"
                                                    background: Rectangle {
                                                        color: parent.down ? "#3182ce" : (parent.hovered ? "#63b3ed" : "#4299e1")
                                                        radius: 4
                                                    }
                                                    contentItem: Text {
                                                        text: parent.text
                                                        font.pixelSize: 14
                                                        color: "white"
                                                        horizontalAlignment: Text.AlignHCenter
                                                        verticalAlignment: Text.AlignVCenter
                                                    }
                                                    onClicked: {
                                                        // 遍历所有模块并启动
                                                        for (var i = 0; i < Modules.systemStatusData.modules.length; i++) {
                                                            var module = Modules.systemStatusData.modules[i];
                                                            // 只启动未运行的程序
                                                            if (module.status !== "运行中" && module.status !== "异常") {
                                                                launchApplication(module.title);
                                                            }
                                                        }
                                                    }
                                                }
                                            }

                                            // 模块卡片区
                                            GridLayout {
                                                Layout.fillWidth: true
                                                Layout.leftMargin: 20
                                                Layout.rightMargin: 20
                                                columns: 2
                                                columnSpacing: 20
                                                rowSpacing: 20

                                                // 这里建议用Repeater，简化代码
                                                Repeater {
                                                    model: Modules.systemStatusData.modules

                                                    Rectangle {
                                                        Layout.fillWidth: true
                                                        Layout.preferredHeight: 130
                                                        color: "white"
                                                        border.color: "#e0e0e0"
                                                        radius: 4

                                                        property bool isRunning: modelData.status === "运行中"  // 添加运行状态属性
                                                        property bool isError: modelData.status === "异常"  // 新增异常状态

                                                        ColumnLayout {
                                                            anchors.fill: parent
                                                            anchors.margins: 15
                                                            spacing: 10

                                                            RowLayout {
                                                                Layout.fillWidth: true
                                                                Image {
                                                                    source: modelData.icon
                                                                    sourceSize.width: 20
                                                                    sourceSize.height: 20
                                                                    width: 20
                                                                    height: 20
                                                                }
                                                                Text {
                                                                    text: modelData.title
                                                                    font.pixelSize: 13
                                                                    font.weight: Font.Medium
                                                                    color: "#333"
                                                                }
                                                                Item {
                                                                    Layout.fillWidth: true
                                                                }
                                                                Rectangle {
                                                                    width: 60
                                                                    height: 24
                                                                    radius: 12
                                                                    // 修改背景色逻辑
                                                                    color: {
                                                                        if (parent.parent.parent.isError)
                                                                            return "#fff2f0";
                                                                            // 异常状态 - 红色背景
                                                                        else if (parent.parent.parent.isRunning)
                                                                            return "#e6f7ff";
                                                                            // 运行状态 - 蓝色背景
                                                                        else
                                                                            return "#f5f5f5";                                   // 未启动 - 灰色背景
                                                                    }

                                                                    Text {
                                                                        anchors.centerIn: parent
                                                                        // 修改文字和颜色逻辑
                                                                        text: {
                                                                            if (parent.parent.parent.parent.isError)
                                                                                return "异常";
                                                                            else if (parent.parent.parent.parent.isRunning)
                                                                                return "运行中";
                                                                            else
                                                                                return "未启动";
                                                                        }
                                                                        font.pixelSize: 10
                                                                        color: {
                                                                            if (parent.parent.parent.parent.isError)
                                                                                return "#ff4d4f";
                                                                                // 异常 - 红色
                                                                            else if (parent.parent.parent.parent.isRunning)
                                                                                return "#1890ff";
                                                                                // 运行中 - 蓝色
                                                                            else
                                                                                return "#999999";                                        // 未启动 - 灰色
                                                                        }
                                                                    }
                                                                }
                                                            }

                                                            Text {
                                                                Layout.fillWidth: true
                                                                text: modelData.description
                                                                font.pixelSize: 10
                                                                color: "#666"
                                                                wrapMode: Text.Wrap
                                                            }

                                                            Item {
                                                                Layout.fillHeight: true
                                                            }

                                                            // 修改底部按钮布局
                                                            RowLayout {
                                                                Layout.fillWidth: true
                                                                spacing: 15

                                                                // 重启/启动按钮
                                                                Rectangle {
                                                                    Layout.alignment: Qt.AlignHCenter  // 水平居中对齐
                                                                    Layout.preferredWidth: 120         // 增加按钮宽度
                                                                    height: 24
                                                                    radius: 12
                                                                    // 异常状态下按钮显示"重启"
                                                                    color: startBtnHover.containsMouse ? "#40a9ff" : "#1890ff"

                                                                    Text {
                                                                        anchors.centerIn: parent
                                                                        text: {
                                                                            if (parent.parent.parent.parent.isError)
                                                                                return "关闭显示";
                                                                            else if (parent.parent.parent.parent.isRunning)
                                                                                return "关闭显示";
                                                                            else
                                                                                return "显示";
                                                                        }
                                                                        font.pixelSize: 12
                                                                        color: "white"
                                                                    }

                                                                    MouseArea {
                                                                        id: startBtnHover
                                                                        anchors.fill: parent
                                                                        hoverEnabled: true
                                                                        cursorShape: Qt.PointingHandCursor

                                                                        onClicked: {
                                                                            if (parent.parent.parent.parent.isError) {
                                                                                // 处理异常重启逻辑
                                                                                console.log("处理异常重启:", modelData.title);
                                                                                launchApplication(modelData.title);
                                                                            } else if (parent.parent.parent.parent.isRunning) {
                                                                                // 正常重启逻辑
                                                                                console.log("正常重启:", modelData.title);
                                                                                if (openedWindows[modelData.title]) {
                                                                                    openedWindows[modelData.title].close();
                                                                                }
                                                                                launchApplication(modelData.title);
                                                                            } else {
                                                                                // 启动逻辑
                                                                                console.log("启动程序:", modelData.title);
                                                                                launchApplication(modelData.title);
                                                                            }
                                                                        }
                                                                    }
                                                                }

                                                                Item {
                                                                    Layout.fillWidth: true
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }

                                    // 服务监控
                                    Rectangle {
                                        Layout.fillWidth: true
                                        Layout.fillHeight: true        // 添加高度填充
                                        Layout.preferredWidth: parent.width * 0.3  // 左右各占50%
                                        color: "white"
                                        radius: 8

                                        ColumnLayout {
                                            anchors.fill: parent
                                            anchors.margins: 15
                                            spacing: 10

                                            Text {
                                                text: "服务监控"
                                                font.pixelSize: 16
                                                font.weight: Font.DemiBold
                                                color: "#333"
                                            }

                                            // 服务状态指示器
                                            GridLayout {
                                                Layout.fillWidth: true
                                                columns: 2
                                                columnSpacing: 20
                                                rowSpacing: 10
                                                Repeater {
                                                    model: Modules.systemStatusData.services
                                                    Rectangle {
                                                        Layout.fillWidth: true
                                                        Layout.preferredHeight: 40
                                                        color: "white"
                                                        RowLayout {
                                                            anchors.fill: parent
                                                            spacing: 10
                                                            //Image { source: modelData.icon || "qrc:/infinity_station/res/icon/service.ico"; width: 5; height: 5 }
                                                            Text {
                                                                text: modelData.title
                                                                font.pixelSize: 14
                                                                color: "#333"
                                                            }
                                                            Rectangle {
                                                                width: 10
                                                                height: 10
                                                                radius: 5
                                                                color: modelData.status === "正常" ? "#52c41a" : "#f5222d"
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    
                                }

                                // ======= 下方主机状态监控 =======
                                Rectangle {
                                    id: statusMonitor
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: parent.height * 0.4
                                    Layout.leftMargin: 20
                                    Layout.rightMargin: 20
                                    Layout.bottomMargin: 20
                                    color: "white"
                                    radius: 8


                                    Connections {
                                        target: qmlCommunication
                                        function onhardware_resourceFromCpp(info) {
                                            //console.log("Received hardware resource data:cpu", info.cpu_percent, "memory", info.memory_cost);
                                            if (Modules.systemStatusData.servers.length > 0) {
                                                var server = Modules.systemStatusData.servers[0]
                                                server.cpu = info.cpu_percent || 0
                                                server.memory = info.memory_cost || 0


                                                // 维护历史数据队列
                                                function updateHistory(history, newValue) {
                                                    history.push(newValue)
                                                    while(history.length > statusMonitor.historyLength) {
                                                        history.shift()
                                                    }
                                                }
                                                updateHistory(server.cpuHistory, server.cpu)
                                                updateHistory(server.memoryHistory, server.memory)


                                                // 触发重绘
                                                cpuCanvas.requestPaint()
                                                memoryCanvas.requestPaint()

                                            }
                                        }
                                    }

                                    ColumnLayout {
                                            anchors.fill: parent
                                            anchors.margins: 15
                                            spacing: 10

                                            Text {
                                                text: "主机状态监控"
                                                font.pixelSize: 18
                                                font.weight: Font.DemiBold
                                                color: "#333"
                                            }

                                            // CPU
                                            RowLayout {
                                                Layout.fillWidth: true
                                                spacing: 10
                                                Text {
                                                    text: "CPU使用率: " 
                                                    font.pixelSize: 14
                                                    color: "#9254de"
                                                }
                                                Canvas {
                                                    id: cpuCanvas
                                                    Layout.fillWidth: true
                                                    Layout.preferredHeight: 30
                                                    onPaint: {
                                                    var ctx = getContext("2d")
                                                    var width = cpuCanvas.width
                                                    var height = cpuCanvas.height
                                                    ctx.clearRect(0, 0, width, height)

                                                    // 绘制背景网格
                                                    ctx.strokeStyle = "#e0e0e0"
                                                    ctx.lineWidth = 1
                                                    for (var y = 0; y <= height; y += height / 4) {
                                                        ctx.beginPath()
                                                        ctx.moveTo(0, y)
                                                        ctx.lineTo(width, y)
                                                        ctx.stroke()
                                                    }

                                                    // 获取历史数据
                                                    var history = Modules.systemStatusData.servers[0].cpuHistory || []
                                                    if (history.length < 2)
                                                        return

                                                    // 绘制折线
                                                    ctx.beginPath()
                                                    var stepX = width / (history.length - 1)
                                                    var maxValue = 100
                                                    ctx.moveTo(0, height - (history[0] / maxValue * height))
                                                    for (var i = 1; i < history.length; i++) {
                                                        ctx.lineTo(i * stepX, height - (history[i] / maxValue * height))
                                                    }
                                                    ctx.strokeStyle = "#9254de"
                                                    ctx.lineWidth = 2
                                                    ctx.stroke()

                                                    // 绘制最新值文字
                                                    ctx.fillStyle = "#333"
                                                    ctx.font = "bold 14px sans-serif"
                                                    ctx.textAlign = "right"
                                                    ctx.textBaseline = "top"
                                                    ctx.fillText((history[history.length-1] || 0).toFixed(1) + "%", width-4, 4)
                                                }
                                                }
                                            }

                                            // 内存
                                            RowLayout {
                                                Layout.fillWidth: true
                                                spacing: 10
                                                Text {
                                                    text: "内存使用: " 
                                                    font.pixelSize: 14
                                                    color: "#52c41a"
                                                }
                                                Canvas {
                                                    id: memoryCanvas
                                                    Layout.fillWidth: true
                                                    Layout.preferredHeight: 30
                                                    onPaint: {
                                                        var ctx = getContext("2d")
                                                        var width = memoryCanvas.width
                                                        var height = memoryCanvas.height
                                                        ctx.clearRect(0, 0, width, height)

                                                        // 绘制背景网格
                                                        ctx.strokeStyle = "#e0e0e0"
                                                        ctx.lineWidth = 1
                                                        for (var y = 0; y <= height; y += height / 4) {
                                                            ctx.beginPath()
                                                            ctx.moveTo(0, y)
                                                            ctx.lineTo(width, y)
                                                            ctx.stroke()
                                                        }

                                                        // 获取历史数据
                                                        var history = Modules.systemStatusData.servers[0].memoryHistory || []
                                                        if (history.length < 2)
                                                            return

                                                        // 假设最大内存为16000MB，可根据实际调整
                                                        var maxMem = 16000

                                                        // 绘制折线
                                                        ctx.beginPath()
                                                        var stepX = width / (history.length - 1)
                                                        ctx.moveTo(0, height - (history[0] / maxMem * height))
                                                        for (var i = 1; i < history.length; i++) {
                                                            ctx.lineTo(i * stepX, height - (history[i] / maxMem * height))
                                                        }
                                                        ctx.strokeStyle = "#52c41a"
                                                        ctx.lineWidth = 2
                                                        ctx.stroke()

                                                        // 绘制最新值文字
                                                        ctx.fillStyle = "#333"
                                                        ctx.font = "bold 14px sans-serif"
                                                        ctx.textAlign = "right"
                                                        ctx.textBaseline = "top"
                                                        ctx.fillText((history[history.length-1] || 0).toFixed(1) + "MB", width-4, 4)
                                                    }
                                                }
                                            }
                                    }
                                }
                            }
                        }
                    }

                    // 应用详情界面
                    Item {
                        id: appDetailsPage

                        // 主内容
                        Item {
                            anchors.fill: parent
                            anchors.margins: 30

                            ColumnLayout {
                                anchors.fill: parent
                                spacing: 20

                                // 标题区域
                                RowLayout {
                                    Layout.fillWidth: true
                                    spacing: 15

                                    // 大图标
                                    Rectangle {
                                        width: 64
                                        height: 64
                                        radius: 10
                                        color: "#e9ecf1"

                                        Image {
                                            anchors.centerIn: parent
                                            source: currentAppIndex < Modules.appData.length ? Modules.appData[currentAppIndex].icon : ""
                                            sourceSize.width: 40
                                            sourceSize.height: 40
                                        }
                                    }

                                    // 标题和简介
                                    ColumnLayout {
                                        Layout.fillWidth: true
                                        spacing: 5

                                        Text {
                                            text: currentAppIndex < Modules.appData.length ? Modules.appData[currentAppIndex].title : ""
                                            font.pixelSize: 24
                                            font.weight: Font.DemiBold
                                            color: "#1a202c"
                                        }

                                        Text {
                                            text: currentAppIndex < Modules.appData.length && Modules.appData[currentAppIndex].isFutureModule ? "开发中..." : "点击启动打开应用"
                                            font.pixelSize: 14
                                            color: "#718096"
                                        }
                                    }
                                }

                                // 分隔线
                                Rectangle {
                                    Layout.fillWidth: true
                                    height: 1
                                    color: "#e2e8f0"
                                }

                                // 详情信息
                                ScrollView {
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    clip: true
                                    ScrollBar.vertical.policy: ScrollBar.AsNeeded

                                    ColumnLayout {
                                        width: parent.width
                                        spacing: 20

                                        // 描述信息
                                        Text {
                                            Layout.fillWidth: true
                                            text: currentAppIndex < Modules.appData.length ? Modules.appData[currentAppIndex].info : ""
                                            font.pixelSize: 15
                                            color: "#4a5568"
                                            wrapMode: Text.WordWrap
                                            lineHeight: 1.4
                                        }

                                        // 功能特点
                                        Rectangle {
                                            Layout.fillWidth: true
                                            height: featureColumn.height + 30
                                            color: "#ffffff"
                                            radius: 10
                                            visible: currentAppIndex < Modules.appData.length && !Modules.appData[currentAppIndex].isFutureModule

                                            ColumnLayout {
                                                id: featureColumn
                                                anchors.left: parent.left
                                                anchors.right: parent.right
                                                anchors.top: parent.top
                                                anchors.margins: 15
                                                spacing: 10

                                                Text {
                                                    text: "主要功能"
                                                    font.pixelSize: 16
                                                    font.weight: Font.DemiBold
                                                    color: "#2d3748"
                                                }

                                                Repeater {
                                                    model: currentAppIndex < Modules.appData.length && Modules.appData[currentAppIndex].features ? Modules.appData[currentAppIndex].features.length : 0
                                                    delegate: RowLayout {
                                                        Layout.fillWidth: true
                                                        spacing: 10

                                                        Rectangle {
                                                            width: 8
                                                            height: 8
                                                            radius: 4
                                                            color: "#4299e1"
                                                        }

                                                        Text {
                                                            text: currentAppIndex < Modules.appData.length && Modules.appData[currentAppIndex].features ? Modules.appData[currentAppIndex].features[index] : ""
                                                            font.pixelSize: 14
                                                            color: "#4a5568"
                                                            Layout.fillWidth: true
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }

                                // 底部启动按钮
                                Rectangle {
                                    id: launchButton
                                    Layout.alignment: Qt.AlignRight
                                    width: Math.max(parent.width * 0.18, 100)
                                    height: width * 0.38
                                    radius: 8
                                    color: launchArea.containsMouse ? "#2b6cb0" : "#3182ce"
                                    visible: currentAppIndex < Modules.appData.length && !Modules.appData[currentAppIndex].isFutureModule && !Modules.appData[currentAppIndex].isBuiltIn
                                    enabled: currentAppIndex < Modules.appData.length && !Modules.appData[currentAppIndex].isFutureModule && !Modules.appData[currentAppIndex].isBuiltIn

                                    Text {
                                        anchors.centerIn: parent
                                        text: "显示"
                                        font.pixelSize: 16
                                        font.weight: Font.Medium
                                        color: "white"
                                    }

                                    MouseArea {
                                        id: launchArea
                                        anchors.fill: parent
                                        hoverEnabled: true
                                        onClicked: {
                                            if (currentAppIndex >= Modules.appData.length)
                                                return;
                                            launchApplication(Modules.appData[currentAppIndex].title);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    // 修改后的 Timer 部分
    Timer {
        interval: 1000 // 每秒检查一次
        running: true
        repeat: true
        onTriggered: {
            // 检查所有已打开的窗口是否仍然有效
            for (var key in openedWindows) {
                var window = openedWindows[key];
                // 检查窗口对象是否有效且可见
                if (!window || !window.visible) {
                    console.log("窗口已关闭:", key);

                    // 保留数据到缓存
                    if (!pageDataCache[key]) {
                        pageDataCache[key] = {
                            data: window.pageData // 假设窗口有一个 pageData 属性存储数据
                            ,
                            timestamp: new Date().getTime()
                        };
                        //console.log("缓存数据:", key, pageDataCache[key].data);
                    }

                    // 从 openedWindows 中移除引用
                    delete openedWindows[key];
                }
            }

        }
    }

    // 应用启动公共函数
    function launchApplication(appTitle) {
        console.log("尝试启动应用:", appTitle);

        // 查找对应的应用数据
        var modelData = null;
        for (var i = 0; i < Modules.appData.length; i++) {
            console.log("应用数据:", Modules.appData[i].title);
            if (Modules.appData[i].title === appTitle) {
                modelData = Modules.appData[i];
                console.log("找到匹配的应用数据:", modelData);
                break;
            }
        }

        if (!modelData) {
            console.error("未找到匹配的应用数据:", appTitle);
            return;
        }

        // 检查 source 路径
        if (modelData.hasOwnProperty("source") && modelData.source !== "") {
            var resolvedUrl = Qt.resolvedUrl(modelData.source);
            console.log("解析后的路径:", resolvedUrl);
            if (!resolvedUrl) {
                console.error("路径解析失败:", modelData.source);
                return;
            }

            // 检查窗口是否已经打开
            if (openedWindows[modelData.title] && openedWindows[modelData.title].visible) {
                console.log("窗口已存在，提升并激活:", modelData.title);
                openedWindows[modelData.title].raise();
                openedWindows[modelData.title].requestActivate();
            } else {
                // 删除可能存在的无效引用
                if (openedWindows[modelData.title]) {
                    console.log("删除无效的窗口引用:", modelData.title);
                    delete openedWindows[modelData.title];
                }

                // 创建新窗口
                var component = Qt.createComponent(modelData.source);
                if (component.status === Component.Ready) {
                    console.log("组件加载成功:", modelData.source);
                    var window = component.createObject(null, {
                        "visible": true,
                        "width": 1080,
                        "height": 820,
                        "title": modelData.title
                    });

                    // 注册窗口
                    openedWindows[modelData.title] = window;

                    // 如果有缓存数据，恢复它
                    if (pageDataCache[modelData.title] && pageDataCache[modelData.title].data) {
                        window.pageData = pageDataCache[modelData.title].data;
                    }

                    // 窗口关闭事件处理
                    window.closing.connect(function () {
                        if (!pageDataCache[modelData.title]) {
                            pageDataCache[modelData.title] = {
                                data: window.pageData,
                                timestamp: new Date().getTime()
                            };
                        }
                        delete openedWindows[modelData.title];
                    });

                    // 更新模块状态为运行中
                    for (var j = 0; j < Modules.systemStatusData.modules.length; j++) {
                        if (Modules.systemStatusData.modules[j].title === modelData.title) {
                            Modules.systemStatusData.modules[j].status = "运行中";
                            break;
                        }
                    }
                } else {
                    console.error("组件加载失败:", component.errorString());
                }
            }
        } else {
            console.error("应用数据缺少 source 属性:", modelData);
        }
    }
}
