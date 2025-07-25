import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtCharts
import QtQuick.Window 

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

        // 主内容区域
        RowLayout {
            anchors.fill: parent
            anchors.margins: 10
            anchors.topMargin: titleBar.height + 10
            spacing: 10

            // 左侧图表区域
            Rectangle {
                Layout.fillHeight: true
                Layout.preferredWidth: parent.width * 0.7
                border.color: "#e0e0e0"
                radius: 4

                 Canvas {
                    id: tunnelOutlineCanvas
                    anchors.fill: parent
                    antialiasing: true
                    
                        // 添加缩放限制
                    property real minZoom: 0.9
                    property real maxZoom: 1.1

                    // 属性定义
                    property var dataBuffer: []
                    property var paintBuffer: []
                    property int frameRate: 5
                    property bool isProcessing: false
                    property real zoomFactor: 1.0
                    property point pan: Qt.point(0, 0)
                    
                    // 坐标系属性
                    property real xMin: -6000
                    property real xMax: 6000
                    property real yMin: -2000
                    property real yMax: 7000
                    
                    // 网格属性
                    property bool showGrid: true
                    property color gridColor: "#e0e0e0"
                    property int gridLineWidth: 1

                    // 添加区域线和补偿基准线显示属性
                    property bool showAreaLines: false
                    property bool showCompensationLines: false

                    // 坐标转换函数
                    function worldToCanvas(worldX, worldY) {
                        var canvasX = (worldX - xMin) / (xMax - xMin) * width * zoomFactor + pan.x
                        var canvasY = height - (worldY - yMin) / (yMax - yMin) * height * zoomFactor + pan.y
                        return Qt.point(canvasX, canvasY)
                    }

                    // 画布坐标转世界坐标
                    function canvasToWorld(canvasX, canvasY) {
                        var worldX = (canvasX - pan.x) / (width * zoomFactor) * (xMax - xMin) + xMin
                        var worldY = ((height - canvasY + pan.y) / (height * zoomFactor)) * (yMax - yMin) + yMin
                        return Qt.point(worldX, worldY)
                    }
                        // 更新缩放函数
                    function zoom(factor, center) {
                        var oldZoom = zoomFactor
                        var newZoom = oldZoom * factor
                        
                        // 限制缩放范围
                        newZoom = Math.max(minZoom, Math.min(maxZoom, newZoom))
                        
                        if (newZoom !== oldZoom) {
                            var zoomCenter = center || Qt.point(width/2, height/2)
                            var worldCenter = canvasToWorld(zoomCenter.x, zoomCenter.y)
                            
                            zoomFactor = newZoom
                            
                            // 更新平移以保持缩放中心
                            var newPos = worldToCanvas(worldCenter.x, worldCenter.y)
                            pan.x += zoomCenter.x - newPos.x
                            pan.y += zoomCenter.y - newPos.y
                            
                            requestPaint()
                        }
                    }
                    
                    // 重置视图函数
                    function resetView() {
                        zoomFactor = 1.0
                        pan = Qt.point(0, 0)
                        requestPaint()
                    }
    

                    onPaint: {
                        var ctx = getContext("2d")
                        ctx.clearRect(0, 0, width, height)
                        ctx.save()  // 保存当前状态

                        // 应用缩放和平移变换
                        ctx.translate(pan.x, pan.y)
                        ctx.scale(zoomFactor, zoomFactor)

                        // 1. 首先绘制网格
                        if (showGrid) {
                            drawGrid(ctx)
                        }

                        // 2. 绘制区域线
                        if (showAreaLines) {
                            drawAreaLines(ctx)
                        }

                        // 3. 绘制补偿基准线
                        if (showCompensationLines) {
                            drawCompensationLines(ctx)
                        }

                        // 4. 最后绘制点云数据
                        if (paintBuffer.length > 0) {
                            drawPoints(ctx)
                        }

                        ctx.restore()  // 恢复状态
                    }

                    // 添加区域线绘制函数
                    function drawAreaLines(ctx) {
                        ctx.strokeStyle = "#2196F3"
                        ctx.lineWidth = 2
                        
                        // 定义区域线坐标（使用世界坐标系）
                        var points = [
                            {x: -4000, y: -1000},
                            {x: 4000, y: -1000},
                            {x: 4000, y: 6000},
                            {x: -4000, y: 6000},
                            {x: -4000, y: -1000}
                        ]
                        
                        ctx.beginPath()
                        // 转换第一个点到画布坐标
                        var firstPoint = worldToCanvasNoTransform(points[0].x, points[0].y)
                        ctx.moveTo(firstPoint.x, firstPoint.y)
                        
                        // 转换并连接其他点
                        for (var i = 1; i < points.length; i++) {
                            var canvasPoint = worldToCanvasNoTransform(points[i].x, points[i].y)
                            ctx.lineTo(canvasPoint.x, canvasPoint.y)
                        }
                        
                        ctx.stroke()
                    }

                    // 添加补偿基准线绘制函数
                    function drawCompensationLines(ctx) {
                        ctx.strokeStyle = "#4CAF50"
                        ctx.lineWidth = 2
                        
                        // 绘制水平基准线
                        var start = worldToCanvasNoTransform(-5000, 0)
                        var end = worldToCanvasNoTransform(5000, 0)
                        ctx.beginPath()
                        ctx.moveTo(start.x, start.y)
                        ctx.lineTo(end.x, end.y)
                        ctx.stroke()
                        
                        // 绘制垂直基准线
                        start = worldToCanvasNoTransform(0, -1500)
                        end = worldToCanvasNoTransform(0, 6500)
                        ctx.beginPath()
                        ctx.moveTo(start.x, start.y)
                        ctx.lineTo(end.x, end.y)
                        ctx.stroke()
                    }
                    
                    // 添加新的坐标转换函数（不包含变换）
                    function worldToCanvasNoTransform(worldX, worldY) {
                        var canvasX = (worldX - xMin) / (xMax - xMin) * width
                        var canvasY = height - (worldY - yMin) / (yMax - yMin) * height
                        return Qt.point(canvasX, canvasY)
                    }

                    // 绘制网格
                    function drawGrid(ctx) {
                        ctx.strokeStyle = gridColor
                        ctx.lineWidth = gridLineWidth
                        
                        // 设置文本样式
                        ctx.font = "12px Arial"
                        ctx.fillStyle = "#333333"
                        ctx.textAlign = "center"
                        ctx.textBaseline = "middle"

                        // 计算网格间距
                        var xStep = 1000
                        var yStep = 1000

                        // 绘制垂直线和X轴刻度值
                        for (var x = xMin; x <= xMax; x += xStep) {
                            var start = worldToCanvas(x, yMin)
                            var end = worldToCanvas(x, yMax)
                            ctx.beginPath()
                            ctx.moveTo(start.x, start.y)
                            ctx.lineTo(end.x, end.y)
                            ctx.stroke()

                            // 在顶部绘制X轴刻度值
                            ctx.fillText(x.toString(), start.x, height - 15)
                        }

                        // 绘制水平线和Y轴刻度值
                        for (var y = yMin; y <= yMax; y += yStep) {
                            start = worldToCanvas(xMin, y)
                            end = worldToCanvas(xMax, y)
                            ctx.beginPath()
                            ctx.moveTo(start.x, start.y)
                            ctx.lineTo(end.x, end.y)
                            ctx.stroke()

                            // 在左侧绘制Y轴刻度值
                            ctx.fillText(y.toString(), 30, start.y)
                        }

                        // 绘制坐标轴标签
                        ctx.fillStyle = "#000000"
                        ctx.font = "14px Arial"
                        ctx.fillText("X轴 (mm)", width / 2, 30)  // X轴标签
                        ctx.save()
                        ctx.translate(15, height / 2)
                        ctx.rotate(-Math.PI / 2)
                        ctx.fillText("Y轴 (mm)", 0, 0)  // Y轴标签
                        ctx.restore()
                    }

                    // 绘制点云
                    function drawPoints(ctx) {
                        ctx.fillStyle = "#d21919"
                        const pointSize = 1

                        for (let i = 0; i < paintBuffer.length; i++) {
                            const point = paintBuffer[i]
                            if (!point || point.x === 0 || point.y === 0) continue

                            const canvasPoint = worldToCanvas(point.x, point.y)
                            ctx.beginPath()
                            ctx.arc(canvasPoint.x, canvasPoint.y, pointSize, 0, 2 * Math.PI)
                            ctx.fill()
                        }
                    }

                    // 鼠标交互
                    MouseArea {
                        anchors.fill: parent
                        property point lastPos: Qt.point(0, 0)
                        property bool isDragging: false

                        onWheel: {
                            var zoomFactor = wheel.angleDelta.y > 0 ? 1.1 : 0.9
                            var center = Qt.point(wheel.x, wheel.y)
                            parent.zoom(zoomFactor, center)
                        }

                        onPressed: {
                            lastPos = Qt.point(mouse.x, mouse.y)
                            isDragging = true
                            cursorShape = Qt.ClosedHandCursor
                        }

                        onReleased: {
                            isDragging = false
                            cursorShape = Qt.ArrowCursor
                        }

                        onPositionChanged: {
                            if (isDragging) {
                                var delta = Qt.point(mouse.x - lastPos.x, mouse.y - lastPos.y)
                                parent.pan.x += delta.x
                                parent.pan.y += delta.y
                                lastPos = Qt.point(mouse.x, mouse.y)
                                parent.requestPaint()
                            }
                        }
                    }

                    Connections {
                        target: qmlCommunication
                    
                        function onRadar_pageFromCpp(receivedData) {
                            console.log("Received radar data:")

                            if (!dataCanvas.dataBuffer) {
                                console.error("dataCanvas.dataBuffer is not defined")
                                return
                            }

                            for (const points of receivedData) {
                                console.log("Received points:", points.length)
                                for (const point of points) {
                                    if (point.hasOwnProperty("x") && point.hasOwnProperty("y")) {
                                        const pointX = point.x
                                        const pointY = point.y
                                        console.log("point x, point y:", pointX, pointY)

                                        // 将 point 添加到 dataBuffer
                                        dataCanvas.dataBuffer.push({x: pointX, y: pointY})
                                    } else {
                                        console.error("Invalid point structure:", point)
                                    }
                                }
                            }
                        }
                    }

                   Timer {
                        id: updateTimer
                        interval: 1000 / tunnelOutlineCanvas.frameRate
                        running: false
                        repeat: true

                        onTriggered: {
                            if (tunnelOutlineCanvas.dataBuffer.length === 0) {
                                return
                            }

                            //var startTime = new Date().getTime()
                            var frameData = tunnelOutlineCanvas.dataBuffer.shift()

                            // 限制缓存大小
                            while (tunnelOutlineCanvas.dataBuffer.length > 5) {
                                tunnelOutlineCanvas.dataBuffer.shift()
                            }

                            if (frameData && frameData.length > 0) {
                                try {
                                    var validPoints = frameData.filter(point => {
                                        return point && point.x !== 0 && point.y !== 0 &&
                                            typeof point.x === 'number' && typeof point.y === 'number' &&
                                            !isNaN(point.x) && !isNaN(point.y) &&
                                            isFinite(point.x) && isFinite(point.y)
                                    })

                                    //console.log("有效点数量:", validPoints.length)

                                    // 更新数据并触发重绘
                                    tunnelOutlineCanvas.paintBuffer = validPoints
                                    tunnelOutlineCanvas.requestPaint()

                                    //console.log("点云绘制完成")
                                } catch (e) {
                                    //console.error("绘制出错:", e)
                                }
                            }

                            //var endTime = new Date().getTime()
                            //console.log("绘制耗时:", endTime - startTime, "ms")
                        }
                    }
                 }
                
            }

            // 右侧控制面板
            Rectangle {
                Layout.fillHeight: true
                Layout.preferredWidth: parent.width * 0.3
                color: "#f5f5f5"
                radius: 4

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 15

                    GroupBox {
                        title: "检测配置" 
                        Layout.fillWidth: true

                        ColumnLayout {
                            anchors.fill: parent
                            spacing: 10

                            RowLayout {
                                Label {
                                    text: "线路名称:   "
                                    Layout.alignment: Qt.AlignLeft
                                }
                                ComboBox {
                                    Layout.fillWidth: true
                                    model: ["请选择"]
                                }
                            }

                            RowLayout {
                                Label { 
                                    text: "线路方向:   "
                                    Layout.alignment: Qt.AlignLeft
                                }
                                ComboBox {
                                    Layout.fillWidth: true
                                    model: ["请选择"]
                                }
                            }


                            RowLayout {
                                Label {
                                    text: "起始站区:   "
                                    Layout.alignment: Qt.AlignLeft
                                }
                                ComboBox {
                                    Layout.fillWidth: true
                                    model: ["请选择"]
                                }
                            }

                            RowLayout {
                                Label {
                                    text: "起始公里标:"
                                    Layout.alignment: Qt.AlignLeft
                                }
                                ComboBox {
                                    Layout.fillWidth: true
                                    model: ["请选择"]
                                }
                            }

                            RowLayout {
                                Layout.fillWidth: true
                                spacing: 10

                                Button {
                                    text: "开始"
                                    Layout.fillWidth: true
                                }

                                Button {
                                    text: "结束"
                                    Layout.fillWidth: true
                                }
                            }
                        }
                    }

                    GroupBox {
                        title: "显示选项"
                        Layout.fillWidth: true

                        ColumnLayout {
                            anchors.fill: parent
                            spacing: 10

                            // 视图选择行
                            RowLayout {
                                Layout.alignment: Qt.AlignHCenter  // 水平居中
                                spacing: 20  // 增加间距

                                RadioButton {
                                    text: "二维视图"
                                    checked: true
                                }
                                RadioButton {
                                    text: "三维视图"
                                }
                            }

                            // 复选框行
                            RowLayout {
                                Layout.alignment: Qt.AlignHCenter  // 水平居中
                                spacing: 20  // 增加间距

                                CheckBox {
                                    text: "网格线"
                                    checked: true
                                    onCheckedChanged: {
                                        // 直接控制 Canvas 的网格显示
                                        tunnelOutlineCanvas.showGrid = checked
                                        tunnelOutlineCanvas.requestPaint()
                                    }
                                }
                                
                                CheckBox {
                                    text: "区域线"
                                    checked: false
                                    onCheckedChanged: {
                                        tunnelOutlineCanvas.showAreaLines = checked
                                        tunnelOutlineCanvas.requestPaint()
                                    }
                                }
                                
                                CheckBox {
                                    text: "补偿基准线"
                                    checked: false
                                    onCheckedChanged: {
                                        tunnelOutlineCanvas.showCompensationLines = checked
                                        tunnelOutlineCanvas.requestPaint()
                                    }
                                }
                            }

                            // 按钮行
                            RowLayout {
                                Layout.alignment: Qt.AlignHCenter  // 水平居中
                                spacing: 20  // 增加间距

                                Button {
                                    text: "放大"
                                    implicitWidth: 80
                                    onClicked: {
                                        tunnelOutlineCanvas.zoom(1.2)
                                    }
                                }
                                Button {
                                    text: "缩小"
                                    implicitWidth: 80
                                    onClicked: {
                                        tunnelOutlineCanvas.zoom(0.8)
                                    }
                                }
                                Button {
                                    text: "复位"
                                    implicitWidth: 80
                                    onClicked: {
                                        tunnelOutlineCanvas.resetView()
                                    }
                                }
                            }
                        }
                    }

                    GroupBox {
                        title: "控制选项"
                        Layout.fillWidth: true

                        ColumnLayout {
                            anchors.fill: parent
                            spacing: 10

                            // 左侧开关区域
                            RowLayout {
                                Layout.fillWidth: true

                                // 左边开关列
                                ColumnLayout {
                                    Layout.alignment: Qt.AlignLeft
                                    spacing: 10

                                    RowLayout {
                                        spacing: 10
                                        Label { 
                                            text: "保存分析开关:"
                                            Layout.minimumWidth: 100
                                        }
                                        Switch {
                                            id: analysisSwitch
                                            onToggled: {
                                                var time = getCurrentTime()
                                                if (checked) {
                                                    messageModel.insert(0, {
                                                        message: `[${time}] 分析功能已开启`
                                                    })
                                                } else {
                                                    messageModel.insert(0, {
                                                        message: `[${time}] 分析功能已关闭`
                                                    })
                                                }
                                            }
                                        }
                                    }

                                    RowLayout {
                                        spacing: 10
                                        Label { 
                                            text: "设备告警开关:"
                                            Layout.minimumWidth: 100
                                        }
                                        Switch {
                                            id: alarmSwitch
                                            onToggled: {
                                                var time = getCurrentTime()
                                                if (checked) {
                                                    messageModel.insert(0, {
                                                        message: `[${time}] 告警功能已开启`
                                                    })
                                                } else {
                                                    messageModel.insert(0, {
                                                        message: `[${time}] 告警功能已关闭`
                                                    })
                                                }
                                            }
                                        }
                                    }
                                }

                                // 右边按钮列
                                ColumnLayout {
                                    Layout.alignment: Qt.AlignRight
                                    spacing: 10

                                    Button {
                                        text: "相机重启"
                                        implicitWidth: 100
                                        Layout.alignment: Qt.AlignRight 
                                    }

                                    Button {
                                        text: "截图/保存"
                                        implicitWidth: 100
                                        Layout.alignment: Qt.AlignRight
                                    }
                                }
                            }
                        }
                    }

                    // 状态显示区域
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        
                        ColumnLayout {
                            anchors.fill: parent
                            spacing: 10

                            // 系统状态显示区域
                            GroupBox {
                                title: "程序状态显示"
                                Layout.fillWidth: true
                                Layout.preferredHeight: parent.height * 0.5
                                
                                Rectangle {
                                    anchors.fill: parent
                                    color: "white"
                                    border.color: "#CCCCCC"

                                    ListModel {
                                        id: systemMessageModel
                                        ListElement {
                                            message: "[00:00:00] 系统启动..."
                                        }
                                    }

                                    ListView {
                                        id: systemMessageListView
                                        anchors.fill: parent
                                        model: systemMessageModel
                                        delegate: Text {
                                            padding: 5
                                            text: message
                                            color: "#666666"
                                        }
                                    }
                                }
                            }

                            // 错误信息显示区域
                            GroupBox {
                                title: "限界超限告警"
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                
                                Rectangle {
                                    anchors.fill: parent
                                    color: "white"
                                    border.color: "#CCCCCC"

                                    ListModel {
                                        id: errorMessageModel
                                        ListElement {
                                            message: "[00:00:00] 等待运行..."
                                        }
                                    }

                                    ListView {
                                        id: errorMessageListView
                                        anchors.fill: parent
                                        model: errorMessageModel
                                        delegate: Text {
                                            padding: 5
                                            text: message
                                            color: "#FF4444"  // 错误信息使用红色显示
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }  // 主窗口容器 Rectangle 结束

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



    // 首先在文件顶部添加当前时间获取函数
    function getCurrentTime() {
        var date = new Date()
        return date.toLocaleTimeString(Qt.locale(), "hh:mm:ss")
    }
}
