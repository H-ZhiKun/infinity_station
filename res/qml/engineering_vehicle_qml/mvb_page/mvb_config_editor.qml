import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Dialogs
import InfinityStation 1.0

Window {
    id: root
    width: 800
    height: 700
    title: "MVB配置修改工具"
    visible: true

    // 定义应用程序中使用的颜色
    QtObject {
        id: appColors
        property color mainBackground: "#f5f5f5"
        property color headerBackground: "#34495e"
        property color headerText: "#ffffff"
    }

     MessageDialog {
          id: saveResultDialog
          title: "提示"
          text: ""
          visible: false
          onAccepted: visible = false
     }

    // 主布局
    Rectangle {
        anchors.fill: parent
        color: appColors.mainBackground

        // 顶部标题栏
        Rectangle {
            id: header
            width: parent.width
            height: 60
            color: appColors.headerBackground

            Label {
                anchors.centerIn: parent
                text: "MVB 配置修改工具"
                font.pixelSize: 18
                font.bold: true
                color: appColors.headerText
            }

            Label {
                anchors.right: parent.right
                anchors.rightMargin: 20
                anchors.verticalCenter: parent.verticalCenter
                text: "版本 1.0.0"
                font.pixelSize: 12
                color: Qt.lighter(appColors.headerText, 0.8)
            }
        }

        // 内容区域
        ScrollView {
            anchors.top: header.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 20
            clip: true
               Connections {
                    target: qmlCommunication
                    function onconfig_recvFromCpp(data) {
                         var jsonData = data[0]

                         // 更新基础配置
                         for (var i = 0; i < mvbTypeCombo.count; i++) {
                              if (mvbTypeCombo.model[i] === jsonData.mvb_type) {
                                   mvbTypeCombo.currentIndex = i
                                   break
                              }
                         }
                         comidField.text = jsonData.comid
                         softVersionField.text = jsonData.softversion
                         pmsVersionField.text = jsonData.pmsversion
                         msgTypeField.text = jsonData.msgtype
                         stationFilePathField.text = jsonData.station_file_path

                         // 更新读配置
                         readConfigList.model.clear()
                         for (var i = 0; i < jsonData.read_config.length; i++) {
                              readConfigList.model.append({
                                   readPort: jsonData.read_config[i].port,
                                   readSize: jsonData.read_config[i].size,
                                   readInterval: jsonData.read_config[i].interval
                              })
                         }

                         // 更新写配置
                         writeConfigList.model.clear()
                         for (var j = 0; j < jsonData.write_config.length; j++) {
                              writeConfigList.model.append({
                                   writePort: jsonData.write_config[j].port,
                                   writeSize: jsonData.write_config[j].size,
                                   writeInterval: jsonData.write_config[j].interval
                              })
                         }
                    }

                    function onedit_stateFromCpp(state){
                         console.log("onedit_stateFromCpp: " + state)
                         if(state ===true ){
                              saveResultDialog.text = "保存成功！"
                         }else{
                              saveResultDialog.text = "保存失败！"
                         }
                         saveResultDialog.visible = true
                    }
               }   
            ColumnLayout {
                width: parent.width
                spacing: 20

                Button {
                    text: "获取默认配置"

                    onClicked: {
                        // 发送信号给后端，刷新配置
                        if (typeof qmlCommunication !== "undefined") {
                            qmlCommunication.behaviorFromQml(QmlCommunication.QmlActions.UpdateConfigButton, {
                                "fullpath": "config/mvb_card/mvbfull.yaml",
                                "path": "config/mvb_card/mvb.yaml"
                            });
                        }

                        
                    }
                }

                // 基础配置部分
                GroupBox {
                    title: "基础配置"
                    Layout.fillWidth: true

                    GridLayout {
                        columns: 2
                        rowSpacing: 10
                        columnSpacing: 15
                        width: parent.width

                        Label { text: "MVB板卡类型:" }
                        ComboBox {
                            id: mvbTypeCombo
                            model: ["duagon"]
                            currentIndex: -1 // 去掉默认值
                            Layout.minimumWidth: 250
                            Layout.fillWidth: true
                        }

                        Label { text: "COMID:" }
                        TextField {
                            id: comidField
                            placeholderText: "请输入COMID"
                            validator: IntValidator {bottom: 0; top: 65535}
                            Layout.minimumWidth: 250
                            Layout.fillWidth: true
                        }

                        Label { text: "软件版本:" }
                        TextField {
                            id: softVersionField
                            placeholderText: "请输入软件版本"
                            validator: RegularExpressionValidator { regularExpression: /^0x[0-9A-Fa-f]{4}$/ }
                            Layout.minimumWidth: 250
                            Layout.fillWidth: true
                        }

                        Label { text: "PMS版本:" }
                        TextField {
                            id: pmsVersionField
                            placeholderText: "请输入PMS版本"
                            validator: RegularExpressionValidator { regularExpression: /^0x[0-9A-Fa-f]{4}$/ }
                            Layout.minimumWidth: 250
                            Layout.fillWidth: true
                        }

                        Label { text: "消息类型:" }
                        TextField {
                            id: msgTypeField
                            placeholderText: "请输入消息类型"
                            validator: RegularExpressionValidator { regularExpression: /^0x[0-9A-Fa-f]{4}$/ }
                            Layout.minimumWidth: 250
                            Layout.fillWidth: true
                        }

                        Label { text: "站点文件路径:" }
                        TextField {
                            id: stationFilePathField
                            placeholderText: "请输入站点文件路径"
                            Layout.minimumWidth: 250
                            Layout.fillWidth: true
                        }
                    }
                }

                // 读配置部分
                GroupBox {
                    title: "读配置 (read_config)"
                    Layout.fillWidth: true

                    ColumnLayout {
                        width: parent.width
                        spacing: 10

                        // 表头
                        GridLayout {
                            Layout.fillWidth: true
                            columns: 4
                            rowSpacing: 10
                            columnSpacing: 15

                            Label { 
                                text: "端口号" 
                                font.bold: true
                                Layout.preferredWidth: 120
                            }
                            Label { 
                                text: "读长度" 
                                font.bold: true
                                Layout.preferredWidth: 120
                            }
                            Label { 
                                text: "时间间隔(ms)" 
                                font.bold: true
                                Layout.preferredWidth: 120
                            }
                            Label { 
                                text: "操作" 
                                font.bold: true
                                Layout.preferredWidth: 100
                            }
                        }

                        ListView {
                            id: readConfigList
                            Layout.fillWidth: true
                            Layout.preferredHeight: contentHeight
                            interactive: false
                            model: ListModel {}

                            delegate: GridLayout {
                                width: readConfigList.width
                                columns: 4
                                rowSpacing: 10
                                columnSpacing: 15

                              TextField {
                              id: readPortField
                              text: readPort
                              placeholderText: "端口号"
                              Layout.preferredWidth: 120
                              onTextChanged: readConfigList.model.setProperty(index, "readPort", text)
                              }
                              TextField {
                              id: readSizeField
                              text: readSize
                              placeholderText: "读长度"
                              validator: IntValidator {bottom: 1; top: 1024}
                              Layout.preferredWidth: 120
                              onTextChanged: readConfigList.model.setProperty(index, "readSize", text)
                              }
                              TextField {
                              id: readIntervalField
                              text: readInterval
                              placeholderText: "时间间隔(ms)"
                              validator: IntValidator {bottom: 1}
                              Layout.preferredWidth: 120
                              onTextChanged: readConfigList.model.setProperty(index, "readInterval", text)
                              }
                                Button {
                                    text: "删除"
                                    onClicked: readConfigList.model.remove(index)
                                }
                            }
                        }

                        Button {
                            text: "添加读配置"
                            onClicked: readConfigList.model.append({
                                readPort: "",
                                readSize: "",
                                readInterval: ""
                            })
                        }
                    }
                }

                // 写配置部分
                GroupBox {
                    title: "写配置 (write_config)"
                    Layout.fillWidth: true

                    ColumnLayout {
                        width: parent.width
                        spacing: 10

                        // 表头
                        GridLayout {
                            Layout.fillWidth: true
                            columns: 4
                            rowSpacing: 10
                            columnSpacing: 15

                            Label { 
                                text: "端口号" 
                                font.bold: true
                                Layout.preferredWidth: 120
                            }
                            Label { 
                                text: "写长度" 
                                font.bold: true
                                Layout.preferredWidth: 120
                            }
                            Label { 
                                text: "时间间隔(ms)" 
                                font.bold: true
                                Layout.preferredWidth: 120
                            }
                            Label { 
                                text: "操作" 
                                font.bold: true
                                Layout.preferredWidth: 100
                            }
                        }

                        ListView {
                            id: writeConfigList
                            Layout.fillWidth: true
                            Layout.preferredHeight: contentHeight
                            interactive: false
                            model: ListModel {}

                            delegate: GridLayout {
                                width: writeConfigList.width
                                columns: 4
                                rowSpacing: 10
                                columnSpacing: 15

                              TextField {
                              id: writePortField
                              text: writePort
                              placeholderText: "端口号"
                              Layout.preferredWidth: 120
                              onTextChanged: writeConfigList.model.setProperty(index, "writePort", text)
                              }
                              TextField {
                              id: writeSizeField
                              text: writeSize
                              placeholderText: "写长度"
                              validator: IntValidator {bottom: 1; top: 1024}
                              Layout.preferredWidth: 120
                              onTextChanged: writeConfigList.model.setProperty(index, "writeSize", text)
                              }
                              TextField {
                              id: writeIntervalField
                              text: writeInterval
                              placeholderText: "时间间隔(ms)"
                              validator: IntValidator {bottom: 1}
                              Layout.preferredWidth: 120
                              onTextChanged: writeConfigList.model.setProperty(index, "writeInterval", text)
                              }
                                Button {
                                    text: "删除"
                                    onClicked: writeConfigList.model.remove(index)
                                }
                            }
                        }

                        Button {
                            text: "添加写配置"
                            onClicked: writeConfigList.model.append({
                                writePort: "",
                                writeSize: "",
                                writeInterval: ""
                            })
                        }
                    }
                }

                // 保存按钮
                RowLayout {
                    Layout.fillWidth: true
                    Layout.topMargin: 20
                    Layout.bottomMargin: 20
                    
                    Item { Layout.fillWidth: true }
                    
                    Button {
                        text: "重置"
                        onClicked: {
                            // 重置操作
                            if (typeof qmlCommunication !== "undefined") {
                                qmlCommunication.behaviorFromQml(QmlCommunication.QmlActions.UpdateConfigButton, {
                                    "fullpath": "config/mvb_card/mvbfull.yaml",
                                    "path": "config/mvb_card/mvb.yaml"
                                });
                            }
                        }
                    }
                    
                    Button {
                        text: "保存配置"
                        highlighted: true
                        onClicked: {
                            // 构建配置对象
                            var config = {
                                mvb_type: mvbTypeCombo.currentText,
                                comid: comidField.text ? parseInt(comidField.text) : null,
                                softversion: softVersionField.text,
                                pmsversion: pmsVersionField.text,
                                msgtype: msgTypeField.text,
                                station_file_path: stationFilePathField.text,
                                read_config: [],
                                write_config: []
                            }

                              // 收集读配置
                              for (var i = 0; i < readConfigList.model.count; i++) {
                                   var readItem = readConfigList.model.get(i)
                                   config.read_config.push({
                                        port: readItem.readPort,
                                        size: readItem.readSize ? parseInt(readItem.readSize) : null,
                                        interval: readItem.readInterval ? parseInt(readItem.readInterval) : null
                                   })
                              }

                              // 收集写配置
                              for (var j = 0; j < writeConfigList.model.count; j++) {
                                   var writeItem = writeConfigList.model.get(j)
                                   config.write_config.push({
                                        port: writeItem.writePort,
                                        size: writeItem.writeSize ? parseInt(writeItem.writeSize) : null,
                                        interval: writeItem.writeInterval ? parseInt(writeItem.writeInterval) : null
                                   })
                              }

                              //console.log("保存配置", JSON.stringify(config))
                            // 发送保存信号
                            if (typeof qmlCommunication !== "undefined") {
                                qmlCommunication.behaviorFromQml(QmlCommunication.QmlActions.ConfigDataSend, {
                                    "path": "config/mvb_card/mvb.yaml",
                                    "data": config
                                })
                            }
                        }
                    }
                }
            }
        }
    }


}