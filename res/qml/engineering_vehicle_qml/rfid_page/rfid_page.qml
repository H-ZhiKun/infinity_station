import QtQuick 
import QtQuick.Controls 
import QtQuick.Layouts 
import InfinityStation 1.0

Window {
    width: 800
    height: 600
    visible: true
    title: "RFID数据查看器"
    color: "#f8f8f8"
    property var rfidCountMap: ({})

    RowLayout {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 15

        // 左侧表格区域 (80% 宽度)
        Rectangle {
            id: tablePanel
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.preferredWidth: parent.width * 0.8
            color: "#ffffff"
            border.color: "#dddddd"
            radius: 4
            
            // 面板标题
            Rectangle {
                width: parent.width
                height: 30
                color: "#2980b9"
                radius: 4
                
                Text {
                    anchors.centerIn: parent
                    text: "RFID数据"
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
                height: 30
                color: "#e8e8e8"
                
                Row {
                    anchors.fill: parent
                    spacing: 1
                    
                    // 标签ID表头
                    Rectangle {
                        width: parent.width * 0.7
                        height: parent.height
                        color: "transparent"
                        
                        Text {
                            anchors.centerIn: parent
                            text: "标签ID"
                            color: "#444444"
                            font.pixelSize: 12
                            font.bold: true
                        }
                    }
                    
                    // 重复次数表头
                    Rectangle {
                        width: parent.width * 0.3
                        height: parent.height
                        color: "transparent"
                        
                        Text {
                            anchors.centerIn: parent
                            text: "重复次数"
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
                anchors.bottomMargin: 10
                clip: true
                boundsBehavior: Flickable.StopAtBounds

                // 添加滚动条
                ScrollBar.vertical: ScrollBar {
                    active: true
                    policy: ScrollBar.AsNeeded
                    width: 8
                }
                
                model: ListModel {
                    id: rfidDataModel
                    
                    // 示例数据
                    Component.onCompleted: {

                    }
                }
                
                delegate: Rectangle {
                    width: dataListView.width - 10
                    height: 30
                    color: index % 2 == 0 ? "#ffffff" : "#f5f5f5"
                    radius: 2
                    
                  Row {
                        anchors.fill: parent
                        spacing: 1
                        Rectangle {
                            width: parent.width * 0.7
                            height: parent.height
                            color: "transparent"
                            Text {
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.left: parent.left
                                anchors.leftMargin: 10
                                text: rfid
                                font.pixelSize: 12
                                color: "#333333"
                                elide: Text.ElideRight
                            }
                        }
                        Rectangle {
                            width: parent.width * 0.3
                            height: parent.height
                            color: "transparent"
                            Text {
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: count
                                font.pixelSize: 12
                                font.bold: count > 10
                                color: count > 10 ? "#e74c3c" : "#333333"
                            }
                        }
                    }
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            dataListView.currentIndex = index
                            console.log("选中标签:", rfid, "次数:", count)
                        }
                    }
                }
                
                // 高亮当前选中项
                highlight: Rectangle {
                    color: "#d6eaf8"
                    radius: 2
                }
                highlightFollowsCurrentItem: true
                focus: true
            }
        }

        // 右侧操作区域 (20% 宽度)
        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: parent.width * 0.2
            color: "#f5f5f5"
            border.color: "#dddddd"
            radius: 4
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 15
                
                // 操作按钮
                Button {
                    id: testButton
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    text: "测试"
                    font.pixelSize: 14
                    font.bold: true
                    
                    background: Rectangle {
                        color: testButton.down ? "#2980b9" : "#3498db"
                        radius: 4
                    }
                    
                    contentItem: Text {
                        text: testButton.text
                        font: testButton.font
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    
                    onClicked: {
                        console.log("测试按钮被点击")
                        rfidDataModel.append({
                            "rfid": "E2000000000000000" ,
                            "count": 1
                        })
                        qmlCommunication.behaviorFromQml(QmlCommunication.QmlActions.RfidSimulate, "E2000000000000000")

                    }
                }
                
                // 清空按钮
                Button {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    text: "清空数据"
                    font.pixelSize: 14
                    
                    onClicked: {
                        rfidDataModel.clear()
                        console.log("数据已清空")
                    }
                }
                
                // 导出按钮
                Button {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    text: "导出数据"
                    font.pixelSize: 14
                    
                    onClicked: console.log("导出数据功能")
                }
                
                // 添加空白区域使按钮居中
                Item {
                    Layout.fillHeight: true
                }
            }
        }
    }
    Connections {
        target: qmlCommunication
        function onrfid_recvFromCpp(data) {
            console.log("接收到RFID数据:", JSON.stringify(data));
            if (data && data.rfid) {
                // 计数自增
                rfidCountMap[data.rfid] = (rfidCountMap[data.rfid] || 0) + 1;

                // 检查模型中是否已存在该rfid
                var found = false;
                for (var i = 0; i < rfidDataModel.count; ++i) {
                    if (rfidDataModel.get(i).rfid === data.rfid) {
                        rfidDataModel.setProperty(i, "count", rfidCountMap[data.rfid]);
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    rfidDataModel.append({
                        "rfid": data.rfid,
                        "count": rfidCountMap[data.rfid]
                    });
                }
            } else {
                console.warn("接收到无效的RFID数据:", data);
            }
        }

    }

}