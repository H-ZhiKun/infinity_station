import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import InfinityStation 1.0
import QtQuick.Window
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
    // 修改flags设置，添加Qt.CustomizeWindowHint
    flags: Qt.Window | Qt.FramelessWindowHint
    // 添加属性来跟踪窗口状态
    property var openedWindows: ({})
    property var windowPositions: []
    property bool databaseState: false
    property bool videoState: false
    property bool locationState: false

    // 添加全局属性和样式
    property color accentColor: "#1890ff" 
    property color backgroundColor: "#f5f7fa"
    property color cardColor: "white"
    property int animationDuration: 200
    property int borderRadius: 8

    // 添加主窗口容器
    Rectangle {
        id: windowFrame
        anchors.fill: parent
        color: "#f5f7fa"
        border.color: "#0f0f0f"  // 自定义边框颜色
        border.width: 1          // 边框宽度
        //radius: 20  // 圆角

        // 内部白色背景容器（包含标题栏和内容）
        Rectangle {
            id: innerContainer
            anchors.fill: parent
            anchors.margins: 1  // 留出边框空间
            color: "#f5f7fa"
            radius: parent.radius - 1  // 内圆角稍小

            CustomTitleBar {
                id: customTitleBar
                anchors.top: parent.top  
                width: parent.width
                height: 36
                titleName: qsTr("综合控制中心")
                //iconSource: "qrc:/infinity_station/res/icon/gw.ico"
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

            // 主内容区域 - 现在是单一的系统状态页面，从这里加载其它功能
            Rectangle {
                id: mainContent
                anchors.top: customTitleBar.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                color: "transparent"  
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
                
                // 左下角圆角处理
                Rectangle {
                    width: 20
                    height: 20
                    anchors.left: parent.left
                    anchors.bottom: parent.bottom
                    color: parent.color
                    radius: windowFrame.radius
                }
                
                // 左上角圆角处理
                Rectangle {
                    width: 20
                    height: 20
                    anchors.left: parent.left
                    anchors.top: parent.top
                    color: parent.color
                }

                // 系统状态监控界面
                Item {
                    id: systemStatusPage
                    anchors.fill: parent

                    ScrollView {
                        id: systemStatusScrollView
                        anchors.fill: parent
                        contentWidth: parent.width
                        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

                        ColumnLayout {
                            width: systemStatusScrollView.width
                            height: systemStatusScrollView.height
                            spacing: 20

                            // ======= 主内容区域（左右结构） =======
                            RowLayout {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                spacing: 20
                                Layout.leftMargin: 20
                                Layout.rightMargin: 20

                                // ===== 左侧内容（占80%） =====
                                ColumnLayout {
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    Layout.preferredWidth: parent.width * 0.8 - 10
                                    spacing: 15

                                    // === 上方：模块/程序监控 ===
                                    Rectangle {
                                        Layout.fillWidth: true
                                        Layout.fillHeight: true
                                        Layout.preferredHeight: parent.height * 0.8  // 占左侧80%高度
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
                                                    text: "显示所有界面"
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
                                                        for (var i = 0; i < Modules.systemStatusData.appData.length; i++) {
                                                            var module = Modules.systemStatusData.appData[i];
                                                            // 只启动未运行的程序
                                                            if (module.status !== "运行中" && module.status !== "异常") {
                                                                launchApplication(module.title);
                                                            }
                                                        }
                                                    }
                                                }
                                            }

                                            ScrollView {
                                                Layout.fillWidth: true
                                                Layout.fillHeight: true
                                                Layout.leftMargin: 20
                                                Layout.rightMargin: 20
                                                ScrollBar.vertical.policy: ScrollBar.AsNeeded
                                                clip: true

                                                // 模块卡片区
                                                Item {
                                                    width: parent.width
                                                    implicitHeight: Math.ceil(Modules.systemStatusData.appData.filter(item => enabledModules.indexOf(item.modelName) !== -1).length / 3) * 150 // 动态计算高度

                                                    Grid {
                                                        id: modulesGrid
                                                        width: parent.width
                                                        columns: Math.max(2, Math.floor(parent.width / 300))  // 每列至少300px宽
                                                        columnSpacing: 20
                                                        rowSpacing: 20

                                                        Repeater {
                                                            model: Modules.systemStatusData.appData.filter(function(item) {
                                                                return enabledModules.indexOf(item.modelName) !== -1
                                                            })

                                                            Rectangle {
                                                                width: (modulesGrid.width - modulesGrid.columnSpacing * 2) / 3  // 精确计算宽度
                                                                height: 150  // 固定高度
                                                                color: "white"
                                                                border.color: "#e0e0e0"
                                                                border.width: 1
                                                                radius: 6  // 增大圆角

                                                                ColumnLayout {
                                                                    anchors.fill: parent
                                                                    anchors.margins: 12  // 增加内边距
                                                                    spacing: 8

                                                                    // 标题行
                                                                    RowLayout {
                                                                        Layout.fillWidth: true
                                                                        spacing: 8

                                                                        // 图标
                                                                        Rectangle {
                                                                            width: 40
                                                                            height: 40
                                                                            radius: 14
                                                                            color: {
                                                                                if (modelData.status === "异常") return "#fff2f0";
                                                                                else if (modelData.status === "运行中") return "#e6f7ff";
                                                                                else return "#f5f5f5";
                                                                            }

                                                                            Image {
                                                                                anchors.centerIn: parent
                                                                                source: modelData.icon
                                                                                sourceSize: Qt.size(25, 25)
                                                                                width: 25
                                                                                height: 25
                                                                            }
                                                                        }

                                                                        // 标题
                                                                        Text {
                                                                            text: modelData.title
                                                                            font {
                                                                                pixelSize: 18
                                                                                weight: Font.Medium
                                                                            }
                                                                            color: "#333"
                                                                            Layout.fillWidth: true
                                                                            elide: Text.ElideRight
                                                                        }

                                                                        // 状态标签
                                                                        Rectangle {
                                                                            width: 60
                                                                            height: 24
                                                                            radius: 12
                                                                            color: {
                                                                                if (modelData.status === "异常") return "#fff2f0";
                                                                                else if (modelData.status === "运行中") return "#e6f7ff";
                                                                                else return "#f5f5f5";
                                                                            }

                                                                            Text {
                                                                                anchors.centerIn: parent
                                                                                text: modelData.status || "未启动"
                                                                                font.pixelSize: 12
                                                                                color: {
                                                                                    if (modelData.status === "异常") return "#ff4d4f";
                                                                                    else if (modelData.status === "运行中") return "#1890ff";
                                                                                    else return "#999999";
                                                                                }
                                                                            }
                                                                        }
                                                                    }

                                                                    // 描述信息（可选）
                                                                    Text {
                                                                        text: modelData.description || "暂无描述"
                                                                        font.pixelSize: 11
                                                                        color: "#666"
                                                                        wrapMode: Text.WordWrap
                                                                        maximumLineCount: 2
                                                                        elide: Text.ElideRight
                                                                        Layout.fillWidth: true
                                                                        Layout.fillHeight: true
                                                                        visible: !!modelData.description
                                                                    }

                                                                    // 操作按钮
                                                                    Rectangle {
                                                                        Layout.fillWidth: true
                                                                        Layout.preferredHeight: 30
                                                                        radius: 15
                                                                        color: startBtnHover.containsMouse ? "#40a9ff" : "#1890ff"

                                                                        Text {
                                                                            anchors.centerIn: parent
                                                                            text: {
                                                                                if (modelData.status === "异常") return "重新启动";
                                                                                else if (modelData.status === "运行中") return "显示";
                                                                                else return "启动应用";
                                                                            }
                                                                            font.pixelSize: 12
                                                                            color: "white"
                                                                        }

                                                                        MouseArea {
                                                                            id: startBtnHover
                                                                            anchors.fill: parent
                                                                            hoverEnabled: true
                                                                            cursorShape: Qt.PointingHandCursor
                                                                            onClicked: launchApplication(modelData.title)
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

                                    // === 下方：主机状态监控 ===
                                    Rectangle {
                                        Layout.fillWidth: true
                                        Layout.fillHeight: true
                                        Layout.preferredHeight: parent.height * 0.2  // 占左侧20%高度
                                        color: "white"
                                        radius: 8

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

                                            // 状态指标容器
                                            Rectangle {
                                                Layout.fillWidth: true
                                                Layout.preferredHeight: 80
                                                radius: 8
                                                border.color: "#e0e0e0"
                                                border.width: 1
                                                color: "#fafafa"

                                                RowLayout {
                                                    anchors.fill: parent
                                                    anchors.margins: 15
                                                    spacing: 30

                                                    // CPU使用率区块
                                                    Rectangle {
                                                        Layout.preferredWidth: 50
                                                        Layout.preferredHeight: 40
                                                        color: "transparent"

                                                        RowLayout {
                                                            anchors.fill: parent
                                                            spacing: 10

                                                            // 状态指示标
                                                            Rectangle {
                                                                width: 8
                                                                height: 8
                                                                radius: 4
                                                                color: "#11b42c"
                                                            }

                                                            RowLayout {
                                                                spacing: 2

                                                                Text {
                                                                    text: "CPU使用率: "
                                                                    font.pixelSize: 14
                                                                    color: "#000000"
                                                                }

                                                                Text {
                                                                    id: cpuValueText
                                                                    text: {
                                                                        var history = Modules.systemStatusData.servers[0].cpuHistory || [];
                                                                        return (history.length > 0 ? history[history.length-1].toFixed(1) : "0.0") + "%"
                                                                    }
                                                                    font {
                                                                        pixelSize: 24
                                                                        weight: Font.DemiBold
                                                                        family: "Arial"
                                                                    }
                                                                    color: "#000000"
                                                                }
                                                            }
                                                        }
                                                    }

                                                    // 分隔线
                                                    Rectangle {
                                                        width: 1
                                                        Layout.fillHeight: true
                                                        color: "#e0e0e0"
                                                    }

                                                    // 内存使用区块
                                                    Rectangle {
                                                        Layout.preferredWidth: 60
                                                        Layout.preferredHeight: 40
                                                        color: "transparent"

                                                        RowLayout {
                                                            anchors.fill: parent
                                                            spacing: 10

                                                            // 状态指示标
                                                            Rectangle {
                                                                width: 8
                                                                height: 8
                                                                radius: 4
                                                                color: "#11b42c"
                                                            }

                                                            RowLayout {
                                                                spacing: 2

                                                                Text {
                                                                    text: "内存使用: "
                                                                    font.pixelSize: 14
                                                                    color: "#000000"
                                                                }

                                                                Text {
                                                                    id: memoryValueText
                                                                    text: {
                                                                        var history = Modules.systemStatusData.servers[0].memoryHistory || [];
                                                                        return (history.length > 0 ? history[history.length-1].toFixed(1) : "0.0") + " MB"
                                                                    }
                                                                    font {
                                                                        pixelSize: 24
                                                                        weight: Font.DemiBold
                                                                        family: "Arial"
                                                                    }
                                                                    color: "#000000"
                                                                }
                                                            }
                                                        }
                                                    }
                                                    // 分隔线
                                                    Rectangle {
                                                        width: 1
                                                        Layout.fillHeight: true
                                                        color: "#e0e0e0"
                                                    }

                                                    // 磁盘占用区块
                                                    Rectangle {
                                                        Layout.preferredWidth: 100
                                                        Layout.preferredHeight: 60
                                                        color: "transparent"

                                                        RowLayout {
                                                            anchors.fill: parent
                                                            spacing: 10

                                                            // 状态指示标
                                                            Rectangle {
                                                                width: 8
                                                                height: 8
                                                                radius: 4
                                                                color: "#000000"
                                                            }

                                                            RowLayout {
                                                                spacing: 2

                                                                Text {
                                                                    text: "磁盘占用: "
                                                                    font.pixelSize: 14
                                                                    color: "#000000"
                                                                }

                                                                Text {
                                                                    id: diskValueText
                                                                    text: {
                                                                        var server = Modules.systemStatusData.servers[0];
                                                                        var used = server.diskUsed || 63;
                                                                        var total = server.diskTotal || 1024;
                                                                        return (used.toFixed(1) + " GB / " + total.toFixed(1) + " GB")
                                                                    }
                                                                    font {
                                                                        pixelSize: 24
                                                                        weight: Font.DemiBold
                                                                        family: "Arial"
                                                                    }
                                                                    color: "#000000"
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        // 数据更新连接（保持不变）
                                        Connections {
                                            target: qmlCommunication
                                            function onhardware_resourceFromCpp(info) {
                                                if (Modules.systemStatusData.servers.length > 0) {
                                                    var server = Modules.systemStatusData.servers[0]
                                                    server.cpu = info.cpu_percent || 0
                                                    server.memory = info.memory_cost || 0
                                                    
                                                    function updateHistory(history, newValue) {
                                                        history.push(newValue)
                                                        while(history.length > 20) {
                                                            history.shift()
                                                        }
                                                    }
                                                    updateHistory(server.cpuHistory, server.cpu)
                                                    updateHistory(server.memoryHistory, server.memory)
                                                    
                                                    cpuValueText.text = server.cpu.toFixed(1) + "%"
                                                    memoryValueText.text = server.memory.toFixed(1) + " MB"
                                                }
                                            }
                                        }
                                    }
                                }

                                // ===== 右侧：服务监控（占20%） =====
                                Rectangle {
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    Layout.preferredWidth: parent.width * 0.2 - 10
                                    color: "white"
                                    radius: 8

                                    ColumnLayout {
                                        anchors.fill: parent
                                        anchors.margins: 15
                                        spacing: 15

                                        Text {
                                            text: "服务监控"
                                            font.pixelSize: 18
                                            font.weight: Font.DemiBold
                                            color: "#333"
                                            
                                        }

                                    ColumnLayout {
                                        Layout.fillWidth: true
                                        Layout.fillHeight: true
                                        spacing: 0  // 将间距设为0，我们自己控制间距

                                        // 移除顶部弹性填充的Item

                                        Repeater {
                                            model: [
                                                { title: "数据库连接", state: true },
                                                { title: "定位服务", state: true },
                                                { title: "视频监控", state: true },
                                                { title: "受电弓识别", state: false },
                                                { title: "导高拉出值", state: false },
                                                { title: "网络服务", state: true }
                                            ]

                                            Item {
                                                Layout.fillWidth: true
                                                Layout.fillHeight: true  // 每个项目都填充高度
                                                
                                                Rectangle {
                                                    width: parent.width
                                                    height: 50 // 留出10像素作为间距
                                                    anchors.centerIn: parent
                                                    radius: 6
                                                    border.color: "#e0e0e0"
                                                    border.width: 1
                                                    color: "transparent"

                                                    // 内容居中容器
                                                    Item {
                                                        anchors.fill: parent
                                                        anchors.margins: 5  // 内容边距

                                                        Row {
                                                            anchors.centerIn: parent
                                                            spacing: 15

                                                            // 状态指示灯
                                                            Rectangle {
                                                                width: 12
                                                                height: 12
                                                                radius: 6
                                                                anchors.verticalCenter: parent.verticalCenter
                                                                color: modelData.state ? "#52c41a" : "#f5222d"
                                                                border.width: 1
                                                                border.color: Qt.darker(color, 1.2)
                                                            }

                                                            // 服务名称
                                                            Text {
                                                                text: modelData.title
                                                                font.pixelSize: 14
                                                                color: "#333"
                                                                anchors.verticalCenter: parent.verticalCenter
                                                            }

                                                            // 状态标签
                                                            Text {
                                                                text: modelData.state ? "正常" : "异常"
                                                                font {
                                                                    pixelSize: 12
                                                                    bold: true
                                                                }
                                                                color: modelData.state ? "#52c41a" : "#f5222d"
                                                                anchors.verticalCenter: parent.verticalCenter
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                        // 将Connections移到GridLayout外部
                                        Connections {
                                            target: qmlCommunication
                                            
                                            function onDatabase_stateFromCpp(state) {
                                                updateServiceState("数据库连接", state)
                                            }
                                            
                                            function onVideo_stateFromCpp(state) {
                                                updateServiceState("视频监控", state)
                                            }
                                            
                                            function onlocation_stateFromCpp(state) {
                                                updateServiceState("定位服务", state)
                                            }

                                            // 定义在Connections内部的更新函数
                                            function updateServiceState(serviceTitle, state) {
                                                for (var i = 0; i < serviceStatusGrid.services.length; i++) {
                                                    if (serviceStatusGrid.services[i].title === serviceTitle) {
                                                        serviceStatusGrid.services[i].state = state
                                                        serviceStatusGrid.services[i].statename = state ? "正常" : "异常"
                                                        // 强制刷新
                                                        serviceStatusGrid.services = serviceStatusGrid.services.slice()
                                                        break
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
        for (var i = 0; i < Modules.systemStatusData.appData.length; i++) {
            if (Modules.systemStatusData.appData[i].title === appTitle) {
                modelData = Modules.systemStatusData.appData[i];
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
                        "height": 820-40,
                        "title": modelData.title
                    });

                    // 注册窗口
                    openedWindows[modelData.title] = window;

                    if (window) {
                        if (window.closing) {
                            window.closing.connect(function () {
                                delete openedWindows[modelData.title];
                            });
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