pragma Singleton
import QtQuick 2.15

QtObject {
    // 统一数据
    id :root
    property var systemStatusData: {
        return {
            appData:[
                {
                    title: "系统状态监控",
                    modelName: "system_status",
                    info: "系统状态监控模块提供了所有应用程序的状态监控，包括程序运行状态、资源使用情况等信息。",
                    icon: "qrc:/infinity_station/res/icon/system_status.ico",
                    isBuiltIn: true, // 标记为内置界面
                    features: ["程序运行状态监控", "CPU、内存占用监控", "系统日志查看"]
                },
                {
                    title: "限界雷达",
                    modelName: "radar_page",
                    info: "限界雷达模块主要功能为,采集雷达信号显示并且保存。",
                    status: "运行中",
                    icon: "qrc:/infinity_station/res/icon/radar_outline.ico",
                    source: "radar_page/radar_page.qml",
                    fullConfigPath: "radar_config/radarFull.yaml",
                    features: ["采集雷达数据", "显示雷达数据", "保存雷达数据"]
                },
                {
                    title: "离线限界分析",
                    modelName: "egv_offline_analysis_page",
                    info: "",
                    status: "运行中",
                    icon: "qrc:/infinity_station/res/icon/radar_outline.ico",
                    source: "egv_offline_analysis_page/egv_offline_analysis_page.qml",
                    fullConfigPath: "radar_config/radarFull.yaml",
                    features: ["采集雷达数据", "显示雷达数据", "保存雷达数据"]
                },
                {
                    title: "RFID电子标签",
                    modelName: "rfid_page",
                    info: "RFID电子标签模块主要功能为,采集RFID信号显示并且保存。",
                    icon: "qrc:/infinity_station/res/icon/rfid.ico",
                    status: "运行中",
                    source: "rfid_page/rfid_page.qml",
                    fullConfigPath: "rfid_config/rfidFullConfig.yaml",
                    features: ["任务状态监控", "任务进度控制", "任务日志查看"]
                },  
                {
                    title: "定位系统",
                    modelName: "egv_location_page",
                    info: "定位系统模块提供了车辆的实时位置信息，包括经纬度、速度、方向等信息。",
                    icon: "qrc:/infinity_station/res/icon/evlocation.ico",
                    status: "运行中",
                    source: "egv_location_page/egv_location_page.qml",
                    features: ["定位数据显示", "几何数据接收显示", "发送触发信号到巡检拍照程序"]
                },
                {
                    title: "任务管理",
                    modelName: "task_page",
                    info: "任务管理模块提供了任务的执行状态、任务的执行进度等信息，允许用户手动控制任务的执行。",
                    icon: "qrc:/infinity_station/res/icon/task.ico",
                    status: "运行中",
                    source: "egv_task_page/egv_task_page.qml",
                    features: ["任务状态监控", "任务进度控制", "任务日志查看"]
                },                            
                {
                    title: "巡检拍照",
                    modelName: "egv_inspectionimage_page",
                    info: "巡检拍照模块主要功能为,接收触发信号实现触发拍照并且保存。",
                    icon: "qrc:/infinity_station/res/icon/inspection_image.ico",
                    status: "运行中",
                    source: "egv_inspectionimage_page/egv_inspectionimage_page.qml",
                    fullConfigPath: "basler_path/cameraFull.yaml",
                    features: ["自动触发拍照", "图片存储与管理", "拍照任务统计"]
                },
                {
                    title: "监控视频",
                    modelName: "egv_video_page",
                    info: "监控视频模块提供了实时监控视频的播放，包括摄像头的实时图像、视频的播放进度等信息。",
                    icon: "qrc:/infinity_station/res/icon/video.ico",
                    status: "运行中",
                    source: "egv_video_page/egv_video_page.qml",
                    fullConfigPath: "video_config/videoFull.yaml",
                    features: ["实时视频监控", "视频字幕叠加", "视频数据存储与管理"]
                },
                {
                    title: "吊弦触发",
                    modelName: "egv_dropper_trigger_page",
                    info: "吊弦触发块提供了实时监控视频的播放以及吊弦触发功能，包括触发拍照等信息。",
                    icon: "qrc:/infinity_station/res/icon/dropper_trigger.ico",
                    status: "运行中",
                    source: "egv_dropper_trigger_page/egv_dropper_trigger_page.qml",
                    features: ["实时视频监控", "吊弦触发拍照",]
                },
                {
                    title: "MVB",
                    configtitle: "mvb_configeditor",
                    modelName: "mvb_page",
                    info: "MVB监控模块提供了车辆与MVB系统的实时通信信息，包括车辆位置、速度、方向等信息。",
                    icon: "qrc:/infinity_station/res/icon/mvb.ico",
                    status: "运行中",
                    fullConfigPath: "mvb_card/mvbFull.yaml",
                    source: "mvb_page/mvb_page.qml"
                },
                {
                    title: "燃弧接收",
                    modelName: "arc_page",
                    info: "燃弧模块负责监控燃弧传感器接收到的数据。",
                    icon: "qrc:/infinity_station/res/icon/arc.ico",
                    status: "运行中",
                    source: "arc_page/arc_page.qml"
                },
                {
                    title: "配置修改",
                    modelName: "configchooser_page",
                    info: "配置修改模块提供了对车辆配置进行修改的功能。",
                    icon: "qrc:/infinity_station/res/icon/mvb.ico",
                    status: "运行中",
                    source: "configchooser_page/configchooser_page.qml"
                },
                {
                    title: "未来模块",
                    modelName: "",
                    info: "to be continued",
                    icon: "qrc:/infinity_station/res/icon/future.ico",
                    source: "",
                    isFutureModule: true
                }
            ],

            servers: [
                {
                    cpu: 0,
                    memory: 0,
                    cpuHistory: new Array(20).fill(0),
                    memoryHistory: new Array(20).fill(0)
                }
            ],
            modules: [
                {
                    title: "任务管理",
                    description: "任务管理模块提供了任务的执行状态、任务的执行进度等信息，允许用户手动控制任务的执行。",
                    status: "未启动",
                    icon: "qrc:/infinity_station/res/icon/task.ico"
                },
                {
                    title: "限界雷达",
                    description: "限界雷达模块主要功能为,采集雷达信号显示并且保存。",
                    status: "未启动",
                    icon: "qrc:/infinity_station/res/icon/radar_outline.ico"
                },
                {
                    title: "电子标签程序",
                    description: "电子标签程序模块主要功能为,采集电子标签信号显示并且保存。",
                    status: "未启动",
                    icon: "qrc:/infinity_station/res/icon/radar_outline.ico"
                },
                {
                    title: "定位系统",
                    description: "定位系统模块提供了车辆的实时位置信息，包括经纬度、速度、方向等信息。",
                    status: "未启动",
                    icon: "qrc:/infinity_station/res/icon/evlocation.ico"
                },
                {
                    title: "巡检拍照",
                    description: "巡检拍照模块主要功能为,接收触发信号实现触发拍照并且保存",
                    status: "未启动",
                    icon: "qrc:/infinity_station/res/icon/inspection_image.ico"
                },
                {
                    title: "监控视频",
                    description: "监控视频模块提供了实时监控视频的播放，包括摄像头的实时图像、视频的播放进度等信息。",
                    status: "未启动",
                    icon: "qrc:/infinity_station/res/icon/video.ico"
                },
                {
                    title: "配置修改",
                    description: "配置修改模块提供了对车辆配置进行修改的功能。",
                    status: "未启动",
                    icon: "qrc:/infinity_station/res/icon/mvb.ico"
                },
                {
                    title: "吊弦触发",
                    description: "吊弦触发块提供了实时监控视频的播放以及吊弦触发功能，包括触发拍照等信息。",
                    status: "未启动",
                    icon: "qrc:/infinity_station/res/icon/dropper_trigger.ico"
                }
            ],
            services: [
                {
                    title: "数据库连接",
                    state: false,
                    statename: "检测中...", // 初始状态
                    icon: "qrc:/icons/database.ico",
                    loading: true
                },
                {
                    title: "定位服务",
                    state: false,
                    statename: "检测中...",
                    icon: "qrc:/icons/acquisition.ico",
                    loading: true
                },
                {
                    title: "视频监控",
                    state: false,
                    statename: "检测中...",
                    icon: "qrc:/icons/dataservice.ico",
                    loading: true
                }
            ],
            configEditors: [
                {
                    title: "MVB配置",
                    modelName: "mvb_configeditor",
                    icon: "qrc:/infinity_station/res/icon/mvb.ico",
                    fullConfigPath: "mvb_card/mvbFull.yaml",
                    source: "mvb_page/mvb_config_editor.qml"
                },
                {
                    title: "录像程序配置",
                    modelName: "egv_video_configeditor",
                    icon: "qrc:/infinity_station/res/icon/video.ico",
                    source: "egv_video_page/egv_video_configeditor.qml"
                },
                {
                    title: "吊弦触发程序配置",
                    modelName: "dt_config_editor",
                    icon: "qrc:/infinity_station/res/icon/video.ico",
                    source: "egv_dropper_trigger_page/dt_config_editor.qml"
                }

            ]
        }
    }

     // 更新服务状态的方法
    function updateService(serviceTitle, state) {
        for (var i = 0; i < systemStatusData.services.length; i++) {
            var service = systemStatusData.services[i]
            if (service.title === serviceTitle) {
                service.state = state
                service.statename = state ? "正常" : "异常"
                service.loading = false
                break
            }
        }
        systemStatusData = Object.assign({}, systemStatusData)
    }

    // 重置所有状态为检测中（应用启动时调用）
    function resetAllStatus() {
        systemStatusData.services.forEach(service => {
            service.state = false
            service.statename = "检测中..."
            service.loading = true
        })
        systemStatusData = Object.assign({}, systemStatusData)
    }
}