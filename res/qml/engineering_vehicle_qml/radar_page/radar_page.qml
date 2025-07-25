import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtCharts
import QtQuick.Window 
import public_controls 1.0

Window {
    id: root
    visible: true
    width: 1920
    height: 1080
    title: qsTr("雷达限界系统")
    flags: Qt.Window | Qt.FramelessWindowHint | Qt.WindowMaximizeButtonHint
    color: "transparent"


    property string stationAreaText: ""      // 站区
    property string poleNumberText: ""       // 杆号
    property real kilometerPostText: 0.0     // 公里标
    property real currentSpeedText: 0.0   
    property string taskNameField: "无任务"  // 任务名



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
        CustomTitleBar {
            id: customTitleBar
            anchors.top: parent.top  
            width: parent.width
            height: 36
            titleName: qsTr("雷达限界系统")
            iconSource: "qrc:/infinity_station/res/icon/radar.ico"
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
            onCloseClicked: root.close()
        }
        
        // 主内容区域
        RowLayout {
            anchors.fill: parent
            anchors.margins: 10
            anchors.topMargin: customTitleBar.height + 10
            anchors.bottomMargin: 40
            spacing: 10

            // 左侧图表区域
            Rectangle {
                Layout.fillHeight: true
                Layout.preferredWidth: parent.width * 0.7
                border.color: "#e0e0e0"
                radius: 4

                // 修改后的Canvas实现
                Item {
                    id: canvasContainer
                    anchors.fill: parent
                    
                    // 数据绘制区Canvas
                    Canvas {
                        id: dataCanvas
                        anchors.fill: parent
                        z: 2 // 确保数据在最上层
                        antialiasing: true

                        renderTarget : Canvas.FramebufferObject
                        renderStrategy: Canvas.Immediate
                        
                        // 缩放限制(暂时不允许缩放)
                        property real minZoom: 1
                        property real maxZoom: 1
                        
                        // 属性定义
                        property var dataBuffer: []
                        property var paintBuffer: []
                        property var overrunBuffer:[] // 超限数据缓存
                        property int frameRate: 5 
                        property bool isProcessing: false
                        property real zoomFactor: 1.0
                        property point pan: Qt.point(0, 0)
                        
                        // 坐标系属性 - 这是数据的初始范围
                        property real xMin: -4000
                        property real xMax: 4000
                        property real yMin: -4000
                        property real yMax: 4000
                        
                        // 网格属性
                        property bool showGrid: true
                        
                        // 区域线和补偿基准线显示属性
                        property bool showAreaLines: false
                        property bool showCompensationLines: false
                        
                        // 简化的坐标转换函数 - 将世界坐标转换为canvas坐标
                        function worldToCanvas(worldX, worldY) {
                            var margin = 60
                            var viewportWidth = width - margin * 1.5
                            var viewportHeight = height - margin * 1.5
                            var xRange = xMax - xMin
                            var yRange = yMax - yMin
                            
                            // 考虑缩放和平移的转换
                            var canvasX = margin + ((worldX - xMin) / xRange) * viewportWidth * zoomFactor + pan.x
                            var canvasY = height - margin - ((worldY - yMin) / yRange) * viewportHeight * zoomFactor + pan.y
                            
                            return Qt.point(canvasX, canvasY)
                        }
                        
                        // 画布坐标转世界坐标
                        function canvasToWorld(canvasX, canvasY) {
                            var margin = 60
                            var viewportWidth = width - margin * 1.5
                            var viewportHeight = height - margin * 1.5
                            var xRange = xMax - xMin
                            var yRange = yMax - yMin
                            
                            // 计算基于缩放和平移的逆转换
                            var worldX = xMin + ((canvasX - margin - pan.x) / (viewportWidth * zoomFactor)) * xRange
                            var worldY = yMin + ((height - margin - canvasY - pan.y) / (viewportHeight * zoomFactor)) * yRange
                            
                            return Qt.point(worldX, worldY)
                        }
                        
                        // 获取当前可见范围（考虑缩放和平移）
                        function getVisibleRange() {
                            var margin = 60
                            var width = parent.width
                            var height = parent.height
                            
                            // 计算可见范围的四个角
                            var topLeft = canvasToWorld(margin, margin/2)
                            var bottomRight = canvasToWorld(width - margin/2, height - margin)
                            
                            return {
                                xMin: topLeft.x,
                                yMax: topLeft.y,
                                xMax: bottomRight.x,
                                yMin: bottomRight.y
                            }
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
                                
                                // 更新所有画布
                                requestPaint()
                                gridCanvas.requestPaint()
                                axisCanvas.requestPaint()
                            }
                        }
                        
                        // 重置视图函数
                        function resetView() {
                            zoomFactor = 1.0
                            pan = Qt.point(0, 0)
                            
                            // 更新所有画布
                            requestPaint()
                            gridCanvas.requestPaint()
                            axisCanvas.requestPaint()
                        }
                        
                        onPaint: {
                            var ctx = getContext("2d")
                            ctx.reset() // 清除画布
                            
                            // 只绘制数据点和区域线
                            if (paintBuffer.length > 0) {
                                drawPoints(ctx)
                            }
                            
                            // 绘制区域线
                            if (showAreaLines) {
                                drawAreaLines(ctx)
                            }
                            
                            // 绘制补偿基准线
                            if (showCompensationLines) {
                                drawCompensationLines(ctx)
                            }
                        }
                        
                        // 绘制点云
                        function drawPoints(ctx) {
                            ctx.fillStyle = "#d21919"
                            const pointSize = 1.5

                            //使用批量绘制
                            ctx.beginPath()
                            
                            for (let i = 0; i < paintBuffer.length; i++) {
                                const point = paintBuffer[i]
                                if (!point || point.x === 0 || point.y === 0) continue
                                
                                const canvasPoint = worldToCanvas(point.x, point.y)
                                ctx.moveTo(canvasPoint.x, canvasPoint.y)
                                ctx.arc(canvasPoint.x, canvasPoint.y, pointSize, 0, 2 * Math.PI)
                            }

                            ctx.fill()
                        }
                        
                        // 绘制区域线
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
                            var firstPoint = worldToCanvas(points[0].x, points[0].y)
                            ctx.moveTo(firstPoint.x, firstPoint.y)
                            
                            for (var i = 1; i < points.length; i++) {
                                var canvasPoint = worldToCanvas(points[i].x, points[i].y)
                                ctx.lineTo(canvasPoint.x, canvasPoint.y)
                            }
                            
                            ctx.stroke()
                        }
                        
                        // 绘制超限基准线drawCompensationLines
                        function drawCompensationLines(ctx) {
                            if (overrunBuffer.length === 0) return;
                            
                            ctx.strokeStyle = "#f3213d"
                            ctx.lineWidth = 2
                                
                            ctx.beginPath()
                            var firstPoint = worldToCanvas(overrunBuffer[0].x, overrunBuffer[0].y)
                            ctx.moveTo(firstPoint.x, firstPoint.y)

                            for (var i = 1; i < overrunBuffer.length; i++) {
                                var canvasPoint = worldToCanvas(overrunBuffer[i].x, overrunBuffer[i].y)
                                ctx.lineTo(canvasPoint.x, canvasPoint.y)
                            }
                            
                            ctx.stroke()
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
                                    
                                    // 更新所有画布
                                    parent.requestPaint()
                                    gridCanvas.requestPaint()
                                    axisCanvas.requestPaint()
                                }
                            }
                        }
                    }
                                        
                    // 网格Canvas
                    Canvas {
                        id: gridCanvas
                        anchors.fill: parent
                        z: 0 // 确保网格在中层

                        // 基于dataCanvas的属性
                        property bool showGrid: dataCanvas.showGrid

                        onPaint: {
                            var ctx = getContext("2d")
                            ctx.reset() // 清除画布
                            
                            if (showGrid) {
                                drawGrid(ctx)
                            }
                        }
                        
                        function drawGrid(ctx) {
                            var margin = 60 // 坐标轴的边距
                            var width = canvasContainer.width
                            var height = canvasContainer.height
                            
                            // 获取当前可见范围
                            var viewRange = dataCanvas.getVisibleRange()
                            var xMin = viewRange.xMin
                            var xMax = viewRange.xMax
                            var yMin = viewRange.yMin
                            var yMax = viewRange.yMax
                            
                            // X方向网格线
                            var xStep = calculateStep(xMax - xMin)
                            var xStart = Math.floor(xMin / xStep) * xStep
                            var xEnd = Math.ceil(xMax / xStep) * xStep
                            
                            ctx.strokeStyle = "#645c5c"
                            ctx.lineWidth = 1
                            
                            for (var x = xStart; x <= xEnd; x += xStep) {
                                // 使用dataCanvas的转换函数
                                var xPos = dataCanvas.worldToCanvas(x, 0).x
                                
                                if (xPos < margin || xPos > width - margin/2) continue
                                
                                ctx.beginPath()
                                ctx.moveTo(xPos, margin/2)
                                ctx.lineTo(xPos, height - margin)
                                ctx.stroke()
                            }
                            
                            // Y方向网格线
                            var yStep = calculateStep(yMax - yMin)
                            var yStart = Math.floor(yMin / yStep) * yStep
                            var yEnd = Math.ceil(yMax / yStep) * yStep
                            
                            for (var y = yStart; y <= yEnd; y += yStep) {
                                // 使用dataCanvas的转换函数
                                var yPos = dataCanvas.worldToCanvas(0, y).y
                                
                                if (yPos < margin/2 || yPos > height - margin) continue
                                
                                ctx.beginPath()
                                ctx.moveTo(margin, yPos)
                                ctx.lineTo(width - margin/2, yPos)
                                ctx.stroke()
                            }
                        }

                        function calculateStep(range) {
                            var roughStep = range / 10
                            var magnitude = Math.pow(10, Math.floor(Math.log10(roughStep)))
                            var normalized = roughStep / magnitude
                            
                            if (normalized < 2) return magnitude
                            if (normalized < 5) return 2 * magnitude
                            return 5 * magnitude
                        }
                    }
                                        

                    // 坐标轴Canvas
                    Canvas {
                        id: axisCanvas
                        anchors.fill: parent
                        z: 1 
                        
                        onPaint: {
                            var ctx = getContext("2d")
                            ctx.reset() // 清除画布
                            drawAxis(ctx)
                        }
                        
                        // 只绘制坐标轴、刻度和标签
                        function drawAxis(ctx) {
                            var margin = 60 // 坐标轴的边距
                            var width = canvasContainer.width
                            var height = canvasContainer.height
                            
                            // 获取当前可见范围
                            var viewRange = dataCanvas.getVisibleRange()
                            var xMin = viewRange.xMin
                            var xMax = viewRange.xMax
                            var yMin = viewRange.yMin
                            var yMax = viewRange.yMax
                            
                            // 绘制轴线
                            ctx.strokeStyle = "#000000"
                            ctx.lineWidth = 2
                            
                            // X轴 (y=0的水平线)
                            var xAxisYPos = dataCanvas.worldToCanvas(0, 0).y
                            if (xAxisYPos >= margin/2 && xAxisYPos <= height - margin) {
                                ctx.beginPath()
                                ctx.moveTo(margin, xAxisYPos)
                                ctx.lineTo(width - margin/2, xAxisYPos)
                                ctx.stroke()
                            }
                            
                            // Y轴 (x=0的垂直线)
                            var yAxisXPos = dataCanvas.worldToCanvas(0, 0).x
                            if (yAxisXPos >= margin && yAxisXPos <= width - margin/2) {
                                ctx.beginPath()
                                ctx.moveTo(yAxisXPos, margin/2)
                                ctx.lineTo(yAxisXPos, height - margin)
                                ctx.stroke()
                            }
                            
                            // 设置文本样式
                            ctx.font = "12px Arial"
                            ctx.fillStyle = "#333333"
                            ctx.textAlign = "center"
                            ctx.textBaseline = "middle"
                            
                            // 为X轴添加刻度线和标签
                            var xStep = calculateStep(xMax - xMin)
                            var xStart = Math.floor(xMin / xStep) * xStep
                            var xEnd = Math.ceil(xMax / xStep) * xStep
                            
                            for (var x = xStart; x <= xEnd; x += xStep) {
                                // 使用dataCanvas的转换函数
                                var xPos = dataCanvas.worldToCanvas(x, 0).x
                                
                                if (xPos < margin || xPos > width - margin/2) continue
                                
                                // 绘制刻度线
                                ctx.beginPath()
                                ctx.moveTo(xPos, xAxisYPos)
                                ctx.lineTo(xPos, xAxisYPos + 5)
                                ctx.stroke()
                                
                                // 绘制标签
                                ctx.fillText(x.toFixed(1), xPos, xAxisYPos + 15)
                            }
                            
                            // 为Y轴添加刻度线和标签
                            var yStep = calculateStep(yMax - yMin)
                            var yStart = Math.floor(yMin / yStep) * yStep
                            var yEnd = Math.ceil(yMax / yStep) * yStep
                            
                            for (var y = yStart; y <= yEnd; y += yStep) {
                                // 使用dataCanvas的转换函数
                                var yPos = dataCanvas.worldToCanvas(0, y).y
                                
                                if (yPos < margin/2 || yPos > height - margin) continue
                                
                                // 绘制刻度线
                                ctx.beginPath()
                                ctx.moveTo(yAxisXPos, yPos)
                                ctx.lineTo(yAxisXPos - 5, yPos)
                                ctx.stroke()
                                
                                // 绘制标签
                                ctx.textAlign = "right"
                                ctx.fillText(y.toFixed(1), yAxisXPos - 10, yPos)
                                ctx.textAlign = "center" // 重置对齐方式
                            }
                            
                            // 绘制坐标轴标签
                            ctx.fillStyle = "#000000"
                            ctx.font = "14px Arial"
                            ctx.textAlign = "center"
                            ctx.fillText("X轴 (mm)", width / 2, height - 20)
                            
                            ctx.save()
                            ctx.translate(15, height / 2)
                            ctx.rotate(-Math.PI / 2)
                            ctx.fillText("Y轴 (mm)", 0, 0)
                            ctx.restore()
                        }

                        // 根据范围计算合适的步长
                        function calculateStep(range) {
                            var roughStep = range / 10
                            var magnitude = Math.pow(10, Math.floor(Math.log10(roughStep)))
                            var normalized = roughStep / magnitude
                            
                            if (normalized < 2) return magnitude
                            if (normalized < 5) return 2 * magnitude
                            return 5 * magnitude
                        }
                    }
                    
                    // 初始绘制
                    Component.onCompleted: {
                        dataCanvas.requestPaint()
                        gridCanvas.requestPaint()
                        axisCanvas.requestPaint()
                    }
                }
                
                Connections {
                    target: qmlCommunication
                    
                    function onRadar_pageFromCpp(receivedData) {
                        // 如果缓冲区过大，直接丢弃旧数据而不是积累
                        if (dataCanvas.dataBuffer.length > 5) {
                            dataCanvas.dataBuffer.shift()
                        }

                        dataCanvas.dataBuffer.push(receivedData)

                        var time = getCurrentTime()
                        //console.log("解析时间", time)
                        if (!updateTimer.running) {
                            updateTimer.start()
                        }
                    }

                    function onRadar_device_stateFromCpp(receivedData) {
                        
                        
                        // 将 QVariant 转换为 QString
                        var deviceState = receivedData.toString()
                        var time = getCurrentTime()
                        
                        // 插入设备状态消息到系统消息模型
                        systemMessageModel.insert(0, {
                            message: `[${time}] 设备状态: ${deviceState}`
                        })
                    }

                    function onRadar_overrun_infoFromCpp(receivedData) {
                      
                        
                        var message = receivedData.toString()
                        var time = getCurrentTime()
                        
                        errorMessageModel.insert(0, {
                            message: `[${time}] ${message}`
                        })
                    }

                    function onRadar_overrun_configFromCpp(receivedData) {

                        console.log("收到超限配置数据", JSON.stringify(receivedData))
                        dataCanvas.overrunBuffer.push.apply(dataCanvas.overrunBuffer, receivedData)
                    }


                    function onegvlocation_pageFromCpp(data) {
                        // data 就是 QVariantMap，QML 端为 JS 对象
                        // console.log("收到定位数据", JSON.stringify(data))

                        stationAreaText = data.stationName || ""
                        poleNumberText = data.poleName || ""
                        currentSpeedText = data.speed !== undefined ? data.speed : 0
                        kilometerPostText = data.kiloMeter !== undefined ? data.kiloMeter : 0
                    }

                    function onTaskname_recvFromCpp(data) {
                        console.log("VIDEO收到任务名:", data)
                        taskNameField = data
                    }


                }

                Timer {
                    id: updateTimer
                    interval: 50
                    running: true
                    repeat: true
                    
                    onTriggered: {
                        if (dataCanvas.dataBuffer.length === 0) {
                            return
                        }

                        var frameData = dataCanvas.dataBuffer.shift()


                        if (frameData && frameData.length > 0) {
                            try {
                                var validPoints = frameData.filter(point => {
                                    return point && point.x !== 0 && point.y !== 0 &&
                                        typeof point.x === 'number' && typeof point.y === 'number' &&
                                        !isNaN(point.x) && !isNaN(point.y) &&
                                        isFinite(point.x) && isFinite(point.y)
                                })

                                // 获取当前时间
                                var drawTime = getCurrentTime()
                                console.log("绘制帧时间:", drawTime)

                                // 更新数据并触发重绘
                                dataCanvas.paintBuffer = validPoints
                                dataCanvas.requestPaint()
                            } catch (e) {
                                console.error("绘制出错:", e)
                            }
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
                        Layout.preferredHeight: parent.height * 0.3

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
                        Layout.preferredHeight: parent.height * 0.2

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
                                        // 控制网格显示
                                        gridCanvas.showGrid = checked
                                        gridCanvas.requestPaint()
                                    }
                                }
                                
                                CheckBox {
                                    text: "区域线"
                                    checked: false
                                    onCheckedChanged: {
                                        dataCanvas.showAreaLines = checked
                                        dataCanvas.requestPaint()
                                    }
                                }
                                
                                CheckBox {
                                    text: "超限基准线"
                                    checked: false
                                    onCheckedChanged: {
                                        dataCanvas.showCompensationLines = checked
                                        dataCanvas.requestPaint()
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
                                        dataCanvas.zoom(1.2)
                                    }
                                }
                                Button {
                                    text: "缩小"
                                    implicitWidth: 80
                                    onClicked: {
                                        dataCanvas.zoom(0.8)
                                    }
                                }
                                Button {
                                    text: "复位"
                                    implicitWidth: 80
                                    onClicked: {
                                        dataCanvas.resetView()
                                    }
                                }
                            }
                        }
                    }

                    GroupBox {
                        title: "控制选项"
                        Layout.fillWidth: true
                        Layout.preferredHeight: parent.height * 0.2

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
                        Layout.preferredHeight: parent.height * 0.3

                        ColumnLayout {
                            anchors.fill: parent
                            spacing: 10

                            // 系统状态显示区域
                            GroupBox {
                                title: "程序状态显示"
                                Layout.fillWidth: true
                                Layout.preferredHeight: parent.height * 0.3
                                
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
                                Layout.preferredHeight: parent.height * 0.7
                                
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

        Rectangle {
            id: statusBar
            width: parent.width
            height: 40
            color: "#f5f5f5"
            border.color: "#e0e0e0"
            radius: 4
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right

            RowLayout {
                anchors.fill: parent
                spacing: 20

                // 任务名
                Rectangle {
                    Layout.preferredWidth: 140
                    Layout.preferredHeight: 20
                    color: "transparent"

                    ColumnLayout {

                        Text {
                            text: `任务名: ${taskNameField}`
                            font.pixelSize: 12
                            color: "#666666"
                        }

                    }
                }


                // 站区
                Rectangle {
                    Layout.preferredWidth: 100
                    Layout.preferredHeight: 20
                    color: "transparent"

                    ColumnLayout {

                        Text {
                            text: `站区: ${stationAreaText}`
                            font.pixelSize: 12
                            color: "#666666"
                        }

                    }
                }

                // 杆号
                Rectangle {
                    Layout.preferredWidth: 100
                    Layout.preferredHeight: 20
                    color: "transparent"

                    ColumnLayout {

                        Text {
                            text: `杆号: ${poleNumberText}`
                            font.pixelSize: 12
                            color: "#666666"
                        }

                    }
                }

                // 公里标
                Rectangle {
                    Layout.preferredWidth: 100
                    Layout.preferredHeight: 20
                    color: "transparent"

                    ColumnLayout {

                        Text {
                            text: `公里标: ${kilometerPostText.toFixed(3)} km`  // 显示3位小数
                            font.pixelSize: 12
                            color: "#666666"
                        }
                    }
                }

                // 当前速度
                Rectangle {
                    Layout.preferredWidth: 100
                    Layout.preferredHeight: 20
                    color: "transparent"

                    ColumnLayout {

                        Text {
                            text: `速度: ${currentSpeedText.toFixed(2)} km/h`
                            font.pixelSize: 12
                            color: "#666666"
                        }

                    }
                }

                // 线路段类型
                Rectangle {
                    Layout.preferredWidth: 150
                    Layout.preferredHeight: 20
                    color: "transparent"

                    ColumnLayout {

                        Text {
                            text: "线路段类型："
                            font.pixelSize: 12
                            color: "#666666"
                        }

                        Text {
                            id: lineSectionTypeText
                            text: ""  // 初始值为空
                            font.pixelSize: 12
                            color: "#333333"
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
        var h = String(date.getHours()).padStart(2, "0")
        var m = String(date.getMinutes()).padStart(2, "0")
        var s = String(date.getSeconds()).padStart(2, "0")
        var ms = String(date.getMilliseconds()).padStart(3, "0")
        return h + ":" + m + ":" + s + "." + ms
    }
    // 数据Canvas中的视口范围计算函数
    function getCurrentViewport() {
        // 计算四个角的世界坐标
        var topLeft = canvasToWorld(0, 0)
        var topRight = canvasToWorld(width, 0)
        var bottomLeft = canvasToWorld(0, height)
        var bottomRight = canvasToWorld(width, height)
        
        return {
            xMin: Math.min(topLeft.x, bottomLeft.x),
            xMax: Math.max(topRight.x, bottomRight.x),
            yMin: Math.min(bottomLeft.y, bottomRight.y),
            yMax: Math.max(topLeft.y, topRight.y)
        }
    }



    // 添加消息模型，之前代码中引用但未定义
    ListModel {
        id: messageModel
    }
}
