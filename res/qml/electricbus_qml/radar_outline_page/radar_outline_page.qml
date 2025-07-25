import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtCharts
import QtQuick.Window 
import Qt.labs.qmlmodels
Window {
    id: root
    visible: true
    width: 1920
    height: 1080
    title: qsTr("雷达限界系统")
    flags: Qt.Window | Qt.FramelessWindowHint | Qt.WindowMaximizeButtonHint
    color: "transparent"

    // 设置最小尺寸
    minimumWidth: 1920
    minimumHeight: 1080
    
    // 主窗口容器
    Rectangle {
        id: windowFrame
        anchors.fill: parent
        anchors.margins: 10
        color: "#ffffff"
        radius: 8

        // 标题栏
        Rectangle {
            id: titleBar
            width: parent.width
            height: 40
            color: "#f5f5f5"
            radius: 8

            Text {
                anchors.left: parent.left
                anchors.leftMargin: 16
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("雷达限界系统")
                font.pixelSize: 14
                color: "#2c3e50"
            }

            // 配置按钮
            Row {
                anchors.left: parent.left
                anchors.leftMargin: 150
                anchors.verticalCenter: parent.verticalCenter
                spacing: 20
                z: 1 // 确保配置按钮在最上层

                // 打开文件按钮
                Rectangle {
                    width: 80
                    height: 26
                    radius: 4
                    color: openFileArea.containsMouse ? "#e0e0e0" : "transparent"

                    Text {
                        anchors.centerIn: parent
                        text: "打开文件"
                        color: "#2c3e50"
                    }

                    MouseArea {
                        id: openFileArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            // 点击时向后端请求数据
                            qmlCommunication.behaviorFromQml(QmlCommunication.QmlActions.offlineRadarTaskGet)
                            fileMenu.popup()
                        }
                    }

                    Menu {
                        id: fileMenu
                        y: parent.height

                        // 数据模型
                        ListModel {
                            id: fileListModel
                        }

                        // 连接后端数据
                        Connections {
                            target: qmlCommunication
                            function ontasktable_recvFromCpp(data) {
                                // 清空当前model
                                fileListModel.clear()
                                
                                // 解析传入的任务数据
                                for (var i = 0; i < data.length; i++) {
                                    var task = data[i]
                                    
                                    // 构建方向文本
                                    var directionText = task.line_dir === 0 ? "正向" : 
                                                    task.line_dir === 1 ? "反向" : "无效方向"
                                    
                                    // 构建显示名称
                                    var displayName = task.task_name + " | " + 
                                                    task.line_name + " | " + 
                                                    directionText + " | " +
                                                    task.start_station + " -> " + 
                                                    task.end_station
                                    
                                    // 添加到model
                                    fileListModel.append({
                                        displayName: displayName,
                                        createTime: task.created_time,
                                        taskId: task.id
                                    })
                                }
                            }
                        }

                        // 使用Repeater动态创建菜单项
                        Repeater {
                            model: fileListModel

                            MenuItem {
                                text: model.displayName + " | " + model.createTime
                                
                                onTriggered: {
                                    console.log("Selected task:", model.taskId)
                                    // 发送选中的任务ID给后端
                                    qmlCommunication.behaviorFromQml(
                                        QmlCommunication.QmlActions.offlineRadarTaskidGet, 
                                        model.taskId
                                    )
                                }
                            }
                        }
                    }
                }
                
                // 系统配置按钮
                Rectangle {
                    width: 80
                    height: 26
                    radius: 4
                    color: sysConfigArea.containsMouse ? "#e0e0e0" : "transparent"

                    Text {
                        anchors.centerIn: parent
                        text: "系统配置"
                        color: "#2c3e50"
                    }

                    MouseArea {
                        id: sysConfigArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: sysConfigDialog.open()
                    }
                }

                // 通信配置按钮
                Rectangle {
                    width: 80
                    height: 26
                    radius: 4
                    color: commConfigArea.containsMouse ? "#e0e0e0" : "transparent"

                    Text {
                        anchors.centerIn: parent
                        text: "通信配置"
                        color: "#2c3e50"
                    }

                    MouseArea {
                        id: commConfigArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: commConfigDialog.open()
                    }
                }

                // 设备配置按钮
                Rectangle {
                    width: 80
                    height: 26
                    radius: 4
                    color: deviceConfigArea.containsMouse ? "#e0e0e0" : "transparent"

                    Text {
                        id: deviceConfigText
                        anchors.centerIn: parent
                        text: "程序配置"
                        color: "#2c3e50"
                    }

                    MouseArea {
                        id: deviceConfigArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: deviceConfigMenu.open()
                    }

                    // 设备配置下拉菜单
                    Menu {
                        id: deviceConfigMenu
                        y: parent.height // 将菜单定位在按钮下方

                        MenuItem {
                            text: "限界范围配置"
                            onTriggered: radarConfigDialog.open()
                        }
                        MenuItem {
                            text: "数据保存配置"
                            onTriggered: cameraConfigDialog.open()
                        }
                        MenuItem {
                            text: "告警配置"
                            onTriggered: commConfigDialog.open()
                        }
                    }
                }
            }

            // 控制按钮
            Row {
                anchors.right: parent.right
                anchors.rightMargin: 8
                anchors.verticalCenter: parent.verticalCenter
                spacing: 8
                z: 1 // 确保控制按钮在最上层

                // 最小化按钮
                Rectangle {
                    width: 32
                    height: 32
                    radius: 4
                    color: minimizeArea.containsMouse ? "#f0f0f0" : "transparent"

                    Text {
                        anchors.centerIn: parent
                        text: "─"
                        color: "#2c3e50"
                    }

                    MouseArea {
                        id: minimizeArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            console.log("Minimizing window...")
                            root.visibility = Window.Minimized
                        }
                    }
                }

                // 关闭按钮
                Rectangle {
                    width: 32
                    height: 32
                    radius: 4
                    color: closeArea.containsMouse ? "#ff4444" : "transparent"

                    Text {
                        anchors.centerIn: parent
                        text: "✕"  
                        color: closeArea.containsMouse ? "#ffffff" : "#2c3e50"
                    }

                    MouseArea {
                        id: closeArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            console.log("Closing window...")
                            root.close()
                        }
                    }
                }
            }

            // 窗口拖动区域
            MouseArea {
                anchors.fill: parent
                anchors.leftMargin: 150 // 避开左侧配置按钮区域
                anchors.rightMargin: 80 // 避开右侧控制按钮区域
                z: -1 // 确保MouseArea在其他元素下层
                property point clickPos: "0,0"

                onPressed: {
                    clickPos = Qt.point(mouse.x, mouse.y)
                }

                onPositionChanged: {
                    if (pressed) {
                        var delta = Qt.point(mouse.x - clickPos.x, mouse.y - clickPos.y)
                        root.x += delta.x
                        root.y += delta.y
                    }
                }
            }
        }

        // TabBar紧跟标题栏
        TabBar {
            id: tabBar
            anchors.top: titleBar.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 10
            height: 40

            background: Rectangle {
                color: "#ffffff"
                radius: 4
                border.color: "#e0e0e0"
                border.width: 1
            }

            TabButton {
                text: qsTr("实时监测")
                width: 120
                height: parent.height
                
                background: Rectangle {
                    color: parent.checked ? "#2196F3" : "#ffffff"
                    radius: 4
                }
                
                contentItem: Text {
                    text: parent.text
                    font.pixelSize: 14
                    color: parent.checked ? "#ffffff" : "#666666"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            TabButton {
                text: qsTr("数据回放")
                width: 120
                height: parent.height
                
                background: Rectangle {
                    color: parent.checked ? "#2196F3" : "#ffffff"
                    radius: 4
                }
                
                contentItem: Text {
                    text: parent.text
                    font.pixelSize: 14
                    color: parent.checked ? "#ffffff" : "#666666"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            TabButton {
                text: qsTr("数据分析")
                width: 120
                height: parent.height
                
                background: Rectangle {
                    color: parent.checked ? "#2196F3" : "#ffffff"
                    radius: 4
                }
                
                contentItem: Text {
                    text: parent.text
                    font.pixelSize: 14
                    color: parent.checked ? "#ffffff" : "#666666"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            TabButton {
                text: qsTr("系统设置")
                width: 120
                height: parent.height
                
                background: Rectangle {
                    color: parent.checked ? "#2196F3" : "#ffffff"
                    radius: 4
                }
                
                contentItem: Text {
                    text: parent.text
                    font.pixelSize: 14
                    color: parent.checked ? "#ffffff" : "#666666"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }

        // StackLayout放在TabBar下方
        StackLayout {
            id: stackLayout
            anchors.top: tabBar.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 10
            currentIndex: tabBar.currentIndex

            // 第一个标签页：实时监测
            Rectangle {
                //id: windowFrame
                anchors.fill: parent
                anchors.margins: 10
                color: "#ffffff"
                radius: 8


                // 主内容区域
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    anchors.topMargin: titleBar.height + 10
                    spacing: 15  // 增加间距

                    // 左侧区域
                    Rectangle {
                        Layout.fillHeight: true
                        Layout.preferredWidth: parent.width * 0.75  // 调整为75%宽度
                        border.color: "#e0e0e0"
                        radius: 4

                        // 使用 ColumnLayout 分隔上下两个区域
                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 10  // 添加内边距
                            spacing: 15  // 增加间距

                            // 上部分区域 - 包含两个图表
                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: parent.height * 0.65  // 调整高度比例
                                
                                // 使用 RowLayout 并排放置两个图表
                                RowLayout {
                                    anchors.fill: parent
                                    spacing: 15  // 增加图表间距

                                    // 左侧图表
                                    ChartView {
                                        id: leftChart
                                        Layout.fillHeight: true
                                        Layout.fillWidth: true
                                        Layout.preferredWidth: parent.width * 0.5  // 确保两个图表等宽
                                        antialiasing: true
                                        legend.visible: false
                                        margins.top: 0     // 减少图表内边距
                                        margins.bottom: 0
                                        margins.left: 10
                                        margins.right: 10
                                        
                                        ValueAxis {
                                            id: leftAxisX
                                            min: -6000
                                            max: 6000
                                            labelFormat: "%d"
                                            gridVisible: true
                                            tickCount: calculateTickCount(min, max)
                                        }

                                        ValueAxis {
                                            id: leftAxisY
                                            min: -2000
                                            max: 7000
                                            labelFormat: "%d"
                                            gridVisible: true
                                            tickCount: calculateTickCount(min, max)
                                        }

                                        ScatterSeries {
                                            id: leftTunnelOutline
                                            axisX: leftAxisX
                                            axisY: leftAxisY
                                        }
                                    }

                                    // 右侧图表
                                    ChartView {
                                        id: rightChart
                                        Layout.fillHeight: true
                                        Layout.fillWidth: true
                                        Layout.preferredWidth: parent.width * 0.5  // 确保两个图表等宽
                                        antialiasing: true
                                        legend.visible: false
                                        margins.top: 0     // 减少图表内边距
                                        margins.bottom: 0
                                        margins.left: 10
                                        margins.right: 10
                                        
                                        ValueAxis {
                                            id: rightAxisX
                                            min: -6000
                                            max: 6000
                                            labelFormat: "%d"
                                            gridVisible: true
                                            tickCount: calculateTickCount(min, max)
                                        }

                                        ValueAxis {
                                            id: rightAxisY
                                            min: -2000
                                            max: 7000
                                            labelFormat: "%d"
                                            gridVisible: true
                                            tickCount: calculateTickCount(min, max)
                                        }

                                        ScatterSeries {
                                            id: rightTunnelOutline
                                            axisX: rightAxisX
                                            axisY: rightAxisY
                                        }
                                    }
                                }
                            }

                            // 在上部分区域和下部分区域之间添加进度条
                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 40
                                color: "#f5f5f5"
                                radius: 4

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: 5
                                    spacing: 10

                                    // 播放/暂停按钮
                                    Button {
                                        text: "▶"
                                        implicitWidth: 30
                                        implicitHeight: 30
                                        onClicked: {
                                            // 处理播放/暂停逻辑
                                            text = text === "▶" ? "⏸" : "▶"
                                        }
                                    }

                                    // 进度条
                                    Slider {
                                        id: timelineSlider
                                        Layout.fillWidth: true
                                        from: 0
                                        to: 100
                                        value: 0
                                        
                                        // 显示当前值的文本
                                        Text {
                                            text: getCurrentTime()
                                            anchors.bottom: parent.top
                                            anchors.horizontalCenter: parent.left.plus(parent.width * (parent.value / 100))
                                            color: "#666666"
                                            font.pixelSize: 12
                                        }

                                        onMoved: {
                                            // 处理进度条拖动事件
                                            console.log("Timeline position:", value)
                                            // 这里可以添加更新数据显示的逻辑
                                        }
                                    }

                                    // 总时长显示
                                    Text {
                                        text: "00:00:00"
                                        color: "#666666"
                                        font.pixelSize: 12
                                    }
                                }
                            }

                            // 下部分区域
                            Rectangle {
                                Layout.fillWidth: true
                                Layout.preferredHeight: parent.height * 0.2  // 调整高度比例
                                color: "#f5f5f5"
                                radius: 4
                                
                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: 10
                                    spacing: 20

                                    GroupBox {
                                        title: "操作选项"
                                        Layout.preferredWidth: parent.width * 0.3
                                        Layout.fillHeight: true

                                        ColumnLayout {
                                            anchors.fill: parent
                                            spacing: 10


                                            // 新增：播放控制标签和按钮
                                            Label {
                                                text: "播放控制:"
                                                Layout.fillWidth: true
                                            }

                                            RowLayout {
                                                Layout.fillWidth: true
                                                spacing: 10

                                                Button {
                                                    text: "播放"
                                                    Layout.preferredWidth: 80
                                                    Layout.fillWidth: true
                                                    onClicked: {
                                                        console.log("开始播放")
                                                    }
                                                }

                                                Button {
                                                    text: "结束"
                                                    Layout.preferredWidth: 80
                                                    Layout.fillWidth: true
                                                    onClicked: {
                                                        console.log("结束播放")
                                                    }
                                                }
                                            }

                                            // 新增：倍速控制
                                            RowLayout {
                                                Layout.fillWidth: true
                                                spacing: 10

                                                Label {
                                                    text: "倍速:"
                                                    Layout.preferredWidth: 40
                                                }

                                                Button {
                                                    text: "0.5"
                                                    Layout.preferredWidth: 50
                                                    onClicked: {
                                                        console.log("设置0.5倍速")
                                                    }
                                                }

                                                Button {
                                                    text: "0.75"
                                                    Layout.preferredWidth: 60
                                                    onClicked: {
                                                        console.log("设置0.75倍速")
                                                    }
                                                }

                                                Button {
                                                    text: "1.0"
                                                    Layout.preferredWidth: 50
                                                    onClicked: {
                                                        console.log("设置1.0倍速")
                                                    }
                                                }


                                                Button {
                                                    text: "1.5"
                                                    Layout.preferredWidth: 50
                                                    onClicked: {
                                                        console.log("设置1.5倍速")
                                                    }
                                                }
                                                Button {
                                                    text: "2.0"
                                                    Layout.preferredWidth: 50
                                                    onClicked: {
                                                        console.log("设置2.0倍速")
                                                    }
                                                }
                                            }

                                            // 填充剩余空间
                                            Item {
                                                Layout.fillHeight: true
                                            }
                                        }
                                    }
                                    
                                    // 左侧控制组
                                    GroupBox {
                                        title: "显示选项"
                                        Layout.preferredWidth: parent.width * 0.3
                                        Layout.fillHeight: true

                                        ColumnLayout {
                                            anchors.fill: parent
                                            spacing: 10

                                            // 视图选择行
                                            RowLayout {
                                                Layout.alignment: Qt.AlignLeft
                                                spacing: 20

                                                Button {
                                                    text: "放大"
                                                    implicitWidth: 80
                                                    onClicked: {
                                                        // 同时控制两个图表的放大
                                                        zoomCharts(1.2)
                                                    }
                                                }
                                                Button {
                                                    text: "缩小"
                                                    implicitWidth: 80
                                                    onClicked: {
                                                        // 同时控制两个图表的缩小
                                                        zoomCharts(0.8)
                                                    }
                                                }
                                                Button {
                                                    text: "复位"
                                                    implicitWidth: 80
                                                    onClicked: {
                                                        // 重置两个图表的显示范围
                                                        resetCharts()
                                                    }
                                                }
                                            }

                                            // 显示控制行
                                            Flow {
                                                Layout.fillWidth: true
                                                spacing: 20

                                                CheckBox {
                                                    text: "网格线"
                                                    checked: true
                                                    onCheckedChanged: {
                                                        leftChart.axes[0].gridVisible = checked
                                                        leftChart.axes[1].gridVisible = checked
                                                        rightChart.axes[0].gridVisible = checked
                                                        rightChart.axes[1].gridVisible = checked
                                                    }
                                                }
                                                CheckBox {
                                                    text: "区域线"
                                                    onCheckedChanged: {
                                                        // 处理区域线显示
                                                    }
                                                }
                                                CheckBox {
                                                    text: "补偿基准线"
                                                    onCheckedChanged: {
                                                        // 处理补偿基准线显示
                                                    }
                                                }
                                            }
                                        }
                                    }

                                    // 右侧控制组
                                    GroupBox {
                                        title: "操作选项"
                                        Layout.preferredWidth: parent.width * 0.3
                                        Layout.fillHeight: true

                                        ColumnLayout {
                                            anchors.fill: parent
                                            spacing: 10

                                            // 第一行：下拉框
                                            ComboBox {
                                                Layout.fillWidth: true
                                                model: ["选项1", "选项2", "选项3"]
                                                currentIndex: 0
                                            }

                                            // 第二行：下拉框
                                            ComboBox {
                                                Layout.fillWidth: true
                                                model: ["类型1", "类型2", "类型3"]
                                                currentIndex: 0
                                            }

                                            // 第三行：按钮和分隔线
                                            RowLayout {
                                                Layout.fillWidth: true
                                                spacing: 10

                                                Button {
                                                    text: "导出数据"
                                                    Layout.preferredWidth: 80
                                                    onClicked: {
                                                        console.log("导出数据被点击")
                                                    }
                                                }

                                                // 垂直分隔线
                                                Rectangle {
                                                    width: 1
                                                    Layout.fillHeight: true
                                                    color: "#cccccc"
                                                }

                                                // 截图保存按钮
                                                Button {
                                                    text: "截图保存"
                                                    Layout.preferredWidth: 80
                                                    onClicked: {
                                                        console.log("截图保存被点击")
                                                    }
                                                }

                                                // 填充剩余空间
                                                Item {
                                                    Layout.fillWidth: true
                                                }
                                            }

                                            // 填充剩余空间
                                            Item {
                                                Layout.fillHeight: true
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    // 右侧控制面板

                    Rectangle {
                        Layout.fillHeight: true
                        Layout.preferredWidth: parent.width * 0.20
                        color: "#f5f5f5"
                        radius: 4

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 15
                            spacing: 15

                            // 基本信息组
                            GroupBox {
                                title: "播放位置信息显示"
                                Layout.fillWidth: true
                                Layout.fillHeight: true  
                                
                                Connections {
                                    target: qmlCommunication
                                    function onLocationdata_recvFromCpp(data) {
                                        // 创建对应的id引用
                                        let labels = {
                                            stationLabel: null,
                                            poleLabel: null,
                                            speedLabel: null,
                                            disLabel: null
                                        }
                                        
                                        // 遍历GridLayout的子项来获取Label引用
                                        for(let i = 0; i < gridLayout.children.length; i++) {
                                            let child = gridLayout.children[i]
                                            if(child instanceof Label) {
                                                if(child.objectName === "stationLabel") labels.stationLabel = child
                                                else if(child.objectName === "poleLabel") labels.poleLabel = child
                                                else if(child.objectName === "speedLabel") labels.speedLabel = child
                                                else if(child.objectName === "disLabel") labels.disLabel = child
                                            }
                                        }

                                        try {
                                            // 解析接收到的数据
                                            console.log("Received data:", JSON.stringify(data))
                                            
                                            // 更新站区信息
                                            if(data.station_name) {
                                                // 查找对应的Label并更新文本
                                                for(let i = 0; i < gridLayout.children.length; i++) {
                                                    if(gridLayout.children[i] instanceof Label && 
                                                    gridLayout.children[i].objectName === "stationLabel") {
                                                        gridLayout.children[i].text = data.station_name
                                                        break
                                                    }
                                                }
                                            }
                                            
                                            // 更新锚段名称
                                            if(data.maoduan_name) {
                                                // 更新相应的Label
                                            }
                                            
                                            // 更新杆号
                                            if(data.pole_name) {
                                                for(let i = 0; i < gridLayout.children.length; i++) {
                                                    if(gridLayout.children[i] instanceof Label && 
                                                    gridLayout.children[i].objectName === "poleLabel") {
                                                        gridLayout.children[i].text = data.pole_name
                                                        break
                                                    }
                                                }
                                            }
                                            
                                            // 更新行驶距离
                                            if(data.train_move_dis !== undefined) {
                                                for(let i = 0; i < gridLayout.children.length; i++) {
                                                    if(gridLayout.children[i] instanceof Label && 
                                                    gridLayout.children[i].objectName === "disLabel") {
                                                        gridLayout.children[i].text = data.train_move_dis.toFixed(3) + " km"
                                                        break
                                                    }
                                                }
                                            }
                                            
                                            // 更新速度
                                            if(data.speed !== undefined) {
                                                for(let i = 0; i < gridLayout.children.length; i++) {
                                                    if(gridLayout.children[i] instanceof Label && 
                                                    gridLayout.children[i].objectName === "speedLabel") {
                                                        gridLayout.children[i].text = data.speed.toFixed(2) + " km/h"
                                                        break
                                                    }
                                                }
                                            }
                                        } catch(e) {
                                            console.error("Error parsing location data:", e)
                                        }
                                    }
                                }
                                GridLayout {
                                    id: gridLayout
                                    anchors.fill: parent
                                    anchors.margins: 5
                                    columns: 2
                                    columnSpacing: 10
                                    rowSpacing: 8
                                    Layout.fillHeight: true 

                                    Label { 
                                        text: "线路名称:"
                                        Layout.preferredHeight: 30  // 添加固定高度
                                    }
                                    Label { 
                                        text: "名称值"
                                        Layout.fillWidth: true 
                                        Layout.preferredHeight: 30  // 添加固定高度
                                    }

                                    // 左列标签，右列数据
                                    Label { 
                                            text: "线路方向:"
                                            Layout.preferredHeight: 30  // 添加固定高度
                                        }
                                    Label { 
                                        text: ""
                                        Layout.fillWidth: true 
                                        Layout.preferredHeight: 30  // 添加固定高度                                
                                    }

                                    Label { 
                                        text: "起始站区:" 
                                        Layout.preferredHeight: 30  // 添加固定高度
                                        }
                                    Label { 
                                        text: ""
                                        Layout.fillWidth: true 
                                        Layout.preferredHeight: 30
                                    }

                                    Label { 
                                        text: "起始公里标："
                                        Layout.preferredHeight: 30  // 添加固定高度
                                        }
                                    Label { 
                                        text: " "
                                        Layout.fillWidth: true 
                                        Layout.preferredHeight: 30
                                    }

                                    Label { 
                                        text: "线路段类型:" 
                                        Layout.preferredHeight: 30                               
                                        }
                                    Label { 
                                        text: " "
                                        Layout.fillWidth: true 
                                        Layout.preferredHeight: 30
                                    }

                                    Label {
                                        text: "左水平补偿:" 
                                        Layout.preferredHeight: 30
                                        }
                                    Label { 
                                        text: "1.23mm "
                                        Layout.fillWidth: true 
                                        Layout.preferredHeight: 30
                                    }

                                    Label {
                                            text: "左垂直补偿:"
                                            Layout.preferredHeight: 30
                                        }
                                    Label { 
                                        text: "1.23mm "
                                        Layout.fillWidth: true 
                                        Layout.preferredHeight: 30
                                    }

                                    Label { 
                                        text: "右水平补偿:"
                                        Layout.preferredHeight: 30
                                        }
                                    Label { 
                                        text: "1.23mm "
                                        Layout.fillWidth: true 
                                        Layout.preferredHeight: 30
                                    }

                                    Label {
                                        text: "右垂直补偿:" 
                                        Layout.preferredHeight: 30
                                        }
                                    Label { 
                                        text: "1.23mm "
                                        Layout.fillWidth: true 
                                        Layout.preferredHeight: 30
                                    }

                                    Label { 
                                        text: "站区:" 
                                        Layout.preferredHeight: 30
                                        }
                                    Label { 
                                        objectName: "stationLabel"  // 添加objectName
                                        text: " "
                                        Layout.fillWidth: true 
                                        Layout.preferredHeight: 30
                                    }

                                    Label { 
                                        text: "杆号:" 
                                        Layout.preferredHeight: 30
                                        
                                        }
                                    Label { 
                                        objectName: "poleLabel"  // 添加objectName
                                        text: " "
                                        Layout.fillWidth: true 
                                        Layout.preferredHeight: 30
                                    }

                                    Label {
                                        text: "公里标:" 
                                        Layout.preferredHeight: 30
                                        }
                                    Label { 
                                        objectName: "disLabel"  // 添加objectName
                                        text: " "
                                        Layout.fillWidth: true 
                                        Layout.preferredHeight: 30
                                    }

                                    Label { 
                                        text: "检测速度:"
                                        Layout.preferredHeight: 30
                                        }
                                    Label { 
                                        objectName: "speedLabel"  // 添加objectName
                                        text: " "
                                        Layout.fillWidth: true 
                                        Layout.preferredHeight: 30
                                    }
                                    
                                    Label { 
                                        text: "检测时间:"
                                        Layout.preferredHeight: 30
                                        }
                                    Label { 
                                        text: "年月日时分秒"
                                        Layout.fillWidth: true 
                                        Layout.preferredHeight: 30
                                    }

                                }
                            }

                        
                        }
                    }
                }
            }  

            // 第二个标签页：数据回放
            Rectangle {
                id: mainContainer
                width: 1200
                height: 800
                color: "#f5f5f5"
                
                // 顶部导航栏
                Rectangle {
                    //id: tabBar
                    width: parent.width
                    height: 50
                    color: "white"
                    
                    Row {
                        anchors.left: parent.left
                        anchors.leftMargin: 20
                        height: parent.height
                        spacing: 40
                        
                        TabButton {
                            text: "视图"
                            height: parent.height
                            contentItem: Text {
                                text: parent.text
                                font.pixelSize: 14
                                color: "#666666"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            background: Rectangle {
                                color: "transparent"
                            }
                        }
                        
                        TabButton {
                            text: "数据"
                            height: parent.height
                            contentItem: Text {
                                text: parent.text
                                font.pixelSize: 14
                                color: "#2196f3"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            background: Rectangle {
                                color: "transparent"
                                Rectangle {
                                    width: parent.width
                                    height: 3
                                    anchors.bottom: parent.bottom
                                    color: "#2196f3"
                                }
                            }
                        }
                        
                        TabButton {
                            text: "分析"
                            height: parent.height
                            contentItem: Text {
                                text: parent.text
                                font.pixelSize: 14
                                color: "#666666"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            background: Rectangle {
                                color: "transparent"
                            }
                        }
                    }
                }
                
                // 顶部搜索区域
                Rectangle {
                    id: searchArea
                    anchors.top: tabBar.bottom
                    width: parent.width
                    height: 100
                    color: "white"
                    border.color: "#e0e0e0"
                    border.width: 1
                    
                    RowLayout {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.margins: 20
                        spacing: 20
                        
                        // 起始站区
                        Text {
                            text: "起始站区:"
                            font.pixelSize: 14
                        }
                        
                        TextField {
                            id: startStationField
                            Layout.preferredWidth: 250
                            placeholderText: "请输入"
                            selectByMouse: true
                            background: Rectangle {
                                implicitWidth: 250
                                implicitHeight: 36
                                border.color: "#e0e0e0"
                                border.width: 1
                                radius: 4
                            }
                        }
                        
                        // 结束站区
                        Text {
                            text: "结束站区:"
                            font.pixelSize: 14
                        }
                        
                        ComboBox {
                            id: endStationCombo
                            Layout.preferredWidth: 250
                            model: ["请选择"]
                            
                            delegate: ItemDelegate {
                                width: endStationCombo.width
                                contentItem: Text {
                                    text: modelData
                                    elide: Text.ElideRight
                                    verticalAlignment: Text.AlignVCenter
                                }
                                highlighted: endStationCombo.highlightedIndex === index
                            }
                            
                            background: Rectangle {
                                implicitWidth: 250
                                implicitHeight: 36
                                border.color: "#e0e0e0"
                                border.width: 1
                                radius: 4
                            }
                        }
                        
                        Item { Layout.fillWidth: true }
                        
                        // 查询按钮
                        Button {
                            text: "查询"
                            Layout.preferredWidth: 80
                            Layout.preferredHeight: 36
                            background: Rectangle {
                                color: parent.pressed ? "#1976d2" : "#2196f3"
                                radius: 4
                            }
                            contentItem: Text {
                                text: parent.text
                                font.pixelSize: 14
                                color: "white"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                        
                        // 重置按钮
                        Button {
                            text: "重置"
                            Layout.preferredWidth: 80
                            Layout.preferredHeight: 36
                            background: Rectangle {
                                color: "#f5f5f5"
                                border.color: "#e0e0e0"
                                border.width: 1
                                radius: 4
                            }
                            contentItem: Text {
                                text: parent.text
                                font.pixelSize: 14
                                color: "black"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                    }
                }
                
                // 数据展示区域
                Rectangle {
                    id: dataArea
                    anchors.top: searchArea.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.margins: 20
                    color: "white"
                    border.color: "#e0e0e0"
                    border.width: 1

                    // 顶部操作栏
                    RowLayout {
                        id: operationBar
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.leftMargin: 10
                        anchors.rightMargin: 10
                        anchors.topMargin: 10
                        height: 40
                        
                        // 仅显示有限限制复选框
                        CheckBox {
                            id: limitCheckbox
                            text: "仅显示有限限"
                            checked: false
                            indicator: Rectangle {
                                implicitWidth: 16
                                implicitHeight: 16
                                x: 0
                                y: parent.height / 2 - height / 2
                                radius: 2
                                border.color: limitCheckbox.checked ? "#2196f3" : "#bdbdbd"
                                border.width: 1
                                color: limitCheckbox.checked ? "#2196f3" : "transparent"
                                
                                Rectangle {
                                    width: 10
                                    height: 10
                                    anchors.centerIn: parent
                                    radius: 1
                                    color: "white"
                                    visible: limitCheckbox.checked
                                }
                            }
                            
                            contentItem: Text {
                                text: limitCheckbox.text
                                font.pixelSize: 14
                                leftPadding: limitCheckbox.indicator.width + 8
                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                        
                        Item { Layout.fillWidth: true }
                        
                        // 导出按钮
                        Button {
                            text: "导出"
                            Layout.preferredWidth: 70
                            Layout.preferredHeight: 32
                            background: Rectangle {
                                color: "white"
                                border.color: "#e0e0e0"
                                border.width: 1
                                radius: 4
                            }
                            contentItem: Text {
                                text: parent.text
                                font.pixelSize: 14
                                color: "#333333"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                        
                        // 打印按钮
                        Button {
                            text: "打印"
                            Layout.preferredWidth: 70
                            Layout.preferredHeight: 32
                            background: Rectangle {
                                color: "white"
                                border.color: "#e0e0e0"
                                border.width: 1
                                radius: 4
                            }
                            contentItem: Text {
                                text: parent.text
                                font.pixelSize: 14
                                color: "#333333"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                        
                        // 更多按钮
                        Button {
                            text: "..."
                            Layout.preferredWidth: 40
                            Layout.preferredHeight: 32
                            background: Rectangle {
                                color: "white"
                                border.color: "#e0e0e0"
                                border.width: 1
                                radius: 4
                            }
                            contentItem: Text {
                                text: parent.text
                                font.pixelSize: 14
                                color: "#333333"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                    }

                    // 表格区域
                    Rectangle {
                        id: tableContainer
                        anchors.top: operationBar.bottom
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.bottom: paginationBar.top
                        anchors.margins: 10
                        border.color: "#e0e0e0"
                        border.width: 1
                        clip: true

                        // 表头
                        Item {
                            id: tableHeader
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.top: parent.top
                            height: 40
                            z: 2 // 确保表头在最上层

                            Rectangle {
                                anchors.fill: parent
                                color: "#f5f5f5"
                                border.color: "#e0e0e0"
                                border.width: 1
                            }

                            // 表头内容
                            Row {
                                id: headerRow
                                anchors.fill: parent
                                
                                // 列宽定义
                                property var columnWidths: [100, 140, 140, 140, 140, 120, 140, 140, 140, 140, 100, 140, 240]
                                
                                // 表头标题
                                property var headerTitles: ["序号", "站区", "杆号", "公里标", "实时速度", "实时里程(km)", "线路段类型", "是否有超限", "超限点数量", "超限体积预估(cm³)", "超限连续(ms)", "检测时间", "操作"]
                                
                                Repeater {
                                    model: headerRow.headerTitles.length
                                    
                                    Rectangle {
                                        width: headerRow.columnWidths[index]
                                        height: tableHeader.height
                                        color: "transparent"
                                        border.color: "#e0e0e0"
                                        border.width: index > 0 ? 1 : 0
                                        
                                        Text {
                                            anchors.centerIn: parent
                                            text: headerRow.headerTitles[index]
                                            font.pixelSize: 14
                                            font.bold: true
                                            horizontalAlignment: Text.AlignHCenter
                                            verticalAlignment: Text.AlignVCenter
                                        }
                                    }
                                }
                            }
                        }

                        // 表格内容，使用Flickable实现滚动
                        Flickable {
                            id: tableContent
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.top: tableHeader.bottom
                            anchors.bottom: parent.bottom
                            contentWidth: headerRow.width
                            contentHeight: tableRows.height
                            clip: true
                            
                            // 添加垂直滚动条
                            ScrollBar.vertical: ScrollBar {
                                policy: ScrollBar.AsNeeded
                                active: true
                                contentItem: Rectangle {
                                    implicitWidth: 8
                                    implicitHeight: 100
                                    radius: 4
                                    color: "#c0c0c0"
                                }
                            }

                            Column {
                                id: tableRows
                                width: parent.width
                                
                                // 模拟数据
                                property var tableData: [
                                    {seq: "1", area: "某某站", pole: "杆号", marker: "公里标", speed: "速度值", distance: "20.454", roadType: "隧道", hasLimit: "是", limitCount: "212", limitVolume: "计算速值", limitDuration: "28", detectionTime: "年月日时分秒", operation: ""},
                                    {seq: "2", area: "", pole: "", marker: "", speed: "", distance: "", roadType: "高架", hasLimit: "否", limitCount: "0", limitVolume: "", limitDuration: "799", detectionTime: "年月日时分秒", operation: ""},
                                    {seq: "3", area: "", pole: "", marker: "", speed: "", distance: "", roadType: "", hasLimit: "是", limitCount: "", limitVolume: "", limitDuration: "", detectionTime: "年月日时分秒", operation: ""},
                                    {seq: "4", area: "", pole: "", marker: "", speed: "", distance: "", roadType: "", hasLimit: "否", limitCount: "", limitVolume: "", limitDuration: "", detectionTime: "年月日时分秒", operation: ""},
                                    {seq: "5", area: "", pole: "", marker: "", speed: "", distance: "", roadType: "", hasLimit: "", limitCount: "", limitVolume: "", limitDuration: "", detectionTime: "", operation: ""},
                                    {seq: "6", area: "", pole: "", marker: "", speed: "", distance: "", roadType: "高架", hasLimit: "否", limitCount: "0", limitVolume: "", limitDuration: "799", detectionTime: "年月日时分秒", operation: ""},
                                    {seq: "7", area: "", pole: "", marker: "", speed: "", distance: "", roadType: "", hasLimit: "是", limitCount: "", limitVolume: "", limitDuration: "", detectionTime: "年月日时分秒", operation: ""},
                                    {seq: "8", area: "", pole: "", marker: "", speed: "", distance: "", roadType: "", hasLimit: "否", limitCount: "", limitVolume: "", limitDuration: "", detectionTime: "年月日时分秒", operation: ""},
                                    {seq: "9", area: "", pole: "", marker: "", speed: "", distance: "", roadType: "", hasLimit: "", limitCount: "", limitVolume: "", limitDuration: "", detectionTime: "", operation: ""},
                                    {seq: "10", area: "", pole: "", marker: "", speed: "", distance: "", roadType: "高架", hasLimit: "否", limitCount: "0", limitVolume: "", limitDuration: "799", detectionTime: "年月日时分秒", operation: ""},
                                    {seq: "11", area: "", pole: "", marker: "", speed: "", distance: "", roadType: "", hasLimit: "是", limitCount: "", limitVolume: "", limitDuration: "", detectionTime: "年月日时分秒", operation: ""},
                                    {seq: "12", area: "", pole: "", marker: "", speed: "", distance: "", roadType: "", hasLimit: "否", limitCount: "", limitVolume: "", limitDuration: "", detectionTime: "年月日时分秒", operation: ""},
                                    {seq: "13", area: "", pole: "", marker: "", speed: "", distance: "", roadType: "", hasLimit: "", limitCount: "", limitVolume: "", limitDuration: "", detectionTime: "", operation: ""},
                                    {seq: "14", area: "", pole: "", marker: "", speed: "", distance: "", roadType: "高架", hasLimit: "否", limitCount: "0", limitVolume: "", limitDuration: "799", detectionTime: "年月日时分秒", operation: ""},
                                    {seq: "15", area: "", pole: "", marker: "", speed: "", distance: "", roadType: "", hasLimit: "是", limitCount: "", limitVolume: "", limitDuration: "", detectionTime: "年月日时分秒", operation: ""},
                                    {seq: "16", area: "", pole: "", marker: "", speed: "", distance: "", roadType: "", hasLimit: "否", limitCount: "", limitVolume: "", limitDuration: "", detectionTime: "年月日时分秒", operation: ""},
                                    {seq: "17", area: "", pole: "", marker: "", speed: "", distance: "", roadType: "", hasLimit: "", limitCount: "", limitVolume: "", limitDuration: "", detectionTime: "", operation: ""}
                                ]
                                
                                // 动态生成更多数据行，共10行
                                Component.onCompleted: {
                                    for (let i = 6; i <= 20; i++) {
                                        tableData.push({
                                            seq: i.toString(), 
                                            area: i % 3 === 0 ? "站区" + i : "", 
                                            pole: i % 4 === 0 ? "杆" + i : "", 
                                            marker: i % 5 === 0 ? "公里" + i : "", 
                                            speed: i % 2 === 0 ? (80 + i) + "km/h" : "", 
                                            distance: i % 3 === 0 ? (10 + i) + ".123" : "", 
                                            roadType: i % 4 === 0 ? "隧道" : (i % 3 === 0 ? "高架" : ""), 
                                            hasLimit: i % 2 === 0 ? "是" : "否", 
                                            limitCount: i % 2 === 0 ? (100 + i).toString() : "0", 
                                            limitVolume: i % 2 === 0 ? "体积" + i : "", 
                                            limitDuration: i % 3 === 0 ? (20 + i).toString() : "", 
                                            detectionTime: "2025-" + (i % 12 + 1) + "-" + (i % 28 + 1),
                                            operation: ""
                                        });
                                    }
                                }
                                
                                Repeater {
                                    model: tableRows.tableData.length
                                    
                                    // 行
                                    Rectangle {
                                        width: tableContainer.width
                                        height: 40
                                        color: index % 2 === 0 ? "#ffffff" : "#f9f9f9"
                                        
                                        // 行悬停效果
                                        MouseArea {
                                            anchors.fill: parent
                                            hoverEnabled: true
                                            onEntered: parent.color = "#f0f8ff"
                                            onExited: parent.color = index % 2 === 0 ? "#ffffff" : "#f9f9f9"
                                        }
                                        
                                        // 列
                                        Row {
                                            anchors.fill: parent
                                            
                                            Repeater {
                                                model: headerRow.columnWidths.length
                                                
                                                Rectangle {
                                                    width: headerRow.columnWidths[index]
                                                    height: 40
                                                    color: "transparent"
                                                    border.color: "#e0e0e0"
                                                    border.width: 1
                                                    
                                                    // 处理不同的单元格内容
                                                    Item {
                                                        anchors.fill: parent
                                                        anchors.margins: 5
                                                        
                                                        // 根据不同的列显示不同内容
                                                        Component.onCompleted: {
                                                            var rowData = tableRows.tableData[parent.parent.parent.parent.parent.index];
                                                            var columnKey = "";
                                                            
                                                            switch(index) {
                                                                case 0: columnKey = "seq"; break;
                                                                case 1: columnKey = "area"; break;
                                                                case 2: columnKey = "pole"; break;
                                                                case 3: columnKey = "marker"; break;
                                                                case 4: columnKey = "speed"; break;
                                                                case 5: columnKey = "distance"; break;
                                                                case 6: columnKey = "roadType"; break;
                                                                case 7: columnKey = "hasLimit"; break;
                                                                case 8: columnKey = "limitCount"; break;
                                                                case 9: columnKey = "limitVolume"; break;
                                                                case 10: columnKey = "limitDuration"; break;
                                                                case 11: columnKey = "detectionTime"; break;

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

                    // 分页栏
                    Rectangle {
                        id: paginationBar
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        anchors.margins: 10
                        height: 40
                        
                        RowLayout {
                            anchors.fill: parent
                            spacing: 10
                            
                            Text {
                                text: "共 400 条记录 第 1 / 80 页"
                                font.pixelSize: 14
                                color: "#666666"
                            }
                            
                            Item { Layout.fillWidth: true }
                            
                            // 上一页
                            Rectangle {
                                width: 30
                                height: 30
                                border.color: "#e0e0e0"
                                border.width: 1
                                radius: 4
                                
                                Text {
                                    anchors.centerIn: parent
                                    text: "<"
                                    font.pixelSize: 14
                                    color: "#666666"
                                }
                                
                                MouseArea {
                                    anchors.fill: parent
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: console.log("上一页")
                                }
                            }
                            
                            // 页码 1
                            Rectangle {
                                width: 30
                                height: 30
                                color: "#2196f3"
                                radius: 4
                                
                                Text {
                                    anchors.centerIn: parent
                                    text: "1"
                                    color: "white"
                                    font.pixelSize: 14
                                }
                                
                                MouseArea {
                                    anchors.fill: parent
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: console.log("页码 1")
                                }
                            }
                            
                            // 页码 2-9
                            Repeater {
                                model: 8
                                
                                Rectangle {
                                    width: 30
                                    height: 30
                                    border.color: "#e0e0e0"
                                    border.width: 1
                                    radius: 4
                                    
                                    Text {
                                        anchors.centerIn: parent
                                        text: (index + 2).toString()
                                        font.pixelSize: 14
                                        color: "#666666"
                                    }
                                    
                                    MouseArea {
                                        anchors.fill: parent
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: console.log("页码 " + (index + 2))
                                    }
                                }
                            }
                            
                            // 下一页
                            Rectangle {
                                width: 30
                                height: 30
                                border.color: "#e0e0e0"
                                border.width: 1
                                radius: 4
                                
                                Text {
                                    anchors.centerIn: parent
                                    text: ">"
                                    font.pixelSize: 14
                                    color: "#666666"
                                }
                                
                                MouseArea {
                                    anchors.fill: parent
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: console.log("下一页")
                                }
                            }
                            
                            // 每页显示数量
                            ComboBox {
                                id: pageSizeCombo
                                model: ["10条/页", "20条/页", "50条/页", "100条/页"]
                                Layout.preferredWidth: 100
                                Layout.preferredHeight: 30
                                
                                delegate: ItemDelegate {
                                    width: pageSizeCombo.width
                                    contentItem: Text {
                                        text: modelData
                                        elide: Text.ElideRight
                                        verticalAlignment: Text.AlignVCenter
                                        font.pixelSize: 14
                                        color: "#333333"
                                    }
                                    highlighted: pageSizeCombo.highlightedIndex === index
                                }
                                
                                background: Rectangle {
                                    implicitWidth: 100
                                    implicitHeight: 30
                                    border.color: "#e0e0e0"
                                    border.width: 1
                                    radius: 4
                                }
                                
                                contentItem: Text {
                                    leftPadding: 10
                                    rightPadding: pageSizeCombo.indicator.width + 10
                                    text: pageSizeCombo.displayText
                                    font.pixelSize: 14
                                    color: "#333333"
                                    verticalAlignment: Text.AlignVCenter
                                    elide: Text.ElideRight
                                }
                                
                                onActivated: console.log("切换每页显示: " + model[index])
                            }
                            
                            // 跳转
                            Text {
                                text: "跳至"
                                font.pixelSize: 14
                                color: "#666666"
                            }
                            
                            // 页码输入框
                            TextField {
                                id: pageNumberField
                                Layout.preferredWidth: 40
                                Layout.preferredHeight: 30
                                text: "5"
                                horizontalAlignment: TextInput.AlignHCenter
                                selectByMouse: true
                                
                                background: Rectangle {
                                    implicitWidth: 40
                                    implicitHeight: 30
                                    border.color: "#e0e0e0"
                                    border.width: 1
                                    radius: 4
                                }
                                
                                onAccepted: console.log("跳转到页码: " + text)
                            }
                            
                            Text {
                                text: "页"
                                font.pixelSize: 14
                                color: "#666666"
                            }
                        }
                    }
                }
            }

            // 第三个标签页：数据分析
            Rectangle {
                color: "#ffffff"
                Layout.fillWidth: true
                Layout.fillHeight: true

                Text {
                    anchors.centerIn: parent
                    text: "数据分析页面"
                    font.pixelSize: 24
                }
            }

            // 第四个标签页：系统设置
            Rectangle {
                color: "#ffffff"
                Layout.fillWidth: true
                Layout.fillHeight: true

                Text {
                    anchors.centerIn: parent
                    text: "系统设置页面"
                    font.pixelSize: 24
                }
            }
        }
    }

    // 系统配置对话框
    Dialog {
        id: sysConfigDialog
        title: "系统配置"
        width: 400
        height: 300
        anchors.centerIn: parent
        modal: true

        ColumnLayout {
            anchors.fill: parent
            spacing: 10

            GroupBox {
                title: "系统参数"
                Layout.fillWidth: true

                ColumnLayout {
                    anchors.fill: parent
                    TextField {
                        Layout.fillWidth: true
                        placeholderText: "参数1"
                    }
                    TextField {
                        Layout.fillWidth: true
                        placeholderText: "参数2"
                    }
                }
            }

            DialogButtonBox {
                Layout.fillWidth: true
                standardButtons: DialogButtonBox.Ok | DialogButtonBox.Cancel
                onAccepted: sysConfigDialog.accept()
                onRejected: sysConfigDialog.reject()
            }
        }
    }

    // 通信配置对话框
    Dialog {
        id: commConfigDialog
        title: "通信配置"
        width: 400
        height: 300
        anchors.centerIn: parent
        modal: true

        ColumnLayout {
            anchors.fill: parent
            spacing: 10

            GroupBox {
                title: "通信参数"
                Layout.fillWidth: true

                GridLayout {
                    columns: 2
                    anchors.fill: parent

                    Label { text: "IP地址:" }
                    TextField { Layout.fillWidth: true }
                    
                    Label { text: "端口:" }
                    TextField { Layout.fillWidth: true }
                }
            }

            DialogButtonBox {
                Layout.fillWidth: true
                standardButtons: DialogButtonBox.Ok | DialogButtonBox.Cancel
                onAccepted: commConfigDialog.accept()
                onRejected: commConfigDialog.reject()
            }
        }
    }

    // 设备配置对话框
    Dialog {
        id: deviceConfigDialog
        title: "程序配置"
        width: 400
        height: 300
        anchors.centerIn: parent
        modal: true

        ColumnLayout {
            anchors.fill: parent
            spacing: 10

            GroupBox {
                title: "设备参数"
                Layout.fillWidth: true

                ColumnLayout {
                    anchors.fill: parent
                    ComboBox {
                        Layout.fillWidth: true
                        model: ["设备1", "设备2", "设备3"]
                    }
                    TextField {
                        Layout.fillWidth: true
                        placeholderText: "设备参数"
                    }
                }
            }

            DialogButtonBox {
                Layout.fillWidth: true
                standardButtons: DialogButtonBox.Ok | DialogButtonBox.Cancel
                onAccepted: deviceConfigDialog.accept()
                onRejected: deviceConfigDialog.reject()
            }
        }
    }

    // 添加新的配置对话框
    Dialog {
        id: radarConfigDialog
        title: "限界范围配置"
        width: 400
        height: 300
        anchors.centerIn: parent
        modal: true

        ColumnLayout {
            anchors.fill: parent
            spacing: 10

            GroupBox {
                title: "雷达参数"
                Layout.fillWidth: true

                GridLayout {
                    columns: 2
                    anchors.fill: parent

                    Label { text: "雷达型号:" }
                    TextField { Layout.fillWidth: true }
                    
                    Label { text: "扫描频率:" }
                    TextField { Layout.fillWidth: true }
                }
            }

            DialogButtonBox {
                Layout.fillWidth: true
                standardButtons: DialogButtonBox.Ok | DialogButtonBox.Cancel
                onAccepted: radarConfigDialog.accept()
                onRejected: radarConfigDialog.reject()
            }
        }
    }

    Dialog {
        id: cameraConfigDialog
        title: "数据保存配置"
        width: 400
        height: 300
        anchors.centerIn: parent
        modal: true

        ColumnLayout {
            anchors.fill: parent
            spacing: 10

            GroupBox {
                title: "相机参数"
                Layout.fillWidth: true

                GridLayout {
                    columns: 2
                    anchors.fill: parent

                    Label { text: "相机ID:" }
                    TextField { Layout.fillWidth: true }
                    
                    Label { text: "分辨率:" }
                    ComboBox { 
                        Layout.fillWidth: true
                        model: ["1920x1080", "1280x720", "800x600"]
                    }
                }
            }

            DialogButtonBox {
                Layout.fillWidth: true
                standardButtons: DialogButtonBox.Ok | DialogButtonBox.Cancel
                onAccepted: cameraConfigDialog.accept()
                onRejected: cameraConfigDialog.reject()
            }
        }
    }

    // 在Window的最后添加以下JavaScript函数
    function createGridLines() {
        // 清除现有网格线
        gridLines.clear()
        
        // 创建横向网格线
        for (let y = -2000; y <= 7000; y += 1000) {
            gridLines.append(-6000, y)
            gridLines.append(6000, y)
            gridLines.append(NaN, NaN)  // 用于分隔不同的线段
        }
        
        // 创建纵向网格线
        for (let x = -6000; x <= 6000; x += 1000) {
            gridLines.append(x, -2000)
            gridLines.append(x, 7000)
            gridLines.append(NaN, NaN)  // 用于分隔不同的线段
        }
    }

    function createAreaLines() {
        // 清除现有区域线
        areaLines.clear()
        
        // 示例：创建矩形区域
        areaLines.append(-4000, -1000)
        areaLines.append(4000, -1000)
        areaLines.append(4000, 6000)
        areaLines.append(-4000, 6000)
        areaLines.append(-4000, -1000)
    }

    function createCompensationLines() {
        // 清除现有补偿基准线
        compensationLines.clear()
        
        // 示例：创建水平和垂直基准线
        // 水平基准线
        compensationLines.append(-5000, 0)
        compensationLines.append(5000, 0)
        compensationLines.append(NaN, NaN)
        
        // 垂直基准线
        compensationLines.append(0, -1500)
        compensationLines.append(0, 6500)
    }

    function calculateTickCount(min, max) {
        // 计算坐标轴范围
        const range = Math.abs(max - min)
        
        // 根据范围确定合适的网格间距
        let gridStep
        if (range <= 1000) {
            gridStep = 100
        } else if (range <= 5000) {
            gridStep = 500
        } else if (range <= 10000) {
            gridStep = 1000
        } else {
            gridStep = 2000
        }

        // 计算刻度数量（网格线数量 + 1）
        return Math.floor(range / gridStep) + 1
    }

    // 首先在文件顶部添加当前时间获取函数
    function getCurrentTime() {
        var date = new Date()
        return date.toLocaleTimeString(Qt.locale(), "hh:mm:ss")
    }

    // 添加在 Window 的末尾
    function zoomCharts(factor) {
        // 左侧图表
        var leftCenterX = (leftChart.axes[0].max + leftChart.axes[0].min) / 2
        var leftCenterY = (leftChart.axes[1].max + leftChart.axes[1].min) / 2
        
        leftChart.axes[0].min = leftCenterX - (leftCenterX - leftChart.axes[0].min) * factor
        leftChart.axes[0].max = leftCenterX + (leftChart.axes[0].max - leftCenterX) * factor
        leftChart.axes[1].min = leftCenterY - (leftCenterY - leftChart.axes[1].min) * factor
        leftChart.axes[1].max = leftCenterY + (leftChart.axes[1].max - leftCenterY) * factor

        // 右侧图表
        var rightCenterX = (rightChart.axes[0].max + rightChart.axes[0].min) / 2
        var rightCenterY = (rightChart.axes[1].max + rightChart.axes[1].min) / 2
        
        rightChart.axes[0].min = rightCenterX - (rightCenterX - rightChart.axes[0].min) * factor
        rightChart.axes[0].max = rightCenterX + (rightChart.axes[0].max - rightCenterX) * factor
        rightChart.axes[1].min = rightCenterY - (rightCenterY - rightChart.axes[1].min) * factor
        rightChart.axes[1].max = rightCenterY + (rightChart.axes[1].max - rightCenterY) * factor
    }

    function resetCharts() {
        // 重置左侧图表
        leftChart.axes[0].min = -6000
        leftChart.axes[0].max = 6000
        leftChart.axes[1].min = -2000
        leftChart.axes[1].max = 7000

        // 重置右侧图表
        rightChart.axes[0].min = -6000
        rightChart.axes[0].max = 6000
        rightChart.axes[1].min = -2000
        rightChart.axes[1].max = 7000
    }
}
