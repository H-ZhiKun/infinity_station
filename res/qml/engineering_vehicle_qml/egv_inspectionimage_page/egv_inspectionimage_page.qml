import QtQuick 
import QtQuick.Controls 
import QtQuick.Layouts 
import QtQuick.Window 
import InfinityStation 1.0

Window {
    id: mainWindow
    visible: true
    width: 1280
    height: 800
    title: "巡检拍照程序"
    color: "#f0f0f0"

    property string displayType:"all" // 可选值: "all", "flexible", "rigid", "wire"
    property string currentTaskName: "无任务"
    property string currentLine: "上海6号线-上行"
    property string currentStation: "某某站区"
    property string currentPole: "杆号"
    property var currentSelectedImage: ({
        name: "",
        serial: "",
        count: "",
        status: "green",
        imageSource: ""  // 添加图像源属性,
    })
    property ListModel cameraListModel: ListModel { }
    

    SplitView {
        anchors.top: parent.top
        width: parent.width
        height: parent.height - statusBar.height
        orientation: Qt.Horizontal
        
        // Left panel - Camera list and related info
        Rectangle {
            SplitView.preferredWidth: 320
            SplitView.minimumWidth: 250
            
            ColumnLayout {
                anchors.fill: parent
                spacing: 0
                
                // Camera list panel
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: parent.height / 2
                    
                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 0
                        
                        // Title with close button
                        Rectangle {
                            Layout.fillWidth: true
                            height: 30
                            color: "#f0f0f0"
                            
                            Text {
                                anchors.left: parent.left
                                anchors.leftMargin: 10
                                anchors.verticalCenter: parent.verticalCenter
                                text: "相机列表"
                                font.pixelSize: 14
                            }
                            
                            Button {
                                anchors.right: parent.right
                                anchors.rightMargin: 5
                                anchors.verticalCenter: parent.verticalCenter
                                text: "获取相机状态"
                                width: 80
                                height: 30
                                flat: true
                                onClicked: {
                                    console.log("获取相机状态按钮被点击");
                                    qmlCommunication.behaviorFromQml(QmlCommunication.QmlActions.camera_button_clicked,"1");
                                }
                            }
                        }
                        
                        // Header row
                        Rectangle {
                            Layout.fillWidth: true
                            height: 30
                            color: "#f0f0f0"
                            
                            RowLayout {
                                anchors.fill: parent
                                spacing: 0
                                
                                Text {
                                    Layout.preferredWidth: parent.width * 0.4
                                    text: "相机名称"
                                    horizontalAlignment: Text.AlignHCenter
                                    font.pixelSize: 12
                                }
                                
                                Text {
                                    Layout.preferredWidth: parent.width * 0.4
                                    text: "序列号"
                                    horizontalAlignment: Text.AlignHCenter
                                    font.pixelSize: 12
                                }
                                
                                Text {
                                    Layout.preferredWidth: parent.width * 0.2
                                    text: "触发次数"
                                    horizontalAlignment: Text.AlignHCenter
                                    font.pixelSize: 12
                                }
                            }
                        }

                        ScrollView {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            clip: true
                            
                            ScrollBar.vertical: ScrollBar {
                                active: true
                                policy: ScrollBar.AsNeeded
                            }

                            // Camera list
                            ListView {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                clip: true
                                model: cameraListModel
                                
                                delegate: Rectangle {
                                    width: ListView.view.width
                                    height: 40
                                    color: index % 2 === 0 ? "#ffffff" : "#f5f5f5"
                                    
                                    RowLayout {
                                        anchors.fill: parent
                                        spacing: 0
                                        
                                        Rectangle {
                                            Layout.preferredWidth: 20
                                            Layout.fillHeight: true
                                            color: "transparent"
                                            
                                            Rectangle {
                                                width: 10
                                                height: 10
                                                radius: 5
                                                color: model.status === "red" ? "green" : "red"
                                                anchors.centerIn: parent
                                            }
                                        }
                                        
                                        Text {
                                            Layout.preferredWidth: parent.width * 0.4 - 20
                                            text: model.name
                                            horizontalAlignment: Text.AlignLeft
                                            verticalAlignment: Text.AlignVCenter
                                            elide: Text.ElideRight
                                            font.pixelSize: 12
                                        }
                                        
                                        Text {
                                            Layout.preferredWidth: parent.width * 0.4
                                            text: model.serial
                                            horizontalAlignment: Text.AlignHCenter
                                            verticalAlignment: Text.AlignVCenter
                                            elide: Text.ElideRight
                                            font.pixelSize: 12
                                        }
                                        
                                        Text {
                                            Layout.preferredWidth: parent.width * 0.2
                                            text: model.count
                                            horizontalAlignment: Text.AlignHCenter
                                            verticalAlignment: Text.AlignVCenter
                                            font.pixelSize: 12
                                        }
                                    }
                                }
                            }
                        }

                    }
                }
                
                // Configuration panel
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    
                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 0
                        
                        // Title with close button
                        Rectangle {
                            Layout.fillWidth: true
                            height: 30
                            color: "#f0f0f0"
                            
                            Text {
                                anchors.left: parent.left
                                anchors.leftMargin: 10
                                anchors.verticalCenter: parent.verticalCenter
                                text: "相机参数配置"
                                font.pixelSize: 14
                            }
                            
                            ComboBox {
                                anchors.right: closeBtn.left
                                anchors.rightMargin: 5
                                anchors.verticalCenter: parent.verticalCenter
                                width: 100
                                height: 25
                                model: ["请选择"]
                            }
                            
                            Button {
                                id: closeBtn
                                anchors.right: parent.right
                                anchors.rightMargin: 5
                                anchors.verticalCenter: parent.verticalCenter
                                text: "×"
                                width: 20
                                height: 20
                                flat: true
                            }
                        }
                        
                        // Parameter list
                        ListView {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            clip: true
                            model: ListModel {
                                ListElement { param: "保存路径"; value: "D:\\TEST" }
                                ListElement { param: "Baud"; value: "-15872" }
                                ListElement { param: "Com"; value: "1" }
                                ListElement { param: "IOCanIndex"; value: "-1" }
                                ListElement { param: "IsOnlineUse"; value: "0" }
                                ListElement { param: "IsSetCallBack"; value: "0" }
                                ListElement { param: "IsUseJiHeTrigger"; value: "0" }
                                ListElement { param: "LocalPort"; value: "5190" }
                                ListElement { param: "MaxDistance"; value: "-1" }
                                ListElement { param: "MinDistance"; value: "-1" }
                                ListElement { param: "PictureViewPort"; value: "5183" }
                                ListElement { param: "ReceiveTaskPort"; value: "15189" }
                                ListElement { param: "RemotePort"; value: "5180" }
                                ListElement { param: "issave.jpg"; value: "1" }
                            }
                            
                            delegate: Rectangle {
                                width: ListView.view.width
                                height: 25
                                color: index % 2 === 0 ? "#f5f5f5" : "#ffffff"
                                
                                RowLayout {
                                    anchors.fill: parent
                                    spacing: 0
                                    
                                    Text {
                                        Layout.preferredWidth: parent.width * 0.5
                                        text: model.param
                                        horizontalAlignment: Text.AlignLeft
                                        verticalAlignment: Text.AlignVCenter
                                        leftPadding: 10
                                        font.pixelSize: 12
                                    }
                                    
                                    Text {
                                        Layout.preferredWidth: parent.width * 0.5
                                        text: model.value
                                        horizontalAlignment: Text.AlignLeft
                                        verticalAlignment: Text.AlignVCenter
                                        leftPadding: 10
                                        font.pixelSize: 12
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        
        // Middle panel - Detection display (占用剩余全部空间)
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                // Title with close button
                Rectangle {
                    Layout.fillWidth: true
                    height: 30
                    color: "#f0f0f0"

                    Text {
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        anchors.verticalCenter: parent.verticalCenter
                        text: "检测画面"
                        font.pixelSize: 14
                    }

                    Button {
                        anchors.right: parent.right
                        anchors.rightMargin: 5
                        anchors.verticalCenter: parent.verticalCenter
                        text: "×"
                        width: 20
                        height: 20
                        flat: true
                    }
                }

                // Filter options
                Rectangle {
                    Layout.fillWidth: true
                    height: 80
                    color: "#ffffff"
                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 5
                        spacing: 5

                        // 第一行
                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 10
                            ButtonGroup { id: displayTypeGroup; exclusive: true }
                            RadioButton {
                                text: "全部显示"
                                checked: true
                                ButtonGroup.group: displayTypeGroup
                                onCheckedChanged: if (checked) displayType = "all"
                            }
                            RadioButton {
                                text: "柔性悬挂"
                                ButtonGroup.group: displayTypeGroup
                                onCheckedChanged: if (checked) displayType = "flexible"
                            }
                            RadioButton {
                                text: "刚性悬挂"
                                ButtonGroup.group: displayTypeGroup
                                onCheckedChanged: if (checked) displayType = "rigid"
                            }
                        }

                        // 第二行
                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 10
                            RadioButton {
                                text: "柔性吊弦"
                                ButtonGroup.group: displayTypeGroup
                                onCheckedChanged: if (checked) displayType = "wire"
                            }
                            Item { Layout.fillWidth: true }

                            // 添加站区和杆号信息
                            Text {
                                text: "站区: " + currentStation + "  杆号: " + currentPole
                                font.pixelSize: 12
                            }
                        }
                    }
                }

                // Flexible suspension section
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    color: "#f0f0f0"

                    Text {
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        anchors.verticalCenter: parent.verticalCenter
                        text: "柔性悬挂"
                        font.pixelSize: 14
                        color: "#0066cc"
                    }
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true

                    ScrollBar.vertical: ScrollBar {
                        active: true
                        policy: ScrollBar.AsNeeded
                    }

                    GridView {
                        id: previewGrid
                        anchors.fill: parent
                        anchors.margins: 10
                        cellWidth: width / 3  // 改为3列显示
                        cellHeight: cellWidth + 40  // 额外空间用于显示信息
                        model: cameraListModel

                        // 过滤器函数
                        function filterModel(item) {
                            if (displayType === "all") return true;
                            if (displayType === "flexible" && item.name && item.name.indexOf("柔性") !== -1) return true;
                            if (displayType === "rigid" && item.name && item.name.indexOf("刚性") !== -1) return true;
                            if (displayType === "wire" && item.name && item.name.indexOf("吊弦") !== -1) return true;
                            return false;
                        }

                        delegate: Rectangle {
                            id: previewItem
                            width: GridView.view.cellWidth - 10
                            height: GridView.view.cellHeight - 10
                            color: "#ffffff"
                            border.color: model.status === "green" ? "#0066cc" : "red"
                            border.width: 1
                            visible: previewGrid.filterModel(model)

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    currentSelectedImage = {
                                        name: model.name,
                                        serial: model.serial,
                                        count: model.count,
                                        status: model.status,
                                        imageSource: model.imageSource
                                    }
                                    imagePopup.open()
                                }
                            }

                            // 图片预览区域
                            Image {
                                id: previewImage
                                anchors.top: parent.top
                                anchors.left: parent.left
                                anchors.right: parent.right
                                height: width
                                fillMode: Image.PreserveAspectFit
                                cache: true
                                asynchronous: true
                                source: model.imageSource || ""

                                Rectangle {
                                    anchors.fill: parent
                                    color: "#f0f0f0"
                                    visible: parent.status !== Image.Ready && parent.status !== Image.Loading

                                    Text {
                                        anchors.centerIn: parent
                                        text: "图片预览\n" + model.name
                                        color: "#a0a0a0"
                                        horizontalAlignment: Text.AlignHCenter
                                    }
                                }

                                Rectangle {
                                    anchors.fill: parent
                                    color: "#f0f0f0"
                                    visible: parent.status !== Image.Ready && parent.status == Image.Loading

                                    Text {
                                        anchors.centerIn: parent
                                        text: "加载图片中...\n" + model.name
                                        color: "#a0a0a0"
                                        horizontalAlignment: Text.AlignHCenter
                                    }
                                }
                            }

                            // 信息区域
                            Column {
                                anchors.left: parent.left
                                anchors.bottom: parent.bottom
                                anchors.margins: 5
                                spacing: 2

                                Text { text: "相机序列号"; font.pixelSize: 10 }
                                Text { text: "构件编号"; font.pixelSize: 10 }
                            }
                            Column {
                                anchors.right: parent.right
                                anchors.bottom: parent.bottom
                                anchors.margins: 5
                                spacing: 2

                                Text { text: model.serial; font.pixelSize: 10; horizontalAlignment: Text.AlignRight }
                                Text { text: model.count; font.pixelSize: 10; horizontalAlignment: Text.AlignRight }
                            }
                        }
                    }
                }
            }
        }
    }
    
    // 添加弹出式大图预览
    Popup {
        id: imagePopup
        width: parent.width * 0.8
        height: parent.height * 0.8
        anchors.centerIn: parent
        modal: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        
        background: Rectangle {
            color: "#f5f5f5"
            border.color: currentSelectedImage.status === "green" ? "#0066cc" : "red"
            border.width: 2
            radius: 5
            
            // 添加关闭按钮
            Button {
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.margins: 10
                text: "×"
                width: 30
                height: 30
                
                onClicked: imagePopup.close()
                
                background: Rectangle {
                    color: parent.hovered ? "#e0e0e0" : "transparent"
                    radius: width / 2
                }
            }
        }
        
        contentItem: Item {
            // 站区和杆号信息
            Text {
                id: popupLocationInfo
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.margins: 10
                text: "站区: " + currentStation + "  杆号: " + currentPole
                font.pixelSize: 14
            }
            
            // 包含缩放功能的图像查看区域
            Flickable {
                id: imageFlickable
                anchors.top: popupLocationInfo.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: infoRow.top
                anchors.margins: 10
                contentWidth: imageContainer.width * imageScale.xScale
                contentHeight: imageContainer.height * imageScale.yScale
                clip: true
                
                Item {
                    id: imageContainer
                    width: imageFlickable.width
                    height: imageFlickable.height
                    
                    Image {
                        id: popupImage
                        anchors.centerIn: parent
                        width: Math.min(parent.width, parent.height) * 0.9
                        height: width
                        fillMode: Image.PreserveAspectFit
                        source: currentSelectedImage.imageSource || ""
                        
                        transform: Scale {
                            id: imageScale
                            xScale: 1.0
                            yScale: 1.0
                            origin.x: width / 2
                            origin.y: height / 2
                        }
                        
                        Rectangle {
                            anchors.fill: parent
                            color: "#f0f0f0"
                            visible: parent.status !== Image.Ready && parent.status !== Image.Loading
                            
                            Text {
                                anchors.centerIn: parent
                                text: currentSelectedImage.name ? 
                                    "图片预览\n" + currentSelectedImage.name :
                                    "图片预览区域"
                                font.pixelSize: 18
                                color: "#808080"
                                horizontalAlignment: Text.AlignHCenter
                            }
                        }
                    }
                }
                
                MouseArea {
                    anchors.fill: parent
                    onWheel: {
                        // 实现鼠标滚轮缩放
                        if (wheel.angleDelta.y > 0) {
                            // 放大
                            if (imageScale.xScale < 3.0) {
                                imageScale.xScale *= 1.1;
                                imageScale.yScale *= 1.1;
                            }
                        } else {
                            // 缩小
                            if (imageScale.xScale > 0.5) {
                                imageScale.xScale /= 1.1;
                                imageScale.yScale /= 1.1;
                            }
                        }
                    }
                }
            }
            
            // 信息区域
            RowLayout {
                id: infoRow
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 15
                height: 60
                
                Column {
                    spacing: 5
                    
                    Text { 
                        text: "相机名称: " + currentSelectedImage.name
                        font.pixelSize: 14
                    }
                    
                    Text { 
                        text: "相机序列号: " + currentSelectedImage.serial
                        font.pixelSize: 14
                    }
                    
                    Text { 
                        text: "构件编号: " + currentSelectedImage.count
                        font.pixelSize: 14
                    }
                }
                
                Item { Layout.fillWidth: true }
                
                // 添加缩放控制按钮
                RowLayout {
                    spacing: 10
                    
                    Button {
                        text: "+"
                        width: 40
                        height: 40
                        onClicked: {
                            if (imageScale.xScale < 3.0) {
                                imageScale.xScale *= 1.2;
                                imageScale.yScale *= 1.2;
                            }
                        }
                    }
                    
                    Button {
                        text: "-"
                        width: 40
                        height: 40
                        onClicked: {
                            if (imageScale.xScale > 0.5) {
                                imageScale.xScale /= 1.2;
                                imageScale.yScale /= 1.2;
                            }
                        }
                    }
                    
                    Button {
                        text: "重置"
                        width: 60
                        height: 40
                        onClicked: {
                            imageScale.xScale = 1.0;
                            imageScale.yScale = 1.0;
                        }
                    }
                }
            }
        }
    }
    
    // Status bar
    Rectangle {
        id: statusBar
        width: parent.width
        height: 30
        anchors.bottom: parent.bottom
        color: "#f0f0f0"
        
        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            
            Text {
                text: "程序状态: "
                font.pixelSize: 12
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }
            
            Rectangle {
                width: 12
                height: 12
                radius: 6
                color: "green"
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }
            
            Text {
                text: " 正常"
                font.pixelSize: 12
                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
            }
            
            Item { Layout.fillWidth: true }
            
            Text {
                text: "任务名称: " + currentTaskName
                font.pixelSize: 12
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            }
            
            Item { Layout.fillWidth: true }
            
            Text {
                text: "存储路径: 【E://弓网科技/检测结果】"
                font.pixelSize: 12
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            }

            Item { Layout.fillWidth: true }

            Button {
                text: "手动触发"
                width: 80
                height: 24
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                
                // 添加鼠标悬停效果
                background: Rectangle {
                    color: parent.hovered ? "#0077ff" : "#0066cc"
                    radius: 4
                }
                
                contentItem: Text {
                    text: parent.text
                    color: "white"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 12
                }
                
                onClicked: {
                    console.log("巡检拍照手动触发按钮被点击")
                    qmlCommunication.behaviorFromQml(QmlCommunication.QmlActions.SimulateTrigger,"1")

                    // 主动+1当前选中相机的触发次数
                    for (let i = 0; i < cameraListModel.count; i++) {
                        let item = cameraListModel.get(i)
                        if (item.name === currentSelectedImage.name) {
                            let newCount = (parseInt(item.count) + 1).toString()
                            cameraListModel.setProperty(i, "count", newCount)
                            // 同步更新右侧大图信息
                            currentSelectedImage.count = newCount
                            break
                        }
                    }
                }
            }
        }
    }

    Connections {
        target: qmlCommunication
        
        // 相机状态信号处理
        function onCamera_status_recvFromCpp(data) {
            //console.log("收到相机状态:", data)

            cameraListModel.clear()

            if (typeof data !== "string") {
                console.error("接收到的相机状态数据格式无效")
                return
            }

            // 匹配每个 [ ... ] 块
            var matches = data.match(/\[([^\]]+)\]/g)
            if (!matches) {
                console.error("未找到有效的相机数据")
                return
            }

            matches.forEach(function(item) {
                // 去掉前后的中括号
                var content = item.slice(1, -1)
                var obj = {}
                // 拆分 key:value
                content.split(",").forEach(function(pair) {
                    var kv = pair.split(":")
                    if (kv.length === 2) {
                        var key = kv[0].trim()
                        var value = kv[1].trim()
                        obj[key] = value
                    }
                })
                // 状态0为green，其他为red
                cameraListModel.append({
                    status: obj.status === "0" ? "green" : "red",
                    name: obj.name || "",
                    serial: obj.serial || "",
                    count: obj.count || "0",
                    imageSource: ""  // 初始化时没有图像
                })
            })
        }
        
        // 任务名称信号处理
        function onTaskname_recvFromCpp(data) {
            console.log("收到任务名:", data)
            currentTaskName = data
        }
        
        // 定位数据信号处理
        function onEgvlocation_pageFromCpp(data) {
            //console.log("收到定位数据", JSON.stringify(data))
            currentStation = data.stationName || ""
            currentPole = data.poleName || ""
        }
        
        // 巡检拍照数据信号处理
        function onEgvinspectionimage_pageFromCpp(data) {
            //console.log("收到巡检拍照数据", JSON.stringify(data))
            
            if (!data || !data.picturedata || !data.camid) {
                console.error("无效的图像数据")
                return
            }

            // 更新cameraListModel中的图像
            for (let i = 0; i < cameraListModel.count; i++) {
                let item = cameraListModel.get(i)
                if (item.name === data.camid) {
                    // 更新触发次数和图像源
                    let newCount = data.tcount || (parseInt(item.count) + 1).toString()
                    
                    // 更新cameraListModel中的数据
                    cameraListModel.setProperty(i, "count", newCount)
                    cameraListModel.setProperty(i, "imageSource", data.picturedata)
                    
                    break
                }
            }
            
            // 如果当前选中的是这个相机，也更新弹出窗口中的图像
            if (currentSelectedImage.name === data.camid) {
                currentSelectedImage = {
                    name: data.camid,
                    serial: currentSelectedImage.serial,
                    count: data.tcount || currentSelectedImage.count,
                    status: currentSelectedImage.status,
                    imageSource: data.picturedata
                }
                console.log("已更新当前选中图像")
            }
        }
    }
}