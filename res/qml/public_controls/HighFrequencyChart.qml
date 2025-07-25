import QtQuick

Item {
    id: root
    property var dataBuffer: []
    property var dataCache: []  // 新增缓存队列
    property int maxPoints: 1000
    property int dataCount: 0
    property int startIndex: 0
    property bool isRendering: false
    
    // 定义数据范围
    property real minValue: 0
    property real maxValue: 110
    property int targetFPS: 25
    property int lastRenderTime: 0
    
    // 坐标轴属性
    readonly property int yAxisWidth: 40  // Y轴宽度
    readonly property int tickLength: 5   // 刻度线长度
    readonly property int bottomMargin: 25 // 底部边距
    readonly property int topMargin: 25   // 顶部边距
    readonly property int rightMargin: 10  // 右侧边距
    readonly property real graphHeight: height - topMargin - bottomMargin

    property color lineColor: "black"  // 统一线段颜色
    property var typeColors: {
        1: "transparent",  // type=1时不显示
        2: "#4CAF50", // 绿色
        3: "#2196F3", // 蓝色
        4: "#F44336", // 红色
        5: "#9C27B0", // 紫色
        6: "#FFEB3B"  // 黄色
    }

    // 批量处理定时器
    Timer {
        id: batchProcessTimer
        interval: 1000 / root.targetFPS  // 根据目标FPS动态计算间隔
        running: false
        repeat: true
        onTriggered: processBatch()
    }

    function processBatch() {
        if (root.dataCache.length === 0) {
            root.isRendering = false;
            return;
        }

        // 计算本次可处理的最大点数 (1000点/20FPS = 50点/帧)
        var batchSize = Math.min(50, root.dataCache.length);
        
        // 移入主缓冲区
        for (var i = 0; i < batchSize; i++) {
            var value = root.dataCache.shift();
            if (root.dataBuffer.length < root.maxPoints) {
                root.dataBuffer.push(value);
            } else {
                root.dataBuffer[root.startIndex] = value;
                root.startIndex = (root.startIndex + 1) % root.maxPoints;
            }
        }
        
        root.dataCount = Math.min(root.dataCount + batchSize, root.maxPoints);
        canvas.requestPaint();
    }

    function appendData(value) {
        // 先存入缓存队列
        root.dataCache.push(value);
        
        // 启动处理定时器（如果未运行）
        if (!root.isRendering) {
            root.isRendering = true;
            batchProcessTimer.start();
        }
    }

    Canvas {
        id: canvas
        // 调整Canvas大小，为坐标轴和刻度值留出空间
        anchors {
            fill: parent
            leftMargin: 0        // 左边界不需要额外边距，由yAxisWidth处理
            rightMargin: root.rightMargin
            topMargin: root.topMargin 
            bottomMargin: root.bottomMargin
        }
        
        renderTarget: Canvas.FramebufferObject
        renderStrategy: Canvas.Threaded
        
        onPaint: {
            var now =  new Date().getTime()
            if (now - lastRenderTime < 1000/targetFPS) {
                return; // 跳过本次渲染
            }
            lastRenderTime = now;

            var ctx = getContext("2d")
            ctx.reset()
            
            // 计算绘图区域
            const graphX = root.yAxisWidth
            const graphWidth = width - graphX
            const graphHeight = height 
            
            // 计算Y轴缩放比例
            const yScale = graphHeight / (root.maxValue - root.minValue)
            
            // 绘制Y轴
            ctx.beginPath()
            ctx.moveTo(graphX, 0)
            ctx.lineTo(graphX, graphHeight)
            ctx.strokeStyle = "black"
            ctx.lineWidth = 1
            ctx.stroke()
            
            // 绘制底部X轴
            ctx.beginPath()
            ctx.moveTo(graphX, graphHeight)
            ctx.lineTo(width, graphHeight)
            ctx.strokeStyle = "black"
            ctx.lineWidth = 1
            ctx.stroke()
            
            // 绘制Y轴刻度和标签
            ctx.font = "10px Arial"
            ctx.textAlign = "right"
            ctx.textBaseline = "top"
            ctx.fillStyle = "black"
            
            // 以minValue~maxValue等分绘制
            const stepCount = 5
            for (let i = 0; i <= stepCount; i++) {
                const yValue = root.minValue + (root.maxValue - root.minValue) * (i / stepCount)
                const y = graphHeight - (yValue - root.minValue) * yScale   
                
                // 绘制刻度线
                ctx.beginPath()
                ctx.moveTo(graphX - tickLength, y)
                ctx.lineTo(graphX, y)
                ctx.stroke()
                
                // 绘制刻度值
                ctx.fillText(yValue.toFixed(1), graphX - tickLength - 2, y)
                
                // 绘制网格线(可选)
                if (i > 0) {
                    ctx.beginPath()
                    ctx.moveTo(graphX, y)
                    ctx.lineTo(width, y)
                    ctx.strokeStyle = "rgba(200, 200, 200, 0.5)"
                    ctx.stroke()
                    ctx.strokeStyle = "black"
                }
            }
            
            // 绘制数据点和曲线

            if (root.dataCount > 1) {
                const dx = (width - root.yAxisWidth) / (root.dataCount - 1);
                
                // 绘制连线
                ctx.beginPath();
                for (var i = 0; i < root.dataCount; i++) {
                    var idx = (root.startIndex + i) % root.maxPoints;
                    var x = root.yAxisWidth + i * dx;
                    var y = height - (root.dataBuffer[idx][0] - root.minValue) * (height / (root.maxValue - root.minValue));
                    
                    if (i === 0) ctx.moveTo(x, y);
                    else ctx.lineTo(x, y);
                }
                ctx.strokeStyle = root.lineColor;
                ctx.lineWidth = 1;
                ctx.stroke();
                
                // 绘制数据点（跳过type=1）
                for (i = 0; i < root.dataCount; i++) {
                    idx = (root.startIndex + i) % root.maxPoints;
                    var data = root.dataBuffer[idx];
                    var pointType = data[1] || 1;
                    
                    if (pointType === 1) continue;
                    
                    x = root.yAxisWidth + i * dx;
                    y = height - (data[0] - root.minValue) * (height / (root.maxValue - root.minValue));
                    
                    ctx.beginPath();
                    ctx.arc(x, y, 3, 0, Math.PI * 2);
                    ctx.fillStyle = root.typeColors[pointType] || "black";
                    ctx.fill();
                    
                    // 白色边框
                    ctx.lineWidth = 1;
                    ctx.strokeStyle = "white";
                    ctx.stroke();
                }
            }
        }
    }
    
}