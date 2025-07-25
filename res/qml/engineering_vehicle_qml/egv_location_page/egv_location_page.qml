import QtQuick 
import QtQuick.Controls 
import QtQuick.Layouts 
import QtCharts 
import QtQuick.Dialogs
import InfinityStation 1.0
import public_controls

ApplicationWindow {
    id: mainWindow
    visible: true
    width: 1080
    height: 820
    title: "接触网参数检测系统 V2.0"
    color: "#ffffff"   // ← 增加这一行，设置为白色或你想要的不透明色


    // Dialog {
    //     id: closeTipDialog
    //     title: "提示"
    //     modal: true
    //     standardButtons: Dialog.Ok
    //     x: (mainWindow.width - width) / 2
    //     y: (mainWindow.height - height) / 2
    //     contentItem: Text {
    //         text: "当前页面不可关闭"
    //         wrapMode: Text.WordWrap
    //         color: "#333"
    //         font.pixelSize: 15
    //         horizontalAlignment: Text.AlignHCenter
    //         verticalAlignment: Text.AlignVCenter
    //     }
    // }

    property var zig : 8000.0 // 拉出值数据点
    property var hei : 8000.0 // 导高数据点
    property var moHao : 8000.0 // 摩耗数据点
    property var voltage : 8000.0 // 电压数据点
    property var electri : 8000.0 // 网流数据点
    property var press : 8000.0 // 压力数据点
    property var press1 : 8000.0
    property var press2 : 8000.0
    property var press3 : 8000.0
    property var press4 : 8000.0
    property var spotx : 8000.0 // 硬点数据点
    property var spoty : 8000.0 
    property int maxGeometryPoints: 20  // 最大显示点数
    property string stationName: ""     //站区
    property real speed: 0              //速度
    property real moveDistance: 0       //里程
    property real kilometer: 0.0        //公里标
    property real direction: 0.0        //行别
    property var speedPoints: []
    property int maxPoints: 20  // 最大显示点数
    property var zigPoints: []  // 存储拉出值历史数据
    property var heiPoints: []  // 存储导高历史数据
    property string currentStationName:"" // 当前站区名称
    property string currentPoleName:"" // 当前杆号名称
    property var taskWindow: null
    property int globalPositionType: 0
    property var zigMarkerPoints: [] 
    property var heiMarkerPoints: []
    property int lastZigPositionType: 0
    property int lastHeiPositionType: 0
    property var allStations: []    // 站区名累积
    property var allPoles: []       // 杆号累积（可选）
    property var allStationData:({})
    property string taskName: ''
    property bool canClose: true 
    property bool isUserScrolling:false 
    property var positionTypeQueue:[]
    //property int defaultpositionType: 1


    RowLayout {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: statusBar.top
        //anchors.margins: 5
        spacing: 5
        
        // 左侧区域 - 区域信息和综合信息
        Column {
            Layout.preferredWidth: parent.width * 0.25
            Layout.fillHeight: true
            spacing: 5
            
            // 站区信息面板
            Rectangle {
                width: parent.width
                height: parent.height * 0.35
                color: "#f5f5f5"
                border.color: "#dddddd"
                radius: 4
                
                Rectangle {
                    width: parent.width
                    height: 30
                    color: "#2980b9"
                    radius: 4
                    
                    Text {
                        anchors.centerIn: parent
                        text: "站区信息"
                        color: "white"
                        font.pixelSize: 14
                        font.bold: true
                    }
                }
                
                ListView {
                    id: locationListView
                    anchors.top: parent.top
                    anchors.topMargin: 35
                    anchors.left: parent.left
                    anchors.leftMargin: 5
                    anchors.right: parent.right
                    anchors.rightMargin: 5
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 5
                    clip: true
                    
                    // 添加滚动条
                    ScrollBar.vertical: ScrollBar {
                        active: true
                        policy: ScrollBar.AsNeeded
                        width: 8
                    }

                    // 使用动态 ListModel
                    model: ListModel {
                        id: stationListModel
                    }
                    
                    delegate: Rectangle {
                        width: locationListView.width - 10
                        height: 30
                        color: index % 2 == 0 ? "#ffffff" : "#eaf0f7"
                        radius: 2
                        
                        Rectangle {
                            width: 3
                            height: parent.height
                            color: station === currentStationName ? "#e74c3c" : (index === 0 ? "#3498db" : "transparent")
                            radius: 1
                        }
                        
                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: parent.left
                            anchors.leftMargin: 10
                            text: station  // 使用 station 属性
                            // 根据station是否匹配currentStationName来决定颜色
                            color: station === currentStationName ? "#e74c3c" : "#333333"
                            font.pixelSize: 13
                        }
                        
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                currentStationName = station
                                console.log("选择站区:", station)
                            }
                        }
                    }
                }
            }
            
            // 线路信息面板
            Rectangle {
                width: parent.width
                height: parent.height * 0.35
                color: "#f5f5f5"
                border.color: "#dddddd"
                radius: 4
                
                Rectangle {
                    width: parent.width
                    height: 30
                    color: "#2980b9"
                    radius: 4
                    
                    Text {
                        anchors.centerIn: parent
                        text: "线路信息"
                        color: "white"
                        font.pixelSize: 14
                        font.bold: true
                    }
                }
                
                // 数据表头
                Rectangle {
                    id: tableHeader
                    anchors.top: parent.top
                    anchors.topMargin: 35
                    width: parent.width
                    height: 25
                    color: "#e8e8e8"
                    
                    Row {
                        anchors.fill: parent
                        
                        Rectangle {
                            width: parent.width * 0.25
                            height: parent.height
                            color: "transparent"
                            
                            Text {
                                anchors.centerIn: parent
                                text: "杆号"
                                color: "#444444"
                                font.pixelSize: 12
                                font.bold: true
                            }
                        }                      
                        
                        Rectangle {
                            width: parent.width * 0.35
                            height: parent.height
                            color: "transparent"
                            
                            Text {
                                anchors.centerIn: parent
                                text: "公里标"
                                color: "#444444"
                                font.pixelSize: 12
                                font.bold: true
                            }
                        }
                        
                        Rectangle {
                            width: parent.width * 0.20
                            height: parent.height
                            color: "transparent"
                            
                            Text {
                                anchors.centerIn: parent
                                text: "跨距"
                                color: "#444444"
                                font.pixelSize: 12
                                font.bold: true
                            }
                        }
                    }
                }
                
                // 数据列表
                ListView {
                    id: dataListView
                    anchors.top: tableHeader.bottom
                    anchors.topMargin: 2
                    anchors.left: parent.left
                    anchors.leftMargin: 5
                    anchors.right: parent.right
                    anchors.rightMargin: 5
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 20
                    layer.enabled: true  // 启用OpenGL加速
                    clip: true

                    focus: true

                    // 添加滚动条
                    ScrollBar.vertical: ScrollBar {
                        active: true
                        policy: ScrollBar.AsNeeded
                        width: 8
                    }
                    
                    model: ListModel {
                        id: dataListModel
                    }
                    
                    delegate: Rectangle {
                        width: dataListView.width - 10
                        height: 24
                        color: index % 2 == 0 ? "#ffffff" : "#f5f5f5"
                        radius: 2
                        
                        Row {
                            anchors.fill: parent
                            
                            Rectangle {
                                width: parent.width * 0.25
                                height: parent.height
                                color: "transparent"
                                
                                Text {
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.left: parent.left
                                    anchors.leftMargin: 5
                                    text: pole
                                    font.pixelSize: 12
                                    color: pole === currentPoleName ? "#e74c3c" : "#333333"
                                    font.bold: pole === currentPoleName
                                }
                            }
                            
                            Rectangle {
                                width: parent.width * 0.35
                                height: parent.height
                                color: "transparent"
                                
                                Text {
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.right: parent.right
                                    anchors.rightMargin: 5
                                    text: Number(km).toFixed(4)
                                    font.pixelSize: 12
                                    color: pole === currentPoleName ? "#e74c3c" : "#333333"
                                }
                            }
                            
                            Rectangle {
                                width: parent.width * 0.20
                                height: parent.height
                                color: "transparent"
                                
                                Text {
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    text: span
                                    font.pixelSize: 12
                                    color: pole === currentPoleName ? "#e74c3c" : "#333333"
                                }
                            }
                            
                            Rectangle {
                                width: parent.width * 0.20
                                height: parent.height
                                color: "transparent"
                                
                                Text {
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    text: "定位点"
                                    font.pixelSize: 12
                                    color: pole === currentPoleName ? "#e74c3c" : "#333333"
                                }
                            }
                        }
                        
                        MouseArea {
                            anchors.fill: parent
                            onDoubleClicked: {
                                currentPoleName = pole
                                console.log("双击杆号:", pole)
                                let poledata = {
                                    "station_name": currentStationName,
                                    "pole_name": pole
                                }
                                qmlCommunication.behaviorFromQml(QmlCommunication.QmlActions.ULPoleCalibration,poledata)
                            }
                        }
                    }
                    // // 监听滚轮事件
                    // WheelHandler {
                    //     target: dataListView
                    //     acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
                    //     acceptedModifiers: Qt.NoModifier
                    //     onWheel: function(event) {
                    //         isUserScrolling = true;
                    //         scrollTimer.restart(); // 直接重启Timer
                    //     }
                    // }

                }
                
            }

            //校准区域
            Rectangle {
                width: parent.width
                height: parent.height * 0.3  // 调整高度比例
                color: "#f5f5f5"
                border.color: "#dddddd"
                radius: 4
            
                
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 10

                   // 站区选择
                    ComboBox {
                        id: stationSelect
                        Layout.fillWidth: true
                        Layout.preferredHeight: 30
                        // 使用stationListModel作为数据源
                        model: {
                            let stations = ["请选择站区"]
                            for(let i = 0; i < stationListModel.count; i++) {
                                stations.push(stationListModel.get(i).station)
                            }
                            return stations
                        }
                        onCurrentTextChanged: {
                            if (currentIndex > 0) {
                                // 先刷新线路信息数据
                                refreshDataListModel(stationSelect.currentText)

                                // 清空当前杆号选择
                                poleSelect.model = ["请选择杆号"]
                                currentPoleName = ""

                                // 更新杆号选择框的数据
                                let poles = ["请选择杆号"]
                                for(let i = 0; i < dataListModel.count; i++) {
                                    let item = dataListModel.get(i)
                                    poles.push(item.pole)
                                }
                                poleSelect.model = poles

                                // 自动选择第一个杆号（可选）
                                if(poles.length > 1) {
                                    poleSelect.currentIndex = 1
                                    currentPoleName = poleSelect.currentText
                                }
                                else {
                                    // 重置杆号选择
                                    poleSelect.model = ["请选择杆号"]
                                    currentPoleName = ""
                                }
                            }
                        }
                    }

                    // 杆号选择
                    ComboBox {
                        id: poleSelect
                        Layout.fillWidth: true
                        Layout.preferredHeight: 30
                        model: ["请选择杆号"]
                        enabled: stationSelect.currentIndex > 0
                        
                        // 添加当前值显示样式
                        delegate: ItemDelegate {
                            width: poleSelect.width
                            height: 30
                            contentItem: Text {
                                text: modelData
                                color: (modelData === currentPoleName && 
                                    stationSelect.currentText === currentStationName) ? 
                                    "#e74c3c" : "#333333"
                                font.bold: (modelData === currentPoleName && 
                                        stationSelect.currentText === currentStationName)
                                elide: Text.ElideRight
                                verticalAlignment: Text.AlignVCenter
                            }
                            highlighted: poleSelect.highlightedIndex === index
                        }
                        
                        onCurrentTextChanged: {
                            if(currentIndex > 0) {
                                currentPoleName = currentText
                                // 同时更新当前站区（确保一致性）
                                currentStationName = stationSelect.currentText
                            }
                        }
                    }

                    // 定位校准按钮
                    Rectangle {
                        id: calibrateBtn
                        Layout.fillWidth: true
                        Layout.preferredHeight: 36
                        color: mouseArea.pressed ? "#2980b9" : "#3498db"  // 按下时变色
                        radius: 4
                        
                        // 使用 transform 来实现缩放，这样不会影响父容器
                        transform: Scale {
                            id: buttonScale
                            origin.x: calibrateBtn.width / 2
                            origin.y: calibrateBtn.height / 2
                            xScale: mouseArea.pressed ? 0.95 : 1.0
                            yScale: mouseArea.pressed ? 0.95 : 1.0
                            
                            Behavior on xScale {
                                NumberAnimation { duration: 100 }
                            }
                            Behavior on yScale {
                                NumberAnimation { duration: 100 }
                            }
                        }

                        // 颜色过渡动画
                        Behavior on color {
                            ColorAnimation { duration: 100 }
                        }

                        Text {
                            anchors.centerIn: parent
                            text: "定位校准"
                            color: "white"
                            font.pixelSize: 14
                            font.bold: true
                        }

                        MouseArea {
                            id: mouseArea
                            anchors.fill: parent
                            onClicked: {
                                console.log("执行定位校准")
                                let calibrationdata = {
                                    "station_name": stationSelect.currentText,
                                    "pole_name": poleSelect.currentText
                                }
                                qmlCommunication.behaviorFromQml(QmlCommunication.QmlActions.PoleCalibration,calibrationdata)
                                console.log("当前站区:", stationSelect.currentText, "当前杆号:", poleSelect.currentText)
                            }
                        }
                    }

                    // 上一杆下一杆按钮
                    RowLayout {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 36
                        spacing: 10

                        // 上一杆按钮
                        Rectangle {
                            id: prevPoleBtn
                            Layout.fillWidth: true
                            Layout.preferredHeight: 36
                            color: prevPoleArea.pressed ? "#2980b9" : "#3498db"
                            radius: 4

                            // 使用 transform 来实现缩放，这样不会影响父容器
                            transform: Scale {
                                id: prevPoleScale
                                origin.x: prevPoleBtn.width / 2
                                origin.y: prevPoleBtn.height / 2
                                xScale: prevPoleArea.pressed ? 0.95 : 1.0
                                yScale: prevPoleArea.pressed ? 0.95 : 1.0
                                
                                Behavior on xScale {
                                    NumberAnimation { duration: 100 }
                                }
                                Behavior on yScale {
                                    NumberAnimation { duration: 100 }
                                }
                            }

                            // 颜色过渡动画
                            Behavior on color {
                                ColorAnimation { duration: 100 }
                            }

                            Text {
                                anchors.centerIn: parent
                                text: "上一杆"
                                color: "white"
                                font.pixelSize: 13
                            }

                            MouseArea {
                                id: prevPoleArea
                                anchors.fill: parent
                                onClicked: {
                                    // 查找当前杆号在 dataListModel 中的索引
                                    let currentIndex = -1
                                    for (let i = 0; i < dataListModel.count; i++) {
                                        if (dataListModel.get(i).pole === currentPoleName) {
                                            currentIndex = i
                                            break
                                        }
                                    }
                                    
                                    // 如果找到当前杆号且不是第一个
                                    if (currentIndex > 0) {
                                        let prevPole = dataListModel.get(currentIndex - 1).pole
                                        console.log("切换到上一杆:", prevPole, "站区:", currentStationName)
                                        
                                        let poledata = {
                                            "station_name": currentStationName,
                                            "pole_name": prevPole
                                        }
                                        qmlCommunication.behaviorFromQml(QmlCommunication.QmlActions.ULPoleCalibration,poledata)

                                    } else {
                                        console.log("已经是第一个杆号")
                                    }
                                }
                            }

                        }

                        // 下一杆按钮
                        Rectangle {
                            id: nextPoleBtn
                            Layout.fillWidth: true
                            Layout.preferredHeight: 36
                            color: nextPoleArea.pressed ? "#2980b9" : "#3498db"
                            radius: 4

                            // 使用 transform 来实现缩放，这样不会影响父容器
                            transform: Scale {
                                id: nextPoleScale
                                origin.x: nextPoleBtn.width / 2
                                origin.y: nextPoleBtn.height / 2
                                xScale: nextPoleArea.pressed ? 0.95 : 1.0
                                yScale: nextPoleArea.pressed ? 0.95 : 1.0
                                
                                Behavior on xScale {
                                    NumberAnimation { duration: 100 }
                                }
                                Behavior on yScale {
                                    NumberAnimation { duration: 100 }
                                }
                            }

                            // 颜色过渡动画
                            Behavior on color {
                                ColorAnimation { duration: 100 }
                            }

                            Text {
                                anchors.centerIn: parent
                                text: "下一杆"
                                color: "white"
                                font.pixelSize: 13
                            }

                            MouseArea {
                                id: nextPoleArea
                                anchors.fill: parent
                                onClicked: {
                                    // 查找当前杆号在 dataListModel 中的索引
                                    let currentIndex = -1
                                    for (let i = 0; i < dataListModel.count; i++) {
                                        if (dataListModel.get(i).pole === currentPoleName) {
                                            currentIndex = i
                                            break
                                        }
                                    }
                                    
                                    // 如果找到当前杆号且不是最后一个
                                    if (currentIndex !== -1 && currentIndex < dataListModel.count - 1) {
                                        let nextPole = dataListModel.get(currentIndex + 1).pole
                                        console.log("切换到下一杆:", nextPole, "站区:", currentStationName)
                                        let poledata = {
                                            "station_name": currentStationName,
                                            "pole_name": nextPole
                                        }
                                        qmlCommunication.behaviorFromQml(QmlCommunication.QmlActions.ULPoleCalibration,poledata)

                                    } else {
                                        console.log("已经是最后一个杆号")
                                    }
                                }
                            }
                        }
                    }

                    Item { Layout.fillHeight: true } // 填充剩余空间
                }
            }

        }
        
        // 中间区域 - 图表显示区域
        Rectangle {
            Layout.preferredWidth: parent.width * 0.5
            Layout.fillHeight: true
            color: "#f5f5f5"
            radius: 4
            
            // 添加 ScrollView
            ScrollView {
                id: chartScrollView
                anchors.fill: parent
                anchors.margins: 5
                ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
                ScrollBar.vertical.policy: ScrollBar.AsNeeded
                clip: true

                Column {
                    width: chartScrollView.width - 20  // 减去滚动条宽度
                    spacing: 10
            
                    // 拉出值图表
                    Rectangle {
                        width: parent.width
                        height: 180
                        color: "white"
                        border.color: "#dddddd"
                        radius: 4
                        
                        Text {
                            anchors.right: parent.right
                            anchors.top: parent.top
                            anchors.margins: 8
                            text: "拉出值(MM)"
                            font.pixelSize: 12
                            color: "#3498db"
                            font.bold: true
                        }
                    

                        HighFrequencyChart {
                            id: zigchart
                            // 添加适当的边距
                            anchors {
                                fill: parent
                                leftMargin: 10    // 左侧额外边距
                                rightMargin: 10   // 右侧边距
                                topMargin: 10     // 顶部边距
                                bottomMargin: 10  // 底部边距
                            }
                            minValue: -300
                            maxValue: 300
                            maxPoints: 50
                        }


                    }

                    // 导高图表
                    Rectangle {
                        width: parent.width
                        height: 180
                        color: "white"
                        border.color: "#dddddd"
                        radius: 4
                        
                        Text {
                            anchors.right: parent.right
                            anchors.top: parent.top
                            anchors.margins: 8
                            text: "导高(MM)"
                            font.pixelSize: 12
                            color: "#3498db"
                            font.bold: true
                        }
                        

                        HighFrequencyChart {
                            id: heichart
                            // 添加适当的边距
                            anchors {
                                fill: parent
                                leftMargin: 10    // 左侧额外边距
                                rightMargin: 10   // 右侧边距
                                topMargin: 10     // 顶部边距
                                bottomMargin: 10  // 底部边距
                            }
                            minValue: 0
                            maxValue: 5000
                            maxPoints: 50
                        }
                    }
                    
                    // 磨耗图表
                    Rectangle {
                        width: parent.width
                        height: 150
                        color: "white"
                        border.color: "#dddddd"
                        radius: 4
                        
                        Text {
                            anchors.right: parent.right
                            anchors.top: parent.top
                            anchors.margins: 8
                            text: "磨耗(MM)"
                            font.pixelSize: 12
                            color: "#3498db"
                            font.bold: true
                        }

                        HighFrequencyChart {
                            id: moHaochart
                            // 添加适当的边距
                            anchors {
                                fill: parent
                                leftMargin: 10    // 左侧额外边距
                                rightMargin: 10   // 右侧边距
                                topMargin: 10     // 顶部边距
                                bottomMargin: 10  // 底部边距
                            }
                            minValue: 0
                            maxValue: 50
                            maxPoints: 50
                        }
                        

                    }
                    
                    // 车速值图表
                    Rectangle {
                        width: parent.width
                        height: 150
                        color: "white"
                        border.color: "#dddddd"
                        radius: 4
                        
                        Text {
                            anchors.right: parent.right
                            anchors.top: parent.top
                            anchors.margins: 8
                            text: "车速值(KM/H)"
                            font.pixelSize: 12
                            color: "#3498db"
                            font.bold: true
                        }
                        

                        HighFrequencyChart {
                            id: speedchart
                            // 添加适当的边距
                            anchors {
                                fill: parent
                                leftMargin: 10    // 左侧额外边距
                                rightMargin: 10   // 右侧边距
                                topMargin: 10     // 顶部边距
                                bottomMargin: 10  // 底部边距
                            }
                            minValue: 0
                            maxValue: 110
                            maxPoints: 100
                        }

                    }

                    
                }
            }
        }
        
        // 右侧区域 - 定位数据和监测数据
        Column {
            Layout.preferredWidth: parent.width * 0.24
            Layout.fillHeight: true
            spacing: 5
            
            // 实时定位信息面板
            Rectangle {
                width: parent.width
                height: parent.height * 0.35
                color: "#f5f5f5"
                border.color: "#dddddd"
                radius: 4
                
                Rectangle {
                    width: parent.width
                    height: 30
                    color: "#2980b9"
                    radius: 4
                    
                    Text {
                        anchors.centerIn: parent
                        text: "实时定位信息"
                        color: "white"
                        font.pixelSize: 14
                        font.bold: true
                    }
                }
                
                // 定位数据信息
                Column {
                    anchors.top: parent.top
                    anchors.topMargin: 40
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    anchors.right: parent.right
                    anchors.rightMargin: 10
                    spacing: 12
                    
                    Row {
                        width: parent.width
                        spacing: 5
                        
                        Text {
                            width: 80
                            text: "行别:"
                            horizontalAlignment: Text.AlignRight
                            font.pixelSize: 13
                            color: "#555555"
                        }
                        
                        Text {
                            text: ""
                            font.pixelSize: 13
                            color: "#333333"
                        }
                    }
                    
                    Row {
                        width: parent.width
                        spacing: 5
                        
                        Text {
                            width: 80
                            text: "当前站区:"
                            horizontalAlignment: Text.AlignRight
                            font.pixelSize: 13
                            color: "#555555"
                        }
                        
                        Text {
                            text: currentStationName || "未选择"
                            font.pixelSize: 13
                            color: "#333333"
                            font.bold: true
                        }
                    }
                    
                    Row {
                        width: parent.width
                        spacing: 5
                        
                        Text {
                            width: 80
                            text: "当前杆号:"
                            horizontalAlignment: Text.AlignRight
                            font.pixelSize: 13
                            color: "#555555"
                        }
                        
                        Text {
                            text: currentPoleName || "未选择"
                            font.pixelSize: 13
                            color: "#333333"
                            font.bold: true
                        }
                    }
                    
                    Row {
                        width: parent.width
                        spacing: 5
                    }
                    
                    Row {
                        width: parent.width
                        spacing: 5
                        
                        Text {
                            width: 80
                            text: "实时里程:"
                            horizontalAlignment: Text.AlignRight
                            font.pixelSize: 13
                            color: "#555555"
                        }
                        
                        Text {
                            text: Number(moveDistance).toFixed(2) + " km"
                            font.pixelSize: 13
                            color: "#333333"
                        }
                    }
                    
                    Row {
                        width: parent.width
                        spacing: 5
                        
                        Text {
                            width: 80
                            text: "公里标:"
                            horizontalAlignment: Text.AlignRight
                            font.pixelSize: 13
                            color: "#555555"
                        }
                        
                        Text {
                            text: Number(kilometer).toFixed(1) + " km"
                            font.pixelSize: 13
                            color: "#333333"
                        }
                    }
                    
                    Row {
                        width: parent.width
                        spacing: 5
                        
                        Text {
                            width: 80
                            text: "实时车速:"
                            horizontalAlignment: Text.AlignRight
                            font.pixelSize: 13
                            color: "#555555"
                        }
                        
                        Text {
                            text: Number(speed).toFixed(1) + " km/h"
                            font.pixelSize: 13
                            color: "#333333"
                            font.bold: true
                        }
                    }

                    Row {
                        width: parent.width
                        spacing: 5
                        
                        Text {
                            width: 80
                            text: "RFID:"
                            horizontalAlignment: Text.AlignRight
                            font.pixelSize: 13
                            color: "#555555"
                        }
                        
                        
                        Text {
                            id: rfidValue
                            text: "无数据"
                            font.pixelSize: 13
                            color: "#333333"
                        }
                        
                    }
                }
            }
            
            // 实时检测信息面板
            Rectangle {
                width: parent.width
                height: parent.height * 0.4
                color: "#f5f5f5"
                border.color: "#dddddd"
                radius: 4
                
                Rectangle {
                    width: parent.width
                    height: 30
                    color: "#2980b9"
                    radius: 4
                    
                    Text {
                        anchors.centerIn: parent
                        text: "实时检测信息"
                        color: "white"
                        font.pixelSize: 14
                        font.bold: true
                    }
                }
                
                // 监测数据信息
                Column {
                    anchors.top: parent.top
                    anchors.topMargin: 40
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    anchors.right: parent.right
                    anchors.rightMargin: 10
                    spacing: 12
                    
                    Row {
                        width: parent.width
                        spacing: 5
                        
                        Text {
                            width: 80
                            text: "导高:"
                            horizontalAlignment: Text.AlignRight
                            font.pixelSize: 13
                            color: "#555555"
                        }
                        
                        Text {
                            id: heiValueText
                            text: Number(hei).toFixed(1) + " mm"
                            font.pixelSize: 13
                            color: "#333333"
                            font.bold: true
                        }
                    }               
                    Row {
                        width: parent.width
                        spacing: 5
                        
                        Text {
                            width: 80
                            text: "拉出值:"
                            horizontalAlignment: Text.AlignRight
                            font.pixelSize: 13
                            color: "#555555"
                        }
                        
                        Text {
                            id: zigValueText
                            text: Number(zig).toFixed(1) + " mm"
                            font.pixelSize: 13
                            color: "#333333"
                            font.bold: true
                        }
                    }
                    Row {
                        width: parent.width
                        spacing: 5
                        
                        Text {
                            width: 80
                            text: "磨耗:"
                            horizontalAlignment: Text.AlignRight
                            font.pixelSize: 13
                            color: "#555555"
                        }
                        
                        Text {
                            id: moHaoValueText
                            text: Number(moHao).toFixed(1) + " mm"
                            font.pixelSize: 13
                            color: "#333333"
                            font.bold: true
                        }
                    }
                    Row {
                        width: parent.width
                        spacing: 5
                        
                        Text {
                            width: 80
                            text: "网压值:"
                            horizontalAlignment: Text.AlignRight
                            font.pixelSize: 13
                            color: "#555555"
                        }
                        
                        Text {
                            id: voltageValueText
                            text: Number(voltage).toFixed(3) + " v"
                            font.pixelSize: 13
                            color: "#333333"
                            font.bold: true
                        }
                    }
                    Row {
                        width: parent.width
                        spacing: 5
                        
                        Text {
                            width: 80
                            text: "网流值:"
                            horizontalAlignment: Text.AlignRight
                            font.pixelSize: 13
                            color: "#555555"
                        }
                        
                        Text {
                            id: electriValueText
                            text: Number(electri).toFixed(3) + "A"
                            font.pixelSize: 13
                            color: "#333333"
                            font.bold: true
                        }
                    }
                    Row {
                        width: parent.width
                        spacing: 5

                        Text {
                            width: 80
                            text: "压力值:"
                            horizontalAlignment: Text.AlignRight
                            font.pixelSize: 13
                            color: "#555555"
                        }

                        GridLayout {
                            columns: 2                // 两列
                            columnSpacing: 15
                            rowSpacing: 5

                            /* 第 0 行：总压力（跨两列） */
                            Text {
                                text: "总压力值: " + Number(press).toFixed(1) + " N"
                                font.pixelSize: 13
                                color: "#333333"
                                font.bold: true
                                Layout.columnSpan: 2
                                Layout.alignment: Qt.AlignLeft
                            }

                            /* 第 1 行：压力值1 & 压力值2 */
                            Text {
                                text: "压力值1: " + Number(press1).toFixed(1) + " N"
                                font.pixelSize: 13
                                color: "#333333"
                                font.bold: true
                            }
                            Text {
                                text: "压力值2: " + Number(press2).toFixed(1) + " N"
                                font.pixelSize: 13
                                color: "#333333"
                                font.bold: true
                            }

                            /* 第 2 行：压力值3 & 压力值4 */
                            Text {
                                text: "压力值3: " + Number(press3).toFixed(1) + " N"
                                font.pixelSize: 13
                                color: "#333333"
                                font.bold: true
                            }
                            Text {
                                text: "压力值4: " + Number(press4).toFixed(1) + " N"
                                font.pixelSize: 13
                                color: "#333333"
                                font.bold: true
                            }
                        }
                    }

                    Row {
                        width: parent.width
                        spacing: 5
                        
                        Text {
                            width: 80
                            text: "硬点值:"
                            horizontalAlignment: Text.AlignRight
                            font.pixelSize: 13
                            color: "#555555"
                        }

                         ScrollView {
                            width: parent.width - 90  // 留出左侧标签空间
                            height: 30  // 固定高度避免挤压其他行
                            clip: true  // 裁剪超出部分

                            Row{
                                spacing: 15  // 控制项间距
                                Text {
                                    id: spotValueTextX
                                    text: "振动: "+Number(spotx).toFixed(1) + " G   "
                                    font.pixelSize: 13
                                    color: "#333333"
                                    font.bold: true
                                }

                                Text {
                                    id: spotValueTextY
                                    text: "冲击: "+Number(spoty).toFixed(1) + " G"
                                    font.pixelSize: 13
                                    color: "#333333"
                                    font.bold: true
                                }

                            }                      

                         }
                    }

                }
            }
            
            // 检测任务控制面板
            Rectangle {
                width: parent.width
                height: parent.height * 0.3
                color: "#f5f5f5"
                border.color: "#dddddd"
                radius: 4
                
                Rectangle {
                    width: parent.width
                    height: 30
                    color: "#2980b9"
                    radius: 4
                    
                    Text {
                        anchors.centerIn: parent
                        text: "检测任务控制"
                        color: "white"
                        font.pixelSize: 14
                        font.bold: true
                    }
                }
                
                Column {
                    anchors.fill: parent
                    anchors.topMargin: 40
                    anchors.leftMargin: 10
                    anchors.rightMargin: 10
                    anchors.bottomMargin: 10
                    spacing: 15
                    
                    Row {
                        width: parent.width
                        spacing: 5
                        
                        Text {
                            width: 80
                            text: "采集状态:"
                            horizontalAlignment: Text.AlignRight
                            font.pixelSize: 13
                            color: "#555555"
                        }
                        
                        Row {
                            spacing: 5
                            
                            Rectangle {
                                width: 14
                                height: 14
                                radius: 7
                                color: "#2ecc71"
                                border.color: "#27ae60"
                                border.width: 1
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            
                            Text {
                                text: "采集中"
                                font.pixelSize: 13
                                color: "#333333"
                                anchors.verticalCenter: parent.verticalCenter
                            }
                        }
                    }
                    
                    Button {
                        id: detectBtn
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: parent.width * 0.7
                        height: 36
                        text: detectBtn.checked ? "停止检测" : "开始检测"
                        font.pixelSize: 14
                        font.bold: true
                        
                        background: Rectangle {
                            radius: 4
                            color: detectBtn.checked ? "#e74c3c" : "#2ecc71"
                        }
                        
                        contentItem: Text {
                            text: detectBtn.text
                            font: detectBtn.font
                            color: "white"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        
                        checkable: true
                        onCheckedChanged: {
                            if (checked) {
                                // 如果任务窗口不存在才创建新窗口
                                if (!taskWindow) {
                                    createTaskWindow()
                                } else {
                                    // 如果窗口已存在，则显示它
                                    taskWindow.show()
                                    taskWindow.raise()  // 将窗口提到最前
                                    taskWindow.requestActivate()  // 请求激活窗口
                                }
                            } else {
                                // 停止检测逻辑
                                if (taskWindow) {
                                    taskWindow.hide() // 隐藏窗口而不是关闭它
                                    detectBtn.checked = false; // 确保按钮状态为未选中
            
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    // 底部状态栏
    Rectangle {
        id: statusBar
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 25
        color: "#f0f0f0"
        border.color: "#dddddd"
        
        Row {
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            height: parent.height
            spacing: 5
            
            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: "程序状态:"
                font.pixelSize: 12
                color: "#555555"
            }
            
            Rectangle {
                width: 12
                height: 12
                radius: 6
                color: "#2ecc71"
                anchors.verticalCenter: parent.verticalCenter
            }
            
            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: "正常"
                font.pixelSize: 12
                color: "#2ecc71"
                font.bold: true
            }
        }
        
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            text: "任务名称: " + (taskName || "无")
            font.pixelSize: 12
            color: "#555555"
        }
        
    }
    Connections {
        target: qmlCommunication
        function onrfid_count_sendFromCpp(data) {
            //console.log("收到RFID数据", JSON.stringify(data))

            // 无需使用getElementById，直接更新
            rfidValue.text = data || "无数据"
        }
        function onTaskname_recvFromCpp(data) {
            console.log("VIDEO收到任务名:", data)
            taskName = data
        }

        function onlinedata_recvFromCpp(stationObj) {     
            if (!stationObj || !stationObj.station_name) return;

            // 缓存所有站区数据
            allStationData[stationObj.station_name] = stationObj.items || [];

            // 判断是否已存在该站区
            let exists = false;
            for (let i = 0; i < stationListModel.count; i++) {
                if (stationListModel.get(i).station === stationObj.station_name) {
                    exists = true;
                    break;
                }
            }
            if (!exists) {
                stationListModel.append({ "station": stationObj.station_name });
            }

            // 如果是第一个站区，自动填充线路信息面板
            if (stationListModel.count === 1) {
                currentStationName = stationObj.station_name;
                refreshDataListModel(stationObj.station_name);
            }
        }
        function onjihedata_recvFromCpp(data) {
            //console.log("收到几何数据", JSON.stringify(data))
            if (!data || typeof data !== 'object') {
                console.error("无效的几何数据")
                return
            }

            let currentPositionType = positionTypeQueue.length > 0 ? 
                        positionTypeQueue.shift() : 
                        1;

            // 判断数据包类型
            if (data.zig0 !== undefined && data.hei0 !== undefined && data.moHaoLen !== undefined) {
                // 数据包包含 zig0 和 hei0，更新主线和散点
                zig = data.zig0 || 0
                hei = data.hei0 || 0
                moHao = data.moHaoLen || 0


                let zigdata = [zig,currentPositionType]
                //console.log("zigdata",JSON.stringify(zigdata))
                let heidata = [hei,currentPositionType]
                let moHaodata = [moHao,currentPositionType]

                zigchart.appendData(zigdata)
                heichart.appendData(heidata)
                moHaochart.appendData(moHaodata)
            
            } else {
                console.warn("未知数据包类型")
            }
        }
        function onegvlocation_pageFromCpp(data) {
            //console.log("收到定位数据", JSON.stringify(data))
            currentStationName = data.stationName || ""
            refreshDataListModel(currentStationName)
            currentPoleName = data.poleName || ""
            //console.log("当前杆号:", currentPoleName)
            speed = data.speed !== undefined ? data.speed : 0
            moveDistance = data.moveDistance !== undefined ? data.moveDistance : 0
            kilometer = data.kilometer !== undefined ? data.kilometer : 0

             // 读取 positionType 并存储到全局变量
            if (data.positionType !== undefined) {
                positionTypeQueue.push(data.positionType);
            } 
            // globalPositionType = data.positionType !== undefined ? data.positionType : 0;
            // console.log("位置类型:", positionTypeQueue[0]);

            let speeddata = [speed,0]
            speedchart.appendData(speeddata)
            centerStationInView(currentStationName)
            centerPoleInView(currentPoleName)

            canClose = false; // 接收到数据后不允许关闭

        }
        function onVoltage_data_sendFromCpp(data)
        {
            voltage = data || 0
        }
        function onElectri_data_sendFromCpp(data)
        {
            electri = data || 0
        }
        function onPress_data_sendFromCpp(data)
        {
            if (!data || typeof data !== 'object') {
                //console.error("无效的压力数据")
                return
            }
            press = data.press || 0
            press1 = data.press1 || 0
            press2 = data.press2 || 0
            press3 = data.press3 || 0
            press4 = data.press4 || 0
        }
        function onSpot_data_sendFromCpp(data)
        {
            if (!data || typeof data !== 'object') {
                //console.error("无效的硬点数据")
                return
            }
            spotx  = data.spotx || 0
            spoty  = data.spoty || 0
        }
    }

    //更新线路信息listmodel
    function refreshDataListModel(stationName) {
        dataListModel.clear();
        let items = allStationData[stationName] || [];
        items.forEach(function(item) {
            dataListModel.append({
                "pole": item.pole_name,
                "km": Number(item.kilo_meter).toFixed(3),
                "span": "8.00"
            });
        });
        // 自动选中第一个杆号
        if (dataListModel.count > 0)
            currentPoleName = dataListModel.get(0).pole;
        else
            currentPoleName = "";
    }

    function centerPoleInView(poleName) {
        if (isUserScrolling)
            return; // 用户正在滚动时不自动居中
        let index = -1;
        for (let i = 0; i < dataListModel.count; i++) {
            if (dataListModel.get(i).pole === poleName) {
                index = i;
                break;
            }
        }
        if (index !== -1) {
            dataListView.positionViewAtIndex(index, ListView.Center);
        }
    }

    function centerStationInView(stationName) {
        let index = -1;
        for (let i = 0; i < stationListModel.count; i++) {
            if (stationListModel.get(i).station === stationName) {
                index = i;
                break;
            }
        }
        if (index !== -1) {
            locationListView.positionViewAtIndex(index, ListView.Center);
        }
    }

    function createTaskWindow() {
        var component = Qt.createComponent("../egv_task_page/egv_task_page.qml");
        if (component.status === Component.Ready) {
            taskWindow = component.createObject(null, {
                "visible": true,
                "flags": Qt.Window,
                "width": 500,
                "height": 820-30,
                "x": (Screen.width - 500) / 2,
                "y": (Screen.height - 850) / 2
            });
            if (taskWindow === null) {
                console.log("Error creating task window");
            }
        } else if (component.status === Component.Error) {
            console.log("Error loading component:", component.errorString());
        }
    }

    Timer{
        id: scrollTimer
        interval: 2000
        repeat: false
        onTriggered: {
            isUserScrolling = false;
            centerPoleInView(currentPoleName);
        }
    }

    // 拦截关闭事件
    // onClosing: {
    //     if (!canClose) {
    //         close.accepted = false; // 阻止关闭
    //         closeTipDialog.open();
    //     }
    // }
}