import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import InfinityStation 1.0
import public_controls 1.0

Window {
    id: root
    width: 1000
    height: 700
    title: "配置修改工具"
    visible: true

    QtObject {
        id: appColors
        property color sidebarBackground: "#2c3e50"
        property color sidebarText: "#ecf0f1"
        property color sidebarHighlight: "#3498db"
        property color mainBackground: "#f5f5f5"
        property color headerBackground: "#34495e"
        property color headerText: "#ffffff"
    }

    // 主布局
    RowLayout {
        anchors.fill: parent
        spacing: 0

        // 左侧选择栏
        Rectangle {
            id: sidebar
            Layout.preferredWidth: 200
            Layout.fillHeight: true
            color: appColors.sidebarBackground

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                // 标题区域
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 60
                    color: appColors.headerBackground

                    Label {
                        anchors.centerIn: parent
                        text: "配置选择"
                        font.pixelSize: 16
                        font.bold: true
                        color: appColors.headerText
                    }
                }

                // 配置选项列表
                ListView {
                    id: configList
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    model: configModel

                    delegate: Rectangle {
                        width: configList.width
                        height: 50
                        color: configList.currentIndex === index ? appColors.sidebarHighlight : "transparent"

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                configList.currentIndex = index
                                stackView.replace(model.source)
                            }
                        }

                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 15
                            spacing: 10

                            Rectangle {
                                width: 4
                                height: 20
                                radius: 2
                                color: configList.currentIndex === index ? "white" : "transparent"
                                visible: configList.currentIndex === index
                            }

                            Label {
                                text: model.name
                                font.pixelSize: 14
                                color: appColors.sidebarText
                            }

                            Item { Layout.fillWidth: true }
                        }
                    }
                }

                // 底部版本信息
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 30
                    color: Qt.darker(appColors.sidebarBackground, 1.2)

                    Label {
                        anchors.centerIn: parent
                        text: "版本 1.0.0"
                        font.pixelSize: 12
                        color: Qt.lighter(appColors.sidebarText, 0.8)
                    }
                }
            }
        }

        // 右侧内容区
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: appColors.mainBackground

            StackView {
                id: stackView
                anchors.fill: parent
                initialItem: Loader {
                    source: "./mvb_page/mvb_config_editor.qml" // 默认加载MVB配置界面
                }
            }
        }
    }

    // 动态加载配置选项模型
    ListModel {
        id: configModel
    }

    // 从外部获取配置选项数据
    Component.onCompleted: {
        // 取 Modules 单例中的 configEditors
        var editors = Modules.systemStatusData.configEditors
        configModel.clear()
        for (var i = 0; i < editors.length; i++) {
            // 你可以根据需要取 title、source 等字段
            configModel.append({
                name: editors[i].title,
                source: editors[i].source
            })
        }
    }
}