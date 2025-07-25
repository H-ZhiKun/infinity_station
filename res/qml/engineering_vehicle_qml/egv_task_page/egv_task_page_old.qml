import QtQuick 
import QtQuick.Controls 
import QtQuick.Layouts 
import QtQuick.Window 
import InfinityStation 1.0
import QtQuick.Dialogs 

Window {
    width: 400
    height: 600
    minimumWidth: 400
    minimumHeight: 600
    maximumWidth: 400
    maximumHeight: 600
    visible: true
    title: "任务程序"

    // 修改后的属性定义 - 现在使用选中项的值而不是文本框的值
    property alias trainNumber: trainNumberField.text
    property alias taskname: taskNameField.text
    property alias lineName: lineNameField.currentText
    property alias lineDirection: lineDirectionField.currentText
    property alias startStation: startStationField.currentText
    property alias startPole: startPoleField.currentText
    property alias endStation: endStationField.currentText
    //property alias taskState: taskStateField.currentText
    property alias kilometer: kilometerField.text
    property alias runDirection: runDirectionField.currentText
    //property alias timeRange: timeRangeField.text
    property var onlineNameList: []
    property var stationData: null
    // 下拉选择数据源
    property var lineNameList: []
    property var startStationList: []
    property var lineDirectionList: []
    property var startPoleList: []
    property var endStationList: []
    property var kilometerList: []
    property var taskStateList: ["0","1"]
    property var runDirectionList: ["正向", "反向"]
    
    Menu {
        id: rightMenu
        y: buttonLineSelect.y + buttonLineSelect.height + 10
        x: buttonLineSelect.x + buttonLineSelect.width
        // 动态生成菜单项
        Repeater {
            model: onlineNameList
            MenuItem {
                text: modelData
                onTriggered: {
                    // 这里可以处理点击后的逻辑
                    qmlCommunication.behaviorFromQml(QmlCommunication.QmlActions.LineNameSend, text)
                    console.log("选择了线路:", text)
                }
            }
        }
    }
    
    Rectangle {
        anchors.fill: parent
        anchors.rightMargin: 0
        anchors.bottomMargin: 0
        color: "#f0f0f0"

        RowLayout {
            x: 23
            y: 8
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter
            spacing: 10

            // 打开文件按钮
            Button {
                text: "打开文件"
                Layout.preferredWidth: 08
                Layout.alignment: Qt.AlignHCenter
                onClicked: fileDialog.open()
            }

            // 开始按钮
            Button {
                text: "开始"
                Layout.preferredWidth: 80
                Layout.alignment: Qt.AlignHCenter
                onClicked: {
                    // 自动生成任务名称
                    taskNameField.text = trainNumberField.text + "-" + lineNameField.currentText;

                    // 构造数据对象
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
                        "runDirection": runDirectionField.currentText,  // 添加正反向
                    }
                    
                    // 发送数据
                    qmlCommunication.behaviorFromQml(QmlCommunication.QmlActions.TaskSend, taskData)
                }
            }

            // 结束按钮
            Button {
                text: "结束"
                Layout.preferredWidth: 80
                Layout.alignment: Qt.AlignHCenter
                onClicked: {
                    taskNameField.text = trainNumberField.text + "-" + lineNameField.currentText;

                    // 构造数据对象
                    let taskData = {
                        "trainNumber": trainNumberField.text,
                        "taskName": taskNameField.text,
                        "lineName": lineNameField.currentText,
                        "lineDirection": lineDirectionField.currentText,
                        "startStation": startStationField.currentText,
                        "startPole": startPoleField.currentText,
                        "endStation": endStationField.currentText,
                        "taskState": "1",
                        "taskType": "0",
                        "kilometer": kilometerField.text,
                        "runDirection": runDirectionField.currentText,  // 添加正反向
                    }
                    qmlCommunication.behaviorFromQml(QmlCommunication.QmlActions.TaskSend, taskData)
                }
            }

            // 线路选择按钮
            Button {
                id: buttonLineSelect
                text: "线路选择"
                Layout.preferredWidth: 80
                Layout.alignment: Qt.AlignHCenter

                onClicked: {
                    qmlCommunication.behaviorFromQml(QmlCommunication.QmlActions.LineBtnChoose,"1")
                    rightMenu.open()
                    
                    console.log("线路选择按钮被点击")
                }
            }
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20
            anchors.leftMargin: 18
            anchors.rightMargin: 22
            anchors.topMargin: 113
            anchors.bottomMargin: 127
            spacing: 10

            GridLayout {
                columns: 3
                columnSpacing: 10
                rowSpacing: 15
                Layout.fillWidth: true

                // 车号
                Label {
                    text: "车 号:"
                    Layout.alignment: Qt.AlignRight
                }
                TextField {
                    id: trainNumberField
                    Layout.fillWidth: true
                    text: "S1011-2"
                    Layout.columnSpan: 2
                }

                // 任务名称
                Label {
                    text: "任务名称:"
                    Layout.alignment: Qt.AlignRight
                }
                TextField {
                    id: taskNameField
                    Layout.fillWidth: true
                    Layout.columnSpan: 2
                }

                // 线路名称
                Label {
                    text: "线路名称:"
                    Layout.alignment: Qt.AlignRight
                }
                ComboBox {
                    id: lineNameField
                    model: lineNameList
                    Layout.fillWidth: true
                    Layout.columnSpan: 2
                    
                    // 当线路名称变化时，可能需要更新相关数据
                    onCurrentTextChanged: {
                        if (currentText !== "") {
                            // 这里可以添加任何基于线路名称变化时需要执行的逻辑
                            console.log("选择的线路: " + currentText)
                        }
                    }
                }

                // 线路方向
                Label {
                    text: "线路方向:"
                    Layout.alignment: Qt.AlignRight
                }
                ComboBox {
                    id: lineDirectionField
                    model: lineDirectionList
                    Layout.fillWidth: true
                    Layout.columnSpan: 2
                }
                Label {
                    text: "正反向:"
                    Layout.alignment: Qt.AlignRight
                }
                ComboBox {
                    id: runDirectionField
                    model: runDirectionList
                    Layout.fillWidth: true
                    Layout.columnSpan: 2
                }

                // 起始站点
                Label {
                    text: "起始站点:"
                    Layout.alignment: Qt.AlignRight
                }
                ComboBox {
                    id: startStationField
                    model: startStationList
                    Layout.fillWidth: true
                    Layout.columnSpan: 2
                    onCurrentTextChanged: {
                        if (currentText && stationData && stationData[currentText]) {
                            // 更新该站区下的杆号列表
                            let poles = stationData[currentText]
                            let poleNames = poles.map(p => p.pole_name)
                            startPoleField.model = poleNames
                            startPoleField.currentIndex = 0
                            console.log("站区改变，更新杆号列表:", poleNames)
                        }
                    }
                }

                // 起始计号
                Label {
                    text: "起始杆号:"
                    Layout.alignment: Qt.AlignRight
                }
                ComboBox {
                    id: startPoleField
                    model: startPoleList
                    Layout.fillWidth: true
                    Layout.columnSpan: 2
                    onCurrentTextChanged: {
                        if (currentText !== "") {
                            updateKilometer()
                            console.log("杆号改变，更新公里标")
                        }
                    }
                }
                
                Label {
                    text: "公里标:"
                    Layout.alignment: Qt.AlignRight
                }

                TextField {
                    id: kilometerField
                    //model: kilometerList
                    Layout.fillWidth: true
                    Layout.columnSpan: 2
                    readOnly: true
                    validator: DoubleValidator {
                        bottom: 0
                        decimals: 3
                        notation: DoubleValidator.StandardNotation
                    }
                    // 当手动输入时格式化为3位小数
                    onTextChanged: {
                        if (!readOnly && text) {
                            let num = parseFloat(text)
                            if (!isNaN(num)) {
                                text = num.toFixed(3)
                            }
                        }
                    }
                }

                // 终到站点
                Label {
                    text: "终止站点:"
                    Layout.alignment: Qt.AlignRight
                }
                ComboBox {
                    id: endStationField
                    model: endStationList
                    Layout.fillWidth: true
                    Layout.columnSpan: 2
                    onCurrentTextChanged: {
                        if (currentText !== "") {
                            updateRunDirection()  // 在结束站区变化时更新正反向
                        }
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                Layout.topMargin: 20
                spacing: 10

                // 使用一个空Item来填充左侧空间
                Item {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignLeft
                }

                // 按钮们将被居中
                Button {
                    text: "删除"
                    Layout.preferredWidth: 80
                    Layout.alignment: Qt.AlignHCenter
                    onClicked: {
                        // 可以添加删除逻辑
                    }
                }
                
                Button {
                    text: "清空"
                    Layout.preferredWidth: 80
                    Layout.alignment: Qt.AlignHCenter
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
                    Layout.preferredWidth: 80
                    Layout.alignment: Qt.AlignHCenter
                    onClicked: {
                        // 实时功能逻辑
                    }
                }

                // 使用另一个空Item来填充右侧空间
                Item {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignRight
                }
            }
        }

        // 文件对话框
        FileDialog {
            id: fileDialog
            title: "选择任务文件"
            nameFilters: ["All files (*)"]

            onAccepted: {
                // 只处理第一个文件
                let filePath = selectedFiles.length > 0 ? selectedFiles[0].toString() : "";
                console.log("选择的文件路径:", filePath);

                // 只发送文件路径
                qmlCommunication.behaviorFromQml(
                    QmlCommunication.QmlActions.TaskCSVSend,
                    JSON.stringify({
                        action: "file_selected",
                        filepath: filePath
                    })
                )
            }
        }
    }
    Connections {
        target: qmlCommunication

        function onlineNameList_recvFromCpp(data) {
            console.log("收到线路数据:", data)
            //如果需要调试，可以打印接收到的数据
            if (Array.isArray(data)) {
                console.log("线路列表数量:", data.length)
                data.forEach((item, index) => {
                    console.log(`线路 ${index + 1}:`, item)
                })
            }
            // 直接将数据赋值给 onlineNameList
            onlineNameList = data
            
            // 如果需要调试，可以打印接收到的数据
            if (Array.isArray(data)) {
                console.log("线路列表数量:", data.length)
                data.forEach((item, index) => {
                    console.log(`线路 ${index + 1}:`, item)
                })
            }
        }
    }
    // 处理从C++接收的数据
    Connections {
        target: qmlCommunication
        
        function onlinedata_recvFromCpp(jsonData) {
            //console.info("收到数据:", JSON.stringify(jsonData))
            
            // 检查数据有效性
            if (!jsonData || typeof jsonData !== 'object') {
                console.error("无效的JSON数据:", jsonData)
                return
            }

            // 首先保存完整的数据
            stationData = jsonData

            // 用于存储去重后的数据
            let lineNameSet = new Set()
            let maoDuanSet = new Set()
            let poleNameSet = new Set()
            let lineDirSet = new Set()
            let kilometerSet = new Set()

            // 遍历所有站点的数据
            for (let station in jsonData) {
                let poleList = jsonData[station]
                poleList.forEach(pole => {
                    if (pole.line_name) lineNameSet.add(pole.line_name.trim())
                    if (pole.maoduan_name) maoDuanSet.add(pole.maoduan_name)
                    if (pole.pole_name) poleNameSet.add(pole.pole_name)
                    if (pole.line_dir != null) lineDirSet.add(pole.line_dir === 1 ? "下行" : "上行")
                    if (pole.kilo_meter) kilometerSet.add(pole.kilo_meter)
                })
            }

            // 转换Set为数组并更新模型
            lineNameList = Array.from(lineNameSet)
            startStationList = Object.keys(jsonData) // 站点名称直接作为键
            lineDirectionList = Array.from(lineDirSet)
            startPoleList = Array.from(poleNameSet)
            //kilometerList = Array.from(kilometerSet)
            endStationList = [...startStationList].reverse() // 复制站点列表并反转

            // 更新下拉框
            lineNameField.model = lineNameList
            startStationField.model = startStationList
            lineDirectionField.model = lineDirectionList
            startPoleField.model = startPoleList
            //kilometerField.model = kilometerList
            endStationField.model = endStationList

            // 设置默认选择
            if (lineNameList.length > 0) lineNameField.currentIndex = 0
            if (startStationList.length > 0) startStationField.currentIndex = 0
            if (lineDirectionList.length > 0) lineDirectionField.currentIndex = 0
            if (startPoleList.length > 0) startPoleField.currentIndex = 0
            //if (kilometerList.length > 0) kilometerField.currentIndex = 0
            if (endStationList.length > 0) endStationField.currentIndex = 0


            // 打印调试信息
            console.log("线路列表:", lineNameList)
            console.log("站点列表:", startStationList)
            console.log("行别列表:", lineDirectionList)
            console.log("杆号列表:", startPoleList)
        }
    }




    Connections {
        target: qmlCommunication
        
        function onlinedata_recvFromCpp(jsonData) {
            //console.info("收到数据:", JSON.stringify(jsonData))
            
            // 检查数据有效性
            if (!jsonData || typeof jsonData !== 'object') {
                console.error("无效的JSON数据:", jsonData)
                return
            }

            // 首先保存完整的数据
            stationData = jsonData

            // 用于存储去重后的数据
            let lineNameSet = new Set()
            let maoDuanSet = new Set()
            let poleNameSet = new Set()
            let lineDirSet = new Set()
            let kilometerSet = new Set()

            // 遍历所有站点的数据
            for (let station in jsonData) {
                let poleList = jsonData[station]
                poleList.forEach(pole => {
                    if (pole.line_name) lineNameSet.add(pole.line_name.trim())
                    if (pole.maoduan_name) maoDuanSet.add(pole.maoduan_name)
                    if (pole.pole_name) poleNameSet.add(pole.pole_name)
                    if (pole.line_dir != null) lineDirSet.add(pole.line_dir === 1 ? "下行" : "上行")
                    if (pole.kilo_meter) kilometerSet.add(pole.kilo_meter)
                })
            }

            // 转换Set为数组并更新模型
            lineNameList = Array.from(lineNameSet)
            startStationList = Object.keys(jsonData) // 站点名称直接作为键
            lineDirectionList = Array.from(lineDirSet)
            startPoleList = Array.from(poleNameSet)
            //kilometerList = Array.from(kilometerSet)
            endStationList = [...startStationList].reverse() // 复制站点列表并反转

            // 更新下拉框
            lineNameField.model = lineNameList
            startStationField.model = startStationList
            lineDirectionField.model = lineDirectionList
            startPoleField.model = startPoleList
            //kilometerField.model = kilometerList
            endStationField.model = endStationList

            // 设置默认选择
            if (lineNameList.length > 0) lineNameField.currentIndex = 0
            if (startStationList.length > 0) startStationField.currentIndex = 0
            if (lineDirectionList.length > 0) lineDirectionField.currentIndex = 0
            if (startPoleList.length > 0) startPoleField.currentIndex = 0
            //if (kilometerList.length > 0) kilometerField.currentIndex = 0
            if (endStationList.length > 0) endStationField.currentIndex = 0


            // 打印调试信息
            console.log("线路列表:", lineNameList)
            console.log("站点列表:", startStationList)
            console.log("行别列表:", lineDirectionList)
            console.log("杆号列表:", startPoleList)
        }
    }

    function updateRunDirection() {
    let startStation = startStationField.currentText
    let endStation = endStationField.currentText
    
    // 只有当两个站区都选择后才更新方向
    if (startStation && endStation) {
        let startIndex = startStationList.indexOf(startStation)
        let endIndex = startStationList.indexOf(endStation)
        
        if (startIndex !== -1 && endIndex !== -1) {
            // 如果起始站区索引小于结束站区索引，则为正向
            runDirectionField.currentIndex = startIndex < endIndex ? 0 : 1
            console.log("正反向更新:", 
                      "起始站区:", startStation, "索引:", startIndex,
                      "结束站区:", endStation, "索引:", endIndex,
                      "方向:", runDirectionField.currentText)
        }
    }
}

function updateKilometer() {
    let station = startStationField.currentText
    let pole = startPoleField.currentText
    
    // 只有当站区和杆号都选择后才更新公里标
    if (station && pole && stationData && stationData[station]) {
        // 在当前站区数据中查找对应杆号的数据
        let poleData = stationData[station].find(p => p.pole_name === pole)
        
        if (poleData && poleData.kilo_meter !== undefined) {
            // 找到对应的公里标数据
            kilometerField.text = poleData.kilo_meter.toString()
            kilometerField.readOnly = true  // 找到数据时设为只读
            console.log("公里标更新:", 
                      "站区:", station,
                      "杆号:", pole,
                      "公里标:", poleData.kilo_meter)
        } else {
            // 没有找到对应的公里标数据
            kilometerField.text = ""
            kilometerField.readOnly = false  // 没有数据时允许手动输入
            console.log("未找到公里标数据:", 
                      "站区:", station,
                      "杆号:", pole)
        }
    } else {
        // 未选择站区或杆号，清空文本框
        kilometerField.text = ""
        kilometerField.readOnly = false
    }
}
}