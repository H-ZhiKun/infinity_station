import QtQuick 
import QtQuick.Controls 
import QtQuick.Layouts 
import QtQuick.Window 
import InfinityStation 1.0

Window {
    id: mvbWindow
    width: 1500
    height: 1000
    visible: true  // 添加此行确保窗口可见
    minimumWidth: 1500
    minimumHeight: 1000
    maximumWidth: 1500
    maximumHeight: 1000
    color: "#f0f0f0"
   

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        // 顶部控制区域
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            color: "#ffffff"
            border.color: "#ffffff"

            RowLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 15

                Label {
                    text: qsTr("MVB 监控")
                    font.pixelSize: 16
                    font.bold: true
                    color: "#2c3e50"
                }

                Item { Layout.fillWidth: true }

                Button {
                    id: connectButton
                    text: qsTr("连接")
                    implicitWidth: 80
                    implicitHeight: 32

                    background: Rectangle {
                        color: parent.down ? "#1976D2" :
                               parent.hovered ? "#2196F3" : "#2196F3"
                        radius: 4
                    }

                    contentItem: Text {
                        text: parent.text
                        color: "#ffffff"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                Button {
                    id: disconnectButton
                    text: qsTr("断开连接")
                    implicitWidth: 80
                    implicitHeight: 32

                    background: Rectangle {
                        color: parent.down ? "#c62828" :
                               parent.hovered ? "#f44336" : "#f44336"
                        radius: 4
                    }

                    contentItem: Text {
                        text: parent.text
                        color: "#ffffff"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }

        // 数据显示区域
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#ffffff"
            border.color: "#ffffff"
            radius: 4

            // 使用 RowLayout 来创建三列布局
            RowLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 10

                // 左侧数据显示
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "#ffffff"
                    border.color: "#e0e0e0"
                    border.width: 1
                    radius: 4

                    ScrollView {
                        anchors.fill: parent
                        anchors.margins: 10
                        clip: true

                        ListView {
                            id: leftListView
                            width: parent.width
                            model: ListModel {
                                id: mvbModelLeft
                            }

                            header: Rectangle {
                                width: parent.width
                                height: 40
                                color: "#ffffff"

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: 5
                                    spacing: 10

                                    Text {
                                        Layout.preferredWidth: parent.width * 0.5
                                        text: "VCU发送的公共数据"
                                        font.bold: true
                                    }
                                    Text {
                                        Layout.preferredWidth: parent.width * 0.5
                                        text: "数值"
                                        font.bold: true
                                    }
                                }
                            }

                            delegate: Rectangle {
                                width: leftListView.width
                                height: 36
                                color: index % 2 ? "#ffffff" : "#f8f8f8"
                                border.color: "#ffffff"

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: 5
                                    spacing: 10

                                    Text {
                                        Layout.preferredWidth: parent.width * 0.5
                                        text: name
                                        font.pixelSize: 14
                                    }
                                    Text {
                                        Layout.preferredWidth: parent.width * 0.5
                                        text: value
                                        font.pixelSize: 14
                                    }
                                }
                            }
                        }
                    }
                }

                // 中间数据显示
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "#ffffff"
                    border.color: "#e0e0e0"
                    border.width: 1
                    radius: 4

                    ScrollView {
                        anchors.fill: parent
                        anchors.margins: 10
                        clip: true

                        ListView {
                            id:midListView
                            width: parent.width
                            model: ListModel {
                                id: mvbModelMid
                            }

                            header: Rectangle {
                                width: parent.width
                                height: 40
                                color: "#ffffff"

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: 5
                                    spacing: 10

                                    Text {
                                        Layout.preferredWidth: parent.width * 0.5
                                        text: "VCU发送的车辆数据"
                                        font.bold: true
                                    }
                                    Text {
                                        Layout.preferredWidth: parent.width * 0.5
                                        text: "数值"
                                        font.bold: true
                                    }
                                }
                            }

                            delegate: Rectangle {
                                width: midListView.width
                                height: 36
                                color: index % 2 ? "#ffffff" : "#f8f8f8"
                                border.color: "#ffffff"

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: 5
                                    spacing: 10

                                    Text {
                                        Layout.preferredWidth: parent.width * 0.5
                                        text: name
                                        font.pixelSize: 14
                                    }
                                    Text {
                                        Layout.preferredWidth: parent.width * 0.5
                                        text: value
                                        font.pixelSize: 14
                                    }
                                }
                            }
                        }
                    }
                }

                // 右侧数据显示
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "#ffffff"
                    border.color: "#e0e0e0"
                    border.width: 1
                    radius: 4

                    ScrollView {
                        anchors.fill: parent
                        anchors.margins: 10
                        clip: true

                        ListView {
                            id:rightListView
                            width: parent.width
                            
                            model: ListModel {
                                id: mvbModelRight
                            }

                            header: Rectangle {
                                width: parent.width
                                height: 40
                                color: "#ffffff"

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: 5
                                    spacing: 10

                                    Text {
                                        Layout.preferredWidth: parent.width * 0.5
                                        text: "弓网发送的心跳数据"
                                        font.bold: true
                                    }
                                    Text {
                                        Layout.preferredWidth: parent.width * 0.5
                                        text: "数值"
                                        font.bold: true
                                    }
                                }
                            }

                            delegate: Rectangle {
                                width: rightListView.width
                                height: 36
                                color: index % 2 ? "#ffffff" : "#f8f8f8"
                                border.color: "#ffffff"

                                RowLayout {
                                    anchors.fill: parent
                                    anchors.margins: 5
                                    spacing: 10

                                    Text {
                                        Layout.preferredWidth: parent.width * 0.5
                                        text: name
                                        font.pixelSize: 14
                                    }
                                    Text {
                                        Layout.preferredWidth: parent.width * 0.5
                                        text: value
                                        font.pixelSize: 14
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        // 底部信息区域
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 200
            color: "#ffffff"
            border.color: "#ffffff"
            radius: 4

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 10

                // 刷新时间控制组
                GridLayout {
                    Layout.fillWidth: true
                    columns: 4
                    columnSpacing: 20
                    rowSpacing: 10

                    Repeater {
                        model: [
                            { label: "HvRefreshTime", value: "32" },
                            { label: "LvFreshTime", value: "128" },
                            { label: "MpuFreshTime", value: "128" },
                            { label: "PmsFreshTime", value: "256" }
                        ]

                        Rectangle {
                            Layout.preferredWidth: 150
                            Layout.preferredHeight: 70
                            color: "#ffffff"
                            radius: 4

                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 5
                                spacing: 5

                                Text {
                                    text: modelData.label
                                    font.pixelSize: 14
                                    Layout.alignment: Qt.AlignHCenter
                                }
                                TextField {
                                    Layout.preferredWidth: 100
                                    Layout.alignment: Qt.AlignHCenter
                                    text: modelData.value
                                    font.pixelSize: 14
                                }
                            }
                        }
                    }
                }

                TextArea {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 70
                    readOnly: true
                    text: "程序开始运动，版本1.0.0.20230912\nGrpcPath: C:/path/to/service\nServer listening on 0.0.0.0:21300"
                    wrapMode: TextArea.Wrap
                    font.pixelSize: 14
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 20

                    Text { 
                        text: "程序版本:1.0.0.230912"
                        font.pixelSize: 14 
                    }
                    Item { Layout.fillWidth: true }
                    Text { 
                        text: "处理时间:2025 - 01 - 15 10:29:17"
                        font.pixelSize: 14 
                    }
                    Item { Layout.fillWidth: true }
                    Text { 
                        text: "检测状态:X"
                        font.pixelSize: 14 
                    }
                }
            }
        }
    }

    property var fieldMappings: ({
        // 重庆27
        "chongqing27_heart": [
            { key: "recvTime",display: "心跳数据发送时间"},
            { key: "tcmsAlive",display: "TCMS生命信号" },
            { key: "version",display: "软件版本" },
            { key: "testRlt",display: "自检结果"},
            { key: "tcmsLife",display: "TCMS生命信号异常" },
            { key: "pmsStatus",display: "设备状态"},
            { key: "panFault",display: "受电弓严重故障"},
        ],
        "chongqing27_common_data": [
            { key: "recvTime",display: "公共数据接收时间"},
            { key: "tcmsAlive",display: "CCU生命信号" },
            { key: "cab1Occupied",display: "1车司机室激活" },
            { key: "cab6Occupied",display: "6车司机室激活"},
            { key: "doorCmdOpnA",display: "列车开A侧门指令" },
            { key: "doorCmdOpnB",display: "列车开B侧门指令"},
            { key: "doorCmdClsA",display: "列车关A侧门指令"},
            { key: "doorCmdClsB",display: "列车关B侧门指令"},
            { key: "trainUpLine",display: "列车上行"},
            { key: "trainDownLine",display: "列车下行"},
            { key: "timeSetValid",display: "TCMS时间有效"},
            { key: "trainSpeedVld",display: "列车参考速度有效"},
            { key: "codeVld",display: "报站数据有效"},
            { key: "year",display: "年" },
            { key: "month",display: "月"},
            { key: "day",display: "日"},
            { key: "hour",display: "时"},
            { key: "minute",display: "分"},
            { key: "second",display: "秒"},
            { key: "trainSpeed",display: "列车参考速度"},
            { key: "trainNumb",display: "列车号"},
            { key: "startStation",display: "始发站ID"},
            { key: "currentStation",display: "当前站ID"},
            { key: "nextStation",display: "下一站ID"},
            { key: "endStation",display: "终点站ID"},
            { key: "distLv",display: "离站距离"},
            { key: "distAr",display: "目标距离"},
            { key: "rateLoadC1",display: "1车乘车率"},
            { key: "rateLoadC2",display: "2车乘车率"},
            { key: "rateLoadC3",display: "3车乘车率"},
            { key: "rateLoadC4",display: "4车乘车率"},
            { key: "rateLoadC5",display: "5车乘车率"},
            { key: "rateLoadC6",display: "6车乘车率"},
        ],
        "chongqing27_ccu_data": [
            { key: "recvTime",display: "车辆数据接收时间"},
            { key: "tcmsAlive",display: "CCU生命信号" },
            { key: "batteryTrcOK",display: "蓄电池牵引预备" },
            { key: "batteryTrcOn",display: "蓄电池牵引启动"},
            { key: "alarmReset",display: "火灾报警系统复位" },
            { key: "alarmSilence",display: "火灾报警系统消音"},
            { key: "sleepSgnl",display: "ODD休眠信号"},
            { key: "resetSgnl",display: "ODD远程复位信号"},
            { key: "sleep",display: "列车休眠指令"},
            { key: "pan2Up",display: "2车受电弓升起"},
            { key: "pan5Up",display: "5车受电弓升起"},
            { key: "doorOpen",display: "列车开门状态"},
            { key: "doorCls",display: "列车关门状态"},
            { key: "lineNorm",display: "正线" },
            { key: "lineTurn",display: "折返线"},
            { key: "lineInout",display: "出入段线"},
            { key: "trainNum",display: "列车车次号"},
            { key: "vluVlt",display: "网压"},
            { key: "vluCur",display: "网流"},
            { key: "nonspeed",display: "零速信号"},
            { key: "carWashVal",display: "洗车模式"},
            { key: "AMMode",display: "AM模式"},
            { key: "FAMMode",display: "FAM模式"},
            { key: "outTemp",display: "列车外部温度"},
            { key: "trainNumb",display: "列车车次号"},
            { key: "wheel112",display: "1车2轴1架参考轮径值"},
            { key: "wheel212",display: "2车2轴1架参考轮径值"},
            { key: "wheel312",display: "3车2轴1架参考轮径值"},
            { key: "wheel412",display: "4车2轴1架参考轮径值"},
            { key: "wheel512",display: "5车2轴1架参考轮径值"},
            { key: "wheel612",display: "6车2轴1架参考轮径值"},
        ],
        // 重庆环线
        "chongqing_common_data": [
            { key: "recvTime",display: "公共数据接收时间"},
            { key: "lineId",display: "线路号" },
            { key: "carId",display: "列车号" },
            { key: "carIdAdjustReq",display: "列车号调整请求"},
            { key: "carIdVaild",display: "列车号有效" },
            { key: "alive",display: "TCMS生命信号"},
            { key: "year",display: "年" },
            { key: "month",display: "月"},
            { key: "day",display: "日"},
            { key: "hour",display: "时"},
            { key: "minute",display: "分"},
            { key: "second",display: "秒"},
            { key: "timeAdjustReq",display: "日期和时间调整请求"},
            { key: "timeVaild",display: "日期和时间有效"},
        ],
         "chongqing_ccu_data": [
            { key: "recvTime",display: "TCMS数据接收时间"},
            { key: "tcmsAlive",display: "TCMS生命信号" },
            { key: "battrySleep",display: "蓄电池休眠" },
            { key: "battryUndervoltage",display: "蓄电池欠压"},
            { key: "highVoltageOutage",display: "高压断电" },
            { key: "pantoUp",display: "升弓到位"},
            { key: "pantoDown",display: "降弓到位" },
            { key: "mc1Occupy",display: "Mc1车司机室占用"},
            { key: "mc2Occupy",display: "Mc2车司机室占用"},
            { key: "ATBMode",display: "ATB模式"},
            { key: "closeDoor",display: "列车关门"},
            { key: "speed",display: "列车速度"},
            { key: "startStationId",display: "起始站"},
            { key: "endStationId",display: "终点站"},
            { key: "nextStationId",display: "下一站"},
            { key: "currentStationId",display: "当前站"},
            { key: "targetDistance",display: "目标距离"},
            { key: "startDistance",display: "起始距离"},
            { key: "pantoVoltage",display: "受电弓电压"},
            { key: "pantoElectric",display: "受电弓电流"},
        ],
        // 深圳16
        "shenzhen16_heart": [
            { key: "sendTime",display: "心跳数据发送时间"},
            { key: "tcmsAlive",display: "CCU生命信号" },
            { key: "version",display: "软件版本" },
            { key: "selfCheckFailed",display: "自检失败"},
            { key: "selfCheckOver",display: "自检完成" },
            { key: "selfCheckSuccess",display: "自检成功"},
            { key: "selfChecking",display: "自检中"},
            { key: "wtsConnectFailed",display: "WTS车地网络通信故障"},
        ],
        "shenzhen16_common_data": [
            { key: "recvTime",display: "公共数据接收时间"},
            { key: "tcmsAlive",display: "生命信号" },
            { key: "lineNum",display: "列车号" },
            { key: "a1Active",display: "A1车司机室激活"},
            { key: "a2Active",display: "A2车司机室激活" },
            { key: "forward",display: "列车前向状态"},
            { key: "backward",display: "列车后向状态"},
            { key: "carPull",display: "列车牵引工况"},
            { key: "carBrake",display: "列车制动工况"},
            { key: "carCoasting",display: "列车惰行工况"},
            { key: "b1Lowerbow",display: "B1车降弓到位"},
            { key: "b2Lowerbow",display: "B2车降弓到位"},
            { key: "carDormancy",display: "列车休眠指令"},
            { key: "ATBMode",display: "ATB模式"},
            { key: "ATOMode",display: "ATO模式"},
            { key: "timeSet",display: "时间设置标志位"},
            { key: "timeValid",display: "时间有效标志位"},
            { key: "year",display: "年" },
            { key: "month",display: "月"},
            { key: "day",display: "日"},
            { key: "hour",display: "时"},
            { key: "minute",display: "分"},
            { key: "second",display: "秒"},
            { key: "speed",display: "列车参考速度"},
            { key: "allTraction",display: "全列车牵引力"},
            { key: "allAirForce",display: "全列车空气制动力"},
            { key: "allElectricForce",display: "全列车电制动力"},
            { key: "a1Load",display: "1车载荷"},
            { key: "a2Load",display: "2车载荷"},
            { key: "a3Load",display: "3车载荷"},
            { key: "a4Load",display: "4车载荷"},
            { key: "a5Load",display: "5车载荷"},
            { key: "a6Load",display: "6车载荷"},
            { key: "allLoad",display: "全列车载荷"},
            { key: "a1RidingRate",display: "1车乘车率"},
            { key: "a2RidingRate",display: "2车乘车率"},
            { key: "a3RidingRate",display: "3车乘车率"},
            { key: "a4RidingRate",display: "4车乘车率"},
            { key: "a5RidingRate",display: "5车乘车率"},
            { key: "a6RidingRate",display: "6车乘车率"},
            { key: "tc1BlockID",display: "Tc1车BlockID"},
            { key: "tc2BlockID",display: "Tc2车BlockID"},
            { key: "tc1Abcsissa",display: "Tc1车abcsissa"},
            { key: "tc2Abcsissa",display: "Tc2车abcsissa"},
            { key: "batteryVoltage",display: "蓄电池电压"},
            { key: "voltage",display: "网压"},
            { key: "electricity",display: "网流"},
            { key: "startStation",display: "起始站"},
            { key: "currentStation",display: "当前站"},
            { key: "nextStation",display: "下一站"},
            { key: "endStation",display: "终点站"},
            { key: "accumulatedKM",display: "列车总行驶里程"},
            { key: "doorsClosed",display: "所有门关闭状态"},
            { key: "leftDoorsClosed",display: "占有端左侧门关好"},
            { key: "rightDoorsClosed",display: "占有端右侧门关好"},
            { key: "panUP",display: "升弓状态"},
            { key: "panDown",display: "降弓状态"},
            { key: "allClose",display: "所有门关好"},
            { key: "sleepCmd",display: "休眠请求"},
            { key: "blowPower",display: "低功耗指令"},
        ],
        // 贵阳s1
        "guiyangs1_heart": [
            { key: "sendTime",display: "心跳数据发送时间"},
            { key: "tcmsAlive",display: "TCMS生命信号" },
            { key: "year",display: "年" },
            { key: "month",display: "月"},
            { key: "day",display: "日"},
            { key: "hour",display: "时"},
            { key: "minute",display: "分"},
            { key: "second",display: "秒"},
            { key: "catOffGraveALarm",display: "拉出值严重超限"},
            { key: "catOffMiddlingALarm",display: "拉出值中等超限"},
            { key: "catOffSlightALarm",display: "拉出值轻微超限"},
            { key: "catHeiGraveALarm",display: "导线高度严重超限"},
            { key: "catHeiMiddlingALarm",display: "导线高度中等超限"},
            { key: "catHeiSlightALarm",display: "导线高度轻微超限"},
            { key: "tempGraveALarm",display: "弓网接触线温度严重超限"},
            { key: "tempMiddlingALarm",display: "弓网接触线温度中等超限"},
            { key: "tempSlightALarm",display: "弓网接触线温度轻微超限"},
            { key: "arcGraveALarm",display: "弓网燃弧严重超限"},
            { key: "arcMiddlingALarm",display: "弓网燃弧中等超限"},
            { key: "arcSlightALarm",display: "弓网燃弧轻微超限"},
            { key: "strGraveALarm",display: "受电弓结构严重故障"},
            { key: "strMiddlingALarm",display: "受电弓结构中等故障"},
            { key: "strSlightALarm",display: "受电弓结构轻微故障"},
            { key: "catHeiModeFault",display: "拉出值导高测量模块故障"},
            { key: "tmpModFault",display: "接触线温度测量模块故障"},
            { key: "arcModFault",display: "弓网燃弧测量模块故障"},
            { key: "iPanModStatus",display: "受电弓结构故障监测模型故障"},
            { key: "sysNetModStatus",display: "系统通信模块故障"},
            { key: "readyToSleep",display: "准备休眠信号"},
            { key: "bowNetGraveALarm",display: "弓网系统严重故障"},
            { key: "bowNetMiddlingALarm",display: "弓网系统中等故障"},
            { key: "bowNetSlightALarm",display: "弓网系统轻微故障"},
            { key: "abrValGraveALarm",display: "接触线磨耗值严重故障"},
            { key: "abrValMiddlingALarm",display: "接触线磨耗值中等故障"},
            { key: "abrValSlightALarm",display: "接触线磨耗值轻微故障"},
            { key: "abrValOModStatus",display: "接触线磨耗检测模块故障"},
            { key: "panrun",display: "运行模式"},
            { key: "panmnt",display: "维护模式"},
            { key: "version",display: "软件版本"},
        ],
        "guiyangs1_common_data": [
            { key: "recvTime",display: "公共数据接收时间"},
            { key: "tcmsAlive",display: "TCMS生命信号" },
            { key: "year",display: "年" },
            { key: "month",display: "月"},
            { key: "day",display: "日"},
            { key: "hour",display: "时"},
            { key: "minute",display: "分"},
            { key: "second",display: "秒"},
            { key: "realspeed",display: "列车速度"},
            { key: "trainset",display: "列车编号"},
            { key: "timevalid",display: "时间有效标志位"},
            { key: "speedvalid",display: "列车参考速度有效"},
            { key: "cabact_1",display: "1车司机室激活"},
            { key: "cabact_6",display: "6车司机室激活"},
            { key: "ATOmode",display: "ATO模式激活"}
        ],
        "guiyangs1_ccu_data": [
            { key: "recvTime",display: "车辆数据接收时间"},
            { key: "panrun",display: "运行模式" },
            { key: "panmnt",display: "维护模式" },
            { key: "autosign",display: "全自动报站标志位"},
            { key: "panrise2",display: "2车受电弓升弓标志位"},
            { key: "panrise5",display: "5车受电弓升弓标志位"},
            { key: "atbsign",display: "自动折返"},
            { key: "sleepmode",display: "休眠模式"},
            { key: "tardis",display: "目标停车站台距离"},
            { key: "endstationid",display: "终点站ID"},
            { key: "currtstationid",display: "当前站ID"},
            { key: "nextstationid",display: "下一站ID"},
            { key: "wd1",display: "TC1轮径"},
            { key: "wd2",display: "TC2轮径"},
        ],
        // 宁象
        "ningxiang_heart": [
            { key: "sendTime",display: "心跳数据发送时间"},
            { key: "tcmsAlive",display: "CCU生命信号" },
            { key: "version",display: "软件版本" },
            { key: "selfCheckStat",display: "自检结果"},
            { key: "flt",display: "受电弓严重异常"},
        ],
        "ningxiang_common_data": [
            { key: "recvTime",display: "公共数据接收时间"},
            { key: "tcmsAlive",display: "CCU生命信号" },
            { key: "xCab1Occupied",display: "1车司机室激活" },
            { key: "xCab2Occupied",display: "4车司机室激活"},
            { key: "xAMMode",display: "AM模式"},
            { key: "xATPMode",display: "ATP模式"},
            { key: "xRMMode",display: "RM模式"},
            { key: "xNRMMode",display: "NRM模式"},
            { key: "xTrainUpLine",display: "列车上行"},
            { key: "xTrainDownLine",display: "列车下行"},
            { key: "xTimeSetValid",display: "时间设置有效位"},
            { key: "xPanto1UpSt",display: "2车受电弓升起"},
            { key: "xPanto2UpSt",display: "3车受电弓升起"},
            { key: "xALLDoorOpen",display: "列车开门信号"},
            { key: "xALLDoorCLoseLock",display: "列车关门信号"},
            { key: "bYear",display: "年"},
            { key: "bMonth",display: "月"},
            { key: "bDay",display: "日"},
            { key: "bHour",display: "时"},
            { key: "bMinute",display: "分"},
            { key: "bSecond",display: "秒"},
            { key: "usLocation",display: "线路区域"},
            { key: "uiSpd",display: "列车参考速度"},
            { key: "uiStartStation",display: "起始站"},
            { key: "uiCurrentStation",display: "当前站"},
            { key: "uiNextStation",display: "下一站"},
            { key: "uiEndStation",display: "终点站"},
            { key: "uiNextStopDis",display: "离下一站距离"},
            { key: "uiTrainNumber",display: "列车车次号"},
            { key: "uiLineVoltage",display: "网压"},
            { key: "wLineCurrent",display: "网流"},
            { key: "usLineID",display: "线ID"},
        ]
    })
    property var initializedModels: ({})

    Connections{
            target: qmlCommunication

            function onDisplay_mvb_dataFromCpp(jsonData) {
                var mode = mvbModelLeft
                if(jsonData["dataSource"].includes("ccu_data"))
                {
                    mode = mvbModelMid;
                }
                else if(jsonData["dataSource"].includes("heart"))
                {
                    mode = mvbModelRight;
                }
                const mapping = fieldMappings[jsonData["dataSource"]];
                if(!mapping)
                {
                    return;
                }
                if(!initializedModels[jsonData["dataSource"]])
                {
                    mode.clear();
                    mapping.forEach(function(item){
                        if(jsonData[item.key] !== undefined)
                        {
                            mode.append({name: item.display,value: String(jsonData[item.key]+0)});
                        }
                        initializedModels[jsonData["dataSource"]] = true
                    })
                }
                else
                {
                    var index = 0;
                    mapping.forEach(function(item){
                        if(jsonData[item.key] !== undefined)
                        {
                            mode.setProperty(index, "value",String(jsonData[item.key]+0))
                            index++
                        }
                    })
                }
            }
    }
}