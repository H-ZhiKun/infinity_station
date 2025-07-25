import QtQuick 
import QtQuick.Controls 
import QtQuick.Layouts 
import QtQuick.Window 
import QtCharts 

ApplicationWindow {
    id: mainWindow
    width: 1280
    height: 800
    visible: true
    title: "TOGCCrail 数据分析系统"
    
    // 应用程序标题栏
    header: Rectangle {
        id: titleBar
        width: parent.width
        height: 40
        color: "#0a1929"
        
        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            spacing: 20
            
            // 应用程序图标和标题
            Row {
                spacing: 5
                Image {
                    width: 120
                    height: 30
                    source: "qrc:/assets/logo.png" // 替换为实际logo资源
                    fillMode: Image.PreserveAspectFit
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
            
            // 菜单项
            Row {
                spacing: 20
                
                Button {
                    text: "文件(F)"
                    flat: true
                    contentItem: Text {
                        text: parent.text
                        color: "#ffffff"
                    }
                    background: Rectangle {
                        color: "transparent"
                    }
                }
                
                Button {
                    text: "配置(E)"
                    flat: true
                    contentItem: Text {
                        text: parent.text
                        color: "#ffffff"
                    }
                    background: Rectangle {
                        color: "transparent"
                    }
                }
                
                Button {
                    text: "帮助(H)"
                    flat: true
                    contentItem: Text {
                        text: parent.text
                        color: "#ffffff"
                    }
                    background: Rectangle {
                        color: "transparent"
                    }
                }
            }
            
            Item { Layout.fillWidth: true } // 占位以右对齐后面的内容
            
            // 语言选择下拉框
            ComboBox {
                id: languageCombo
                model: ["汉语（简体）"]
                implicitWidth: 150
                implicitHeight: 30
                
                contentItem: Text {
                    leftPadding: 10
                    text: languageCombo.displayText
                    color: "#ffffff"
                    verticalAlignment: Text.AlignVCenter
                }
                
                background: Rectangle {
                    color: "transparent"
                    border.color: "#555555"
                    radius: 2
                }
            }
            
            // 窗口控制按钮
            Row {
                spacing: 10
                
                Button {
                    width: 30
                    height: 30
                    text: "—"
                    flat: true
                    contentItem: Text {
                        text: parent.text
                        color: "#ffffff"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    background: Rectangle {
                        color: "transparent"
                    }
                    onClicked: mainWindow.showMinimized()
                }
                
                Button {
                    width: 30
                    height: 30
                    text: "□"
                    flat: true
                    contentItem: Text {
                        text: parent.text
                        color: "#ffffff"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    background: Rectangle {
                        color: "transparent"
                    }
                    onClicked: mainWindow.showMaximized()
                }
                
                Button {
                    width: 30
                    height: 30
                    text: "✕"
                    flat: true
                    contentItem: Text {
                        text: parent.text
                        color: "#ffffff"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    background: Rectangle {
                        color: "transparent"
                    }
                    onClicked: mainWindow.close()
                }
            }
        }
    }
    
    // 顶部导航栏
    Rectangle {
        id: tabBar
        width: parent.width
        height: 45
        anchors.top: parent.top
        color: "#ffffff"
        
        RowLayout {
            anchors.fill: parent
            spacing: 0
            
            // 导航按钮
            Repeater {
                model: ["图像", "数据", "曲线", "分布图", "超限处理"]
                delegate: Rectangle {
                    Layout.fillHeight: true
                    Layout.preferredWidth: 100
                    color: index === tabBarLayout.currentIndex ? "#ffffff" : "#f5f5f5"
                    border.width: index === tabBarLayout.currentIndex ? 1 : 0
                    border.color: index === tabBarLayout.currentIndex ? "#2196f3" : "transparent"
                    
                    Rectangle {
                        visible: index === tabBarLayout.currentIndex
                        width: parent.width
                        height: 3
                        color: "#2196f3"
                        anchors.bottom: parent.bottom
                    }
                    
                    Label {
                        anchors.centerIn: parent
                        text: modelData
                        font.pixelSize: 14
                    }
                    
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            tabBarLayout.currentIndex = index
                        }
                    }
                }
            }
            
            Item { Layout.fillWidth: true } // 占位用
        }
    }
    
    // 主内容区域 - 使用StackLayout实现多页面切换
    StackLayout {
        id: tabBarLayout
        anchors.top: tabBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        currentIndex: 0
        
        // 第1页: 图像
        Rectangle {
            color: "#f9f9f9"
            
            Label {
                anchors.centerIn: parent
                text: "图像页面内容"
                font.pixelSize: 20
            }
        }
        
        // 第2页: 数据表格
        Rectangle {
            color: "#f9f9f9"
            
            ColumnLayout {
                anchors.fill: parent
                spacing: 0
                
                // 查询区域
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 100
                    color: "#ffffff"
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 20
                        spacing: 20
                        
                        Label {
                            text: "起始站区:"
                            font.pixelSize: 14
                        }
                        
                        TextField {
                            Layout.preferredWidth: 250
                            placeholderText: "请输入"
                        }
                        
                        Label {
                            text: "结束站区:"
                            font.pixelSize: 14
                        }
                        
                        ComboBox {
                            Layout.preferredWidth: 250
                            model: ["请选择"]
                            
                            background: Rectangle {
                                border.color: "#d9d9d9"
                                border.width: 1
                                radius: 2
                            }
                        }
                        
                        Item { Layout.fillWidth: true }
                        
                        Button {
                            text: "查 询"
                            Layout.preferredWidth: 90
                            Layout.preferredHeight: 35
                            
                            contentItem: Text {
                                text: parent.text
                                color: "#ffffff"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            
                            background: Rectangle {
                                color: "#2196f3"
                                radius: 4
                            }
                        }
                        
                        Button {
                            text: "重 置"
                            Layout.preferredWidth: 90
                            Layout.preferredHeight: 35
                            
                            contentItem: Text {
                                text: parent.text
                                color: "#666666"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            
                            background: Rectangle {
                                color: "#f5f5f5"
                                border.color: "#d9d9d9"
                                border.width: 1
                                radius: 4
                            }
                        }
                    }
                }
                
                // 数据表格工具栏
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 50
                    color: "#ffffff"
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 20
                        anchors.rightMargin: 20
                        spacing: 15
                        
                        Button {
                            text: "仅显示数值类结果"
                            Layout.preferredHeight: 30
                            
                            contentItem: Text {
                                text: parent.text
                                color: "#666666"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            
                            background: Rectangle {
                                color: "#f5f5f5"
                                border.color: "#d9d9d9"
                                border.width: 1
                                radius: 4
                            }
                        }
                        
                        Button {
                            text: "仅显示图像识别类结果"
                            Layout.preferredHeight: 30
                            
                            contentItem: Text {
                                text: parent.text
                                color: "#666666"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            
                            background: Rectangle {
                                color: "#f5f5f5"
                                border.color: "#d9d9d9"
                                border.width: 1
                                radius: 4
                            }
                        }
                        
                        Item { Layout.fillWidth: true }
                        
                        Button {
                            text: "数据导出"
                            Layout.preferredHeight: 30
                            
                            contentItem: Text {
                                text: parent.text
                                color: "#666666"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            
                            background: Rectangle {
                                color: "#f5f5f5"
                                border.color: "#d9d9d9"
                                border.width: 1
                                radius: 4
                            }
                        }
                        
                        Button {
                            text: "自定义显示项"
                            Layout.preferredHeight: 30
                            
                            contentItem: Text {
                                text: parent.text
                                color: "#666666"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            
                            background: Rectangle {
                                color: "#f5f5f5"
                                border.color: "#d9d9d9"
                                border.width: 1
                                radius: 4
                            }
                            
                            Rectangle {
                                width: 16
                                height: 16
                                radius: 8
                                color: "#ff9800"
                                anchors.right: parent.right
                                anchors.top: parent.top
                                anchors.rightMargin: -5
                                anchors.topMargin: -5
                                
                                Text {
                                    anchors.centerIn: parent
                                    text: "1"
                                    color: "white"
                                    font.pixelSize: 10
                                }
                            }
                        }
                    }
                }
                
                // 表格内容
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "#ffffff"
                    
                    ListView {
                        id: tableView
                        anchors.fill: parent
                        anchors.margins: 1
                        clip: true
                        
                        // 表头
                        header: Rectangle {
                            width: tableView.width
                            height: 40
                            color: "#f5f5f5"
                            
                            RowLayout {
                                anchors.fill: parent
                                spacing: 0
                                
                                // 表头项
                                Repeater {
                                    model: [
                                        { text: "时间", width: 0.15 },
                                        { text: "站区", width: 0.15 },
                                        { text: "车速(km/h)", width: 0.15 },
                                        { text: "实时里程(km)", width: 0.15 },
                                        { text: "接触轨水平距离(mm)", width: 0.20 },
                                        { text: "接触轨垂直距离(mm)", width: 0.20 }
                                    ]
                                    
                                    delegate: Rectangle {
                                        Layout.fillHeight: true
                                        Layout.preferredWidth: tableView.width * modelData.width
                                        border.width: 1
                                        border.color: "#e8e8e8"
                                        
                                        Label {
                                            anchors.centerIn: parent
                                            text: modelData.text
                                            font.pixelSize: 14
                                        }
                                    }
                                }
                            }
                        }
                        
                        // 样本数据
                        model: 1
                        delegate: Rectangle {
                            width: tableView.width
                            height: 50
                            color: index % 2 == 0 ? "#ffffff" : "#f9f9f9"
                            
                            RowLayout {
                                anchors.fill: parent
                                spacing: 0
                                
                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.preferredWidth: tableView.width * 0.15
                                    border.width: 1
                                    border.color: "#e8e8e8"
                                    
                                    Label {
                                        anchors.centerIn: parent
                                        text: "时间戳值"
                                        font.pixelSize: 14
                                        color: "#666666"
                                    }
                                }
                                
                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.preferredWidth: tableView.width * 0.15
                                    border.width: 1
                                    border.color: "#e8e8e8"
                                    
                                    Label {
                                        anchors.centerIn: parent
                                        text: "某某站"
                                        font.pixelSize: 14
                                        color: "#666666"
                                    }
                                }
                                
                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.preferredWidth: tableView.width * 0.15
                                    border.width: 1
                                    border.color: "#e8e8e8"
                                    
                                    Label {
                                        anchors.centerIn: parent
                                        text: "数值"
                                        font.pixelSize: 14
                                        color: "#666666"
                                    }
                                }
                                
                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.preferredWidth: tableView.width * 0.15
                                    border.width: 1
                                    border.color: "#e8e8e8"
                                    
                                    Label {
                                        anchors.centerIn: parent
                                        text: "20.454"
                                        font.pixelSize: 14
                                        color: "#666666"
                                    }
                                }
                                
                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.preferredWidth: tableView.width * 0.20
                                    border.width: 1
                                    border.color: "#e8e8e8"
                                    
                                    Label {
                                        anchors.centerIn: parent
                                        text: ""
                                        font.pixelSize: 14
                                        color: "#666666"
                                    }
                                }
                                
                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.preferredWidth: tableView.width * 0.20
                                    border.width: 1
                                    border.color: "#e8e8e8"
                                    
                                    Label {
                                        anchors.centerIn: parent
                                        text: ""
                                        font.pixelSize: 14
                                        color: "#666666"
                                    }
                                }
                            }
                            
                            Rectangle {
                                width: parent.width
                                height: 50
                                color: "transparent"
                                
                                RowLayout {
                                    anchors.right: parent.right
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.rightMargin: 10
                                    spacing: 10
                                    
                                    Button {
                                        text: "查看数据曲线"
                                        flat: true
                                        contentItem: Text {
                                            text: parent.text
                                            color: "#2196f3"
                                        }
                                        background: Rectangle {
                                            color: "transparent"
                                        }
                                    }
                                    
                                    Button {
                                        text: "查看采集图像"
                                        flat: true
                                        contentItem: Text {
                                            text: parent.text
                                            color: "#2196f3"
                                        }
                                        background: Rectangle {
                                            color: "transparent"
                                        }
                                    }
                                }
                            }
                        }
                    }
                    
                    // 分页控件
                    Rectangle {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        height: 50
                        color: "#ffffff"
                        border.width: 1
                        border.color: "#e8e8e8"
                        
                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 20
                            anchors.rightMargin: 20
                            
                            Label {
                                text: "共 400 条记录 第 1 / 80 页"
                                font.pixelSize: 14
                                color: "#666666"
                            }
                            
                            Item { Layout.fillWidth: true }
                            
                            // 分页按钮
                            Row {
                                spacing: 5
                                
                                Repeater {
                                    model: ["<", "1", "2", "3", "4", "5", "6", "7", "8", "9", ">"]
                                    
                                    delegate: Rectangle {
                                        width: 40
                                        height: 30
                                        color: modelData === "1" ? "#2196f3" : "#ffffff"
                                        border.width: 1
                                        border.color: "#e8e8e8"
                                        radius: 2
                                        
                                        Label {
                                            anchors.centerIn: parent
                                            text: modelData
                                            color: modelData === "1" ? "#ffffff" : "#666666"
                                            font.pixelSize: 14
                                        }
                                        
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                // 页面切换逻辑
                                            }
                                        }
                                    }
                                }
                            }
                            
                            ComboBox {
                                model: ["10条/页"]
                                implicitWidth: 100
                                
                                contentItem: Text {
                                    leftPadding: 10
                                    text: parent.displayText
                                    color: "#666666"
                                    verticalAlignment: Text.AlignVCenter
                                    horizontalAlignment: Text.AlignHCenter
                                }
                                
                                background: Rectangle {
                                    border.color: "#e8e8e8"
                                    border.width: 1
                                    radius: 2
                                }
                            }
                            
                            Label { text: "跳至" }
                            
                            TextField {
                                Layout.preferredWidth: 50
                                text: "5"
                                horizontalAlignment: TextField.AlignHCenter
                                
                                background: Rectangle {
                                    border.color: "#e8e8e8"
                                    border.width: 1
                                    radius: 2
                                }
                            }
                            
                            Label { text: "页" }
                        }
                    }
                }
            }
        }
        
        // 第3页: 曲线图表
        Rectangle {
            color: "#f9f9f9"
            
            ColumnLayout {
                anchors.fill: parent
                spacing: 0
                
                // 查询区域
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 100
                    color: "#ffffff"
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 20
                        spacing: 20
                        
                        Label {
                            text: "起始站区:"
                            font.pixelSize: 14
                        }
                        
                        TextField {
                            Layout.preferredWidth: 250
                            placeholderText: "请输入"
                        }
                        
                        Label {
                            text: "结束站区:"
                            font.pixelSize: 14
                        }
                        
                        ComboBox {
                            Layout.preferredWidth: 250
                            model: ["请选择"]
                            
                            background: Rectangle {
                                border.color: "#d9d9d9"
                                border.width: 1
                                radius: 2
                            }
                        }
                        
                        Item { Layout.fillWidth: true }
                        
                        Button {
                            text: "查 询"
                            Layout.preferredWidth: 90
                            Layout.preferredHeight: 35
                            
                            contentItem: Text {
                                text: parent.text
                                color: "#ffffff"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            
                            background: Rectangle {
                                color: "#2196f3"
                                radius: 4
                            }
                        }
                        
                        Button {
                            text: "重 置"
                            Layout.preferredWidth: 90
                            Layout.preferredHeight: 35
                            
                            contentItem: Text {
                                text: parent.text
                                color: "#666666"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            
                            background: Rectangle {
                                color: "#f5f5f5"
                                border.color: "#d9d9d9"
                                border.width: 1
                                radius: 4
                            }
                        }
                    }
                }
                
                // 图表控制栏
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 50
                    color: "#f9f9f9"
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 20
                        anchors.rightMargin: 20
                        spacing: 15
                        
                        Button {
                            id: startButton
                            text: "开始"
                            icon.source: "qrc:/assets/play_icon.png" // 播放图标
                            
                            contentItem: RowLayout {
                                spacing: 5
                                Item { width: 20; height: 20; 
                                    Rectangle {
                                        width: 0
                                        height: 0
                                        anchors.centerIn: parent
                                        color: "transparent"
                                        
                                        // 创建三角形播放图标
                                        Canvas {
                                            anchors.fill: parent
                                            anchors.margins: 2
                                            onPaint: {
                                                var ctx = getContext("2d");
                                                ctx.fillStyle = "#333333";
                                                ctx.beginPath();
                                                ctx.moveTo(0, 0);
                                                ctx.lineTo(15, 8);
                                                ctx.lineTo(0, 16);
                                                ctx.closePath();
                                                ctx.fill();
                                            }
                                        }
                                    }
                                }
                                
                                Text {
                                    text: startButton.text
                                    color: "#333333"
                                }
                            }
                            
                            background: Rectangle {
                                color: "#e8e8e8"
                                radius: 4
                            }
                        }
                        
                        Label { text: "播放速度:" }
                        
                        ComboBox {
                            model: ["x1.0"]
                            implicitWidth: 80
                            
                            background: Rectangle {
                                border.color: "#d9d9d9"
                                border.width: 1
                                radius: 2
                            }
                        }
                        
                        Label { text: "每页长:" }
                        
                        ComboBox {
                            model: ["3000"]
                            implicitWidth: 80
                            
                            background: Rectangle {
                                border.color: "#d9d9d9"
                                border.width: 1
                                radius: 2
                            }
                        }
                        
                        Item { Layout.fillWidth: true }
                        
                        Button {
                            text: "自定义显示项"
                            
                            contentItem: Text {
                                text: parent.text
                                color: "#666666"
                            }
                            
                            background: Rectangle {
                                color: "#f5f5f5"
                                border.color: "#d9d9d9"
                                border.width: 1
                                radius: 4
                            }
                            
                            Rectangle {
                                width: 16
                                height: 16
                                radius: 8
                                color: "#ff9800"
                                anchors.right: parent.right
                                anchors.top: parent.top
                                anchors.rightMargin: -5
                                anchors.topMargin: -5
                                
                                Text {
                                    anchors.centerIn: parent
                                    text: "1"
                                    color: "white"
                                    font.pixelSize: 10
                                }
                            }
                        }
                    }
                }
                
                // 图表区域
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "#ffffff"
                    
                    ScrollView {
                        anchors.fill: parent
                        anchors.margins: 10
                        clip: true
                        
                        ColumnLayout {
                            width: parent.width
                            spacing: 20
                            
                            // 第一个图表
                            ChartView {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 200
                                antialiasing: true
                                title: ""
                                legend.visible: false
                                margins.top: 0
                                margins.bottom: 0
                                margins.left: 0
                                margins.right: 0
                                
                                ValueAxis {
                                    id: axisY1
                                    min: 0
                                    max: 75
                                    tickCount: 6
                                    labelFormat: "%d"
                                }
                                
                                ValueAxis {
                                    id: axisX1
                                    min: 1000
                                    max: 1700
                                    tickCount: 8
                                    labelFormat: "%d"
                                }
                                
                                LineSeries {
                                    axisX: axisX1
                                    axisY: axisY1
                                    name: "接触轨水平值"
                                    color: "#2196f3"
                                    width: 2
                                    
                                    // 添加一些样本数据点
                                    XYPoint { x: 1000; y: 15 }
                                    XYPoint { x: 1030; y: 20 }
                                    XYPoint { x: 1100; y: 10 }
                                    XYPoint { x: 1200; y: 30 }
                                    XYPoint { x: 1230; y: 50 }
                                    XYPoint { x: 1300; y: 45 }
                                    XYPoint { x: 1330; y: 45 }
                                    XYPoint { x: 1400; y: 70 }
                                    XYPoint { x: 1500; y: 40 }
                                    XYPoint { x: 1530; y: 20 }
                                    XYPoint { x: 1600; y: 10 }
                                    XYPoint { x: 1630; y: 60 }
                                    XYPoint { x: 1700; y: 45 }
                                }
                                
                                // 垂直参考线
                                Rectangle {
                                    x: parent.width * 0.2
                                    y: 0
                                    width: 1
                                    height: parent.height
                                    color: "#cccccc"
                                }
                                
                                Rectangle {
                                    x: parent.width * 0.2 - 80
                                    y: 80
                                    width: 160
                                    height: 80
                                    color: "#333333"
                                    opacity: 0.8
                                    radius: 5
                                    visible: true
                                    
                                    Column {
                                        anchors.fill: parent
                                        anchors.margins: 10
                                        spacing: 5
                                        
                                        Text {
                                            text: "站区"
                                            color: "white"
                                            font.pixelSize: 14
                                        }
                                        
                                        Text {
                                            text: "某某站: 2908w"
                                            color: "white"
                                            font.pixelSize: 14
                                        }
                                        
                                        Text {
                                            text: "某某站: 3104w"
                                            color: "white"
                                            font.pixelSize: 14
                                        }
                                    }
                                }
                            }
                            
                            // 第二个图表 - 复制第一个图表
                            ChartView {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 200
                                antialiasing: true
                                title: ""
                                legend.visible: false
                                margins.top: 0
                                margins.bottom: 0
                                margins.left: 0
                                margins.right: 0
                                
                                ValueAxis {
                                    id: axisY2
                                    min: 0
                                    max: 75
                                    tickCount: 6
                                    labelFormat: "%d"
                                }
                                
                                ValueAxis {
                                    id: axisX2
                                    min: 1000
                                    max: 1700
                                    tickCount: 8
                                    labelFormat: "%d"
                                }
                                
                                LineSeries {
                                    axisX: axisX2
                                    axisY: axisY2
                                    name: "接触轨水平值"
                                    color: "#2196f3"
                                    width: 2
                                    
                                    // 添加一些样本数据点
                                    XYPoint { x: 1000; y: 15 }
                                    XYPoint { x: 1030; y: 20 }
                                    XYPoint { x: 1100; y: 10 }
                                    XYPoint { x: 1200; y: 30 }
                                    XYPoint { x: 1230; y: 50 }
                                    XYPoint { x: 1300; y: 45 }
                                    XYPoint { x: 1330; y: 45 }
                                    XYPoint { x: 1400; y: 70 }
                                    XYPoint { x: 1500; y: 40 }
                                    XYPoint { x: 1530; y: 20 }
                                    XYPoint { x: 1600; y: 10 }
                                    XYPoint { x: 1630; y: 60 }
                                    XYPoint { x: 1700; y: 45 }
                                }
                            }
                            
                            // 第三个图表 - 复制第一个图表
                            ChartView {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 200
                                antialiasing: true
                                title: ""
                                legend.visible: false
                                margins.top: 0
                                margins.bottom: 0
                                margins.left: 0
                                margins.right: 0
                                
                                ValueAxis {
                                    id: axisY3
                                    min: 0
                                    max: 75
                                    tickCount: 6
                                    labelFormat: "%d"
                                }
                                
                                ValueAxis {
                                    id: axisX3
                                    min: 1000
                                    max: 1700
                                    tickCount: 8
                                    labelFormat: "%d"
                                }
                                
                                LineSeries {
                                    axisX: axisX3
                                    axisY: axisY3
                                    name: "接触轨水平值"
                                    color: "#2196f3"
                                    width: 2
                                    
                                    // 添加一些样本数据点
                                    XYPoint { x: 1000; y: 15 }
                                    XYPoint { x: 1030; y: 20 }
                                    XYPoint { x: 1100; y: 10 }
                                    XYPoint { x: 1200; y: 30 }
                                    XYPoint { x: 1230; y: 50 }
                                    XYPoint { x: 1300; y: 45 }
                                    XYPoint { x: 1330; y: 45 }
                                    XYPoint { x: 1400; y: 70 }
                                    XYPoint { x: 1500; y: 40 }
                                    XYPoint { x: 1530; y: 20 }
                                    XYPoint { x: 1600; y: 10 }
                                    XYPoint { x: 1630; y: 60 }
                                    XYPoint { x: 1700; y: 45 }
                                }
                            }
                            
                            // 底部导航
                            RowLayout {
                                Layout.fillWidth: true
                                Layout.preferredHeight: 50
                                
                                Text {
                                    text: "里程数"
                                    font.pixelSize: 14
                                }
                                
                                Slider {
                                    Layout.fillWidth: true
                                    from: 0
                                    to: 100
                                    value: 50
                                    
                                    background: Rectangle {
                                        x: parent.leftPadding
                                        y: parent.topPadding + parent.availableHeight / 2 - height / 2
                                        width: parent.availableWidth
                                        height: 4
                                        radius: 2
                                        color: "#d0d0d0"
                                        
                                        // 添加一些模拟的高度变化
                                        Row {
                                            anchors.fill: parent
                                            spacing: 0
                                            
                                            Repeater {
                                                model: 20
                                                
                                                Rectangle {
                                                    width: parent.width / 20
                                                    height: 4
                                                    color: "transparent"
                                                    
                                                    Rectangle {
                                                        width: parent.width * 0.8
                                                        height: Math.random() * 20
                                                        anchors.bottom: parent.bottom
                                                        anchors.horizontalCenter: parent.horizontalCenter
                                                        color: "#aaaaaa"
                                                        opacity: 0.7
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    
                                    handle: Rectangle {
                                        x: parent.leftPadding + parent.visualPosition * (parent.availableWidth - width)
                                        y: parent.topPadding + parent.availableHeight / 2 - height / 2
                                        width: 20
                                        height: 20
                                        radius: 10
                                        color: parent.pressed ? "#ffffff" : "#f6f6f6"
                                        border.color: "#bbbbbb"
                                    }
                                }
                                
                                Text {
                                    text: "里程数"
                                    font.pixelSize: 14
                                }
                            }
                        }
                    }
                }
            }
        }
        
        // 第4页: 分布图
        Rectangle {
            color: "#f9f9f9"
            
            Label {
                anchors.centerIn: parent
                text: "分布图页面内容"
                font.pixelSize: 20
            }
        }
        
        // 第5页: 超限处理
        Rectangle {
            color: "#f9f9f9"
            
            Label {
                anchors.centerIn: parent
                text: "超限处理页面内容"
                font.pixelSize: 20
            }
        }
    }
}