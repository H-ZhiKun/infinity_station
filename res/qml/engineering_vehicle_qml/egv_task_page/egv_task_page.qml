import QtQuick 
import QtQuick.Controls 
import QtQuick.Layouts 
import QtQuick.Window 
import InfinityStation 1.0
import QtQuick.Dialogs 
import QtQuick.Controls.Material    

Window {
    Material.theme: Material.Light
    width: 480
    height: 780
    minimumWidth: 480
    minimumHeight: 780
    maximumWidth: 480
    maximumHeight: 780
    visible: true
    title: "任务程序"

    // 修改后的属性定义
    property alias trainNumber: trainNumberField.text
    property alias taskname: taskNameField.text
    property alias lineName: lineNameField.currentText
    property alias lineDirection: lineDirectionField.currentText
    property alias startStation: startStationField.currentText
    property alias startPole: startPoleField.currentText
    property alias endStation: endStationField.currentText
    property alias kilometer: kilometerField.text
    property alias runDirection: runDirectionField.currentText
    property bool isTaskStarted: false
    property var onlineNameList: []
    property var stationData: null
    property var lineNameList: []
    property var startStationList: []
    property var lineDirectionList: []
    property var startPoleList: []
    property var endStationList: []
    property var kilometerList: []
    property var taskStateList: ["0","1"]
    property var runDirectionList: ["正向", "反向"]
    property var allStationData: ({})
    property var allStationList: []
    property bool firstStationArrived: false
    property bool isWaitingForLineData: false
    property string currentSelectedLine: ""

    // 主背景
    Rectangle {
        anchors.fill: parent
        color: "#f5f5f5"
    }

    // 顶部按钮区域
    Rectangle {
        id: buttonBar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 60
        color: "#eeeeee"

        RowLayout {
            anchors.centerIn: parent
            spacing: 15

            // 开始按钮
            Button {
                id: startButton
                text: "开始"
                Material.background: isTaskStarted ? Material.Grey : Material.Green
                Material.foreground: "white"
                enabled: !isTaskStarted && lineNameField.currentIndex >= 0
                onClicked: {
                    isTaskStarted = true
                    taskNameField.text = trainNumberField.text + "-" + lineNameField.currentText;

                    let taskData = {
                        "trainNumber": trainNumberField.text,
                        "taskName": taskNameField.text,
                        "lineName": lineNameField.currentText,
                        "lineDirection": lineDirectionField.currentText,
                        "startStation": startStationField.currentText,
                        "startPole": startPoleField.currentText,
                        "endStation": endStationField.currentText,
                        "taskState": "0",
                        "taskType": "0",
                        "kilometer": kilometerField.text,
                        "runDirection": runDirectionField.currentText === "正向" ? "0" : "1"
                    }
                    
                    qmlCommunication.behaviorFromQml(QmlCommunication.QmlActions.TaskSend, taskData)
                }
            }

            // 结束按钮
            Button {
                id: endButton
                text: "结束"
                Material.background: Material.Red
                Material.foreground: "white"
                enabled: isTaskStarted
                onClicked: {
                    isTaskStarted = false
                    let taskData = {"taskState": "1"}
                    qmlCommunication.behaviorFromQml(QmlCommunication.QmlActions.TaskSend, taskData)
                }
            }

            // 线路选择按钮
            Button {
                id: buttonLineSelect
                text: "线路选择"
                Material.background: Material.Blue
                Material.foreground: "white"
                enabled: !isTaskStarted
                onClicked: {
                    qmlCommunication.behaviorFromQml(QmlCommunication.QmlActions.LineBtnChoose,"1")
                    rightMenu.open()
                }
            }
        }
    }

    // 主内容区域
    ScrollView {
        anchors.top: buttonBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: bottomBar.top
        anchors.margins: 10
        clip: true

        GridLayout {
            width: parent.width +180
            columns: 2
            columnSpacing: 15
            rowSpacing: 12

            // 第一行：车号
            Label {
                text: "车号:"
                Layout.alignment: Qt.AlignRight
                font.bold: true
            }
            TextField {
                id: trainNumberField
                Layout.fillWidth: true
                text: "S1011-2"
                placeholderText: "请输入车号"
            }

            // 第二行：任务名称
            Label {
                text: "任务名称:"
                Layout.alignment: Qt.AlignRight
                font.bold: true
            }
            TextField {
                id: taskNameField
                Layout.fillWidth: true
                placeholderText: "自动生成"
                readOnly: true
            }

            // 线路名称
            Label {
                text: "线路名称:"
                Layout.alignment: Qt.AlignRight
                font.bold: true
            }
            ComboBox {
                id: lineNameField
                model: lineNameList
                Layout.fillWidth: true
                enabled: !isTaskStarted
            }

            // 线路方向
            Label {
                text: "线路方向:"
                Layout.alignment: Qt.AlignRight
                font.bold: true
            }
            ComboBox {
                id: lineDirectionField
                model: lineDirectionList
                Layout.fillWidth: true
                enabled: !isTaskStarted
            }

            // 运行方向
            Label {
                text: "正反向:"
                Layout.alignment: Qt.AlignRight
                font.bold: true
            }
            ComboBox {
                id: runDirectionField
                model: runDirectionList
                Layout.fillWidth: true
                enabled: !isTaskStarted
            }

            // 起始站点
            Label {
                text: "起始站点:"
                Layout.alignment: Qt.AlignRight
                font.bold: true
            }
            ComboBox {
                id: startStationField
                model: startStationList
                Layout.fillWidth: true
                enabled: !isTaskStarted
                onCurrentTextChanged: {
                    if (currentText && allStationData && allStationData[currentText]) {
                        let poles = allStationData[currentText];
                        let poleNames = poles.map(p => p.pole_name);
                        startPoleList = poleNames;
                        startPoleField.model = startPoleList;
                        startPoleField.currentIndex = 0;
                        updateKilometer();
                    }
                }
            }

            // 起始杆号
            Label {
                text: "起始杆号:"
                Layout.alignment: Qt.AlignRight
                font.bold: true
            }
            ComboBox {
                id: startPoleField
                model: startPoleList
                Layout.fillWidth: true
                enabled: !isTaskStarted
                onCurrentTextChanged: updateKilometer()
            }

            // 公里标
            Label {
                text: "公里标:"
                Layout.alignment: Qt.AlignRight
                font.bold: true
            }
            TextField {
                id: kilometerField
                Layout.fillWidth: true
                readOnly: true
                validator: DoubleValidator {
                    bottom: 0
                    decimals: 3
                    notation: DoubleValidator.StandardNotation
                }
            }

            // 终止站点
            Label {
                text: "终止站点:"
                Layout.alignment: Qt.AlignRight
                font.bold: true
            }
            ComboBox {
                id: endStationField
                model: endStationList
                Layout.fillWidth: true
                enabled: !isTaskStarted
                onCurrentTextChanged: updateRunDirection()
            }
        }
    }

    // 底部按钮区域
    Rectangle {
        id: bottomBar
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: 60
        color: "#eeeeee"

        RowLayout {
            anchors.centerIn: parent
            spacing: 15

            Button {
                text: "删除"
                Material.background: Material.Grey
                Material.foreground: "white"
                onClicked: {
                    // 删除逻辑
                }
            }
            
            Button {
                text: "清空"
                Material.background: Material.Grey
                Material.foreground: "white"
                onClicked: {
                    trainNumberField.text = ""
                    taskNameField.text = ""
                    lineNameField.currentIndex = -1
                    lineDirectionField.currentIndex = -1
                    startStationField.currentIndex = -1
                    startPoleField.currentIndex = -1
                    endStationField.currentIndex = -1
                }
            }
            
            Button {
                text: "实时"
                Material.background: Material.Blue
                Material.foreground: "white"
                onClicked: {
                    // 实时功能逻辑
                }
            }
        }
    }

    // 线路数据菜单
    Menu {
        id: rightMenu
        y: buttonLineSelect.y + buttonLineSelect.height + 10
        x: buttonLineSelect.x + buttonLineSelect.width
        
        Repeater {
            model: onlineNameList
            MenuItem {
                text: modelData
                onTriggered: {
                    if (currentSelectedLine === text) return;
                    currentSelectedLine = text;
                    clearAllData();
                    isWaitingForLineData = true;
                    qmlCommunication.behaviorFromQml(QmlCommunication.QmlActions.LineNameSend, text);
                }
            }
        }
    }
    
    function clearAllData() {
        lineNameList = [];
        lineDirectionList = [];
        startStationList = [];
        startPoleList = [];
        endStationList = [];
        kilometerField.text = "";
        taskNameField.text = "";
        allStationData = {};
        allStationList = [];
        
        lineNameField.model = [];
        lineDirectionField.model = [];
        startStationField.model = [];
        startPoleField.model = [];
        endStationField.model = [];
        
        startStationField.currentIndex = -1;
        startPoleField.currentIndex = -1;
        endStationField.currentIndex = -1;
        lineNameField.currentIndex = -1;
        lineDirectionField.currentIndex = -1;
    }

    // 连接信号
    Connections {
        target: qmlCommunication
        function onLineNameList_recvFromCpp(data) { onlineNameList = data }
        function onTaskname_recvFromCpp(data) { taskNameField.text = data }
        function onLinedata_recvFromCpp(jsonData) {
            let dataArr = Array.isArray(jsonData) ? jsonData : [jsonData];
            
            dataArr.forEach(stationItem => {
                let stationName = stationItem.station_name || "";
                if (!stationName) return;
                
                if (!allStationList.includes(stationName)) {
                    allStationList.push(stationName);
                }
                
                allStationData[stationName] = stationItem.items || [];
            });

            startStationList = allStationList.slice();
            startStationField.model = startStationList;
            endStationList = allStationList.slice();
            endStationField.model = endStationList;

            if (startStationList.length > 0) {
                startStationField.currentIndex = 0;
            }

            Qt.callLater(function() {
                let firstStation = startStationList[0];
                if (firstStation && allStationData[firstStation]) {
                    let items = allStationData[firstStation] || [];
                    let poleNameSet = new Set();
                    let lineNameSet = new Set();
                    let lineDirSet = new Set();
                    
                    items.forEach(item => {
                        if (item.pole_name) poleNameSet.add(item.pole_name);
                        if (item.line_name) lineNameSet.add(item.line_name.trim());
                        if (item.line_dir !== undefined)
                            lineDirSet.add(item.line_dir === 1 ? "下行" : "上行");
                    });
                    
                    stationData = allStationData;
                    startPoleList = Array.from(poleNameSet);
                    lineNameList = Array.from(lineNameSet);
                    lineDirectionList = Array.from(lineDirSet);

                    lineNameField.model = lineNameList;
                    lineDirectionField.model = lineDirectionList;
                    startPoleField.model = startPoleList;
                    
                    if (lineNameList.length > 0) lineNameField.currentIndex = 0;
                    if (lineDirectionList.length > 0) lineDirectionField.currentIndex = 0;
                    if (startPoleList.length > 0) startPoleField.currentIndex = 0;
                    if (endStationList.length > 0) endStationField.currentIndex = 0;
                    
                    isWaitingForLineData = false;
                    updateKilometer();
                }
            });
        }
    }

    function updateRunDirection() {
        let startStation = startStationField.currentText;
        let endStation = endStationField.currentText;
        if (startStation && endStation) {
            let startIndex = startStationList.indexOf(startStation);
            let endIndex = startStationList.indexOf(endStation);
            if (startIndex !== -1 && endIndex !== -1) {
                runDirectionField.currentIndex = startIndex < endIndex ? 0 : 1;
            }
        }
    }

    function updateKilometer() {
        let station = startStationField.currentText;
        let pole = startPoleField.currentText;
        if (station && pole && allStationData && allStationData[station]) {
            let poleData = allStationData[station].find(p => p.pole_name === pole);
            if (poleData && poleData.kilo_meter !== undefined) {
                kilometerField.text = poleData.kilo_meter.toString();
                kilometerField.readOnly = true;
            } else {
                kilometerField.text = "";
                kilometerField.readOnly = false;
            }
        } else {
            kilometerField.text = "";
            kilometerField.readOnly = false;
        }
    }
}