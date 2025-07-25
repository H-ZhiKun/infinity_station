import QtQuick 
import QtQuick.Controls 
import QtQuick.Layouts 
import InfinityStation 

Window {
    width: 800
    height: 400
    minimumWidth: 800
    minimumHeight: 400
    maximumWidth: 800
    maximumHeight: 400
    visible: true
    color: "#ffffff"
    title: "System Monitor"

    property var sortColumn: "cpu"
    property bool sortOrderDescending: false

    function sortByColumn(column) {
        if (sortColumn === column) {
            sortOrderDescending = !sortOrderDescending;
        } else {
            sortColumn = column;
            sortOrderDescending = true;
        }

        processModel.sort(sortColumn, sortOrderDescending ? Qt.DescendingSort : Qt.AscendingSort);
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Header
        Rectangle {
            width: 800
            height: 40
            color: "#f0f0f0"
            border.color: "#cccccc"

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 0
                anchors.rightMargin: 0
                anchors.topMargin: 0
                anchors.bottomMargin: 0
                spacing: 0

                Button {
                    Layout.preferredWidth: 80
                    Layout.fillWidth: true
                    background: Rectangle {
                        color: "#f0f0f0"
                        border.color: "#cccccc"
                        //border.rightWidth: 1
                    }
                    onClicked:qmlCommunication.behaviorFromQml(QmlCommunication.TaskControllerTest, "TaskController cpu sort")
                    contentItem: Text {
                        text: "CPU (%)"
                        font.bold: true
                        font.pixelSize: 12
                        horizontalAlignment: Text.AlignRight
                        verticalAlignment: Text.AlignVCenter
                        padding: 5
                    }
                }

                Button {
                    Layout.preferredWidth: 100
                    Layout.fillWidth: true
                    background: Rectangle {
                        color: "#f0f0f0"
                        border.color: "#cccccc"
                        //border.rightWidth: 1
                    }
                    onClicked:qmlCommunication.behaviorFromQml(QmlCommunication.TaskControllerTest, "TaskController Memory sort")
                    contentItem: Text {
                        text: "Memory (MB)"
                        font.bold: true
                        font.pixelSize: 12
                        horizontalAlignment: Text.AlignRight
                        verticalAlignment: Text.AlignVCenter
                        padding: 5
                    }
                }

                Button {
                    Layout.preferredWidth: 100
                    Layout.fillWidth: true
                    background: Rectangle {
                        color: "#f0f0f0"
                        border.color: "#cccccc"
                        //border.rightWidth: 1
                    }
                    onClicked:qmlCommunication.behaviorFromQml(QmlCommunication.TaskControllerTest, "TaskController Disk sort")
                    contentItem: Text {
                        text: "Disk (MB/s)"
                        font.bold: true
                        font.pixelSize: 12
                        horizontalAlignment: Text.AlignRight
                        verticalAlignment: Text.AlignVCenter
                        padding: 5
                    }
                }

                Button {
                    Layout.preferredWidth: 80
                    Layout.fillWidth: true
                    background: Rectangle {
                        color: "#f0f0f0"
                        border.color: "#cccccc"
                    }
                    onClicked:qmlCommunication.behaviorFromQml(QmlCommunication.TaskControllerTest, "TaskController GPU sort")
                    contentItem: Text {
                        text: "GPU (%)"
                        font.bold: true
                        font.pixelSize: 12
                        horizontalAlignment: Text.AlignRight
                        verticalAlignment: Text.AlignVCenter
                        padding: 5
                    }
                }
            }
        }

        // ListView
        ListView {
            width: 800
            height: 360
            model: processModel
            clip: true
            delegate: Rectangle {
                width: 800
                height: 40
                color: index % 2 === 0 ? "#ffffff" : "#fff7e6"
                border.color: "#cccccc"
                //border.bottomWidth: 1

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 10
                    anchors.rightMargin: 10
                    spacing: 0

                    // CPU
                    Text {
                        Layout.preferredWidth: 80
                        Layout.fillWidth: true
                        text: cpu + "%"
                        font.pixelSize: 12
                        horizontalAlignment: Text.AlignRight
                        verticalAlignment: Text.AlignVCenter
                        padding: 5
                    }

                    Rectangle {
                        Layout.preferredWidth: 1
                        color: "#cccccc"
                    }

                    // Memory
                    Text {
                        Layout.preferredWidth: 100
                        Layout.fillWidth: true
                        text: memory + " MB"
                        font.pixelSize: 12
                        horizontalAlignment: Text.AlignRight
                        verticalAlignment: Text.AlignVCenter
                        padding: 5
                    }

                    Rectangle {
                        Layout.preferredWidth: 1
                        color: "#cccccc"
                    }

                    // Disk
                    Text {
                        Layout.preferredWidth: 100
                        Layout.fillWidth: true
                        text: disk + " MB/秒"
                        font.pixelSize: 12
                        horizontalAlignment: Text.AlignRight
                        verticalAlignment: Text.AlignVCenter
                        padding: 5
                    }

                    Rectangle {
                        Layout.preferredWidth: 1
                        color: "#cccccc"
                    }

                    // GPU
                    Text {
                        Layout.preferredWidth: 80
                        Layout.fillWidth: true
                        text: gpu + "%"
                        font.pixelSize: 12
                        horizontalAlignment: Text.AlignRight
                        verticalAlignment: Text.AlignVCenter
                        padding: 5
                    }
                }
            }
        }
    }

    ListModel {
        id: processModel
        ListElement {
            cpu: "0"
            memory: "1,179.1"
            disk: "0"
            network: "0"
            gpu: "0"
            gpuEngine: "GPU 0 - 3D"
        }
        ListElement {
            cpu: "0"
            memory: "708.2"
            disk: "0"
            network: "0"
            gpu: "0"
            gpuEngine: ""
        }
        ListElement {
            cpu: "0"
            memory: "559.4"
            disk: "0"
            network: "0"
            gpu: "0"
            gpuEngine: ""
        }
        // 可以继续添加更多数据...
    }
}

