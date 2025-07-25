#pragma once
namespace TIS_Info
{

    namespace QmlCommunication
    {
        enum class QmlActions
        {
            PageChange = 0,             // 页面切换：在切换不同业务页面时调用
            ImageCapture = 1,           // 图片抓取
            LocationRecv = 2,           // 定位数据接收
            MVBTest = 3,                // MVB测试
            TaskSend = 4,               // 任务测试
            TaskControllerTest = 5,     // 任务控制
            CorrugationGet = 6,         // 波磨数据获取
            VideoGet = 7,               // 录像保存
            ArcUVGet = 8,               // 燃弧数据获取
            offlineRadarTaskidGet = 9,  // 离线雷达任务id获取
            TaskCSVSend = 10,           // 任务csv文件发送
            LineNameSend = 11,          // 线路名称发送
            LineBtnChoose = 12,         // 线路选择按钮
            PoleCalibration = 13,       // 杆号校准
            SimulateTrigger = 14,       // 模拟触发
            ULPoleCalibration = 15,     // 上下杆号校准
            DropperVideoRecord = 16,    // 吊弦录像
            camera_button_clicked = 17, // 巡检拍照相机状态按钮获取按钮点击
            UpdateConfigButton = 18,    // 更新配置
            ConfigDataSend = 19,        // 配置数据发送
            RfidSimulate = 20,          // rfid测试
            VideoRecordButton = 21,     // 视频录像按钮点击
            TestButton = 22,           // 测试按钮点击
            Parameter_modification = 23, // 参数修改
            
        };
        /**
         * @enum ForQmlSignals
         * @brief 定义 cpp 触发的信号，供qml中绑定使用。
         * @note 0-20为页面信号，21-40为页面中得操作
         */
        enum class ForQmlSignals
        {
            main_page = 0,
            mvb_page = 1,
            location_page = 2,
            task_page = 3,
            video_page = 4,
            corrugation_page = 5,
            arc_page = 6,
            radar_page = 7,
            taskcontroller_page = 8,
            offlineradar_page = 9,
            egvinspectionimage_page = 10, // 工程车巡检图片页面
            egvlocation_page = 11,        // 工程车定位页面
            egvvideo_page = 12,           // 工程车录像页面
            egvdropper_trigger_page = 13, // 工程车吊弦页面
            config_recv = 14,             // 配置编辑页面
            tasktable_recv = 21,          // 燃弧界面获取任务信息表信号
            locationdata_recv = 22,       // 燃弧界面定位信息表获取
            linedata_recv = 23,           // 线路信息表获取
            lineNameList_recv = 24,
            taskdata_recv = 25,        // 任务信息表获取,打开CSV方式
            photodata_recv = 26,       // 巡检图片数据接收
            jihedata_recv = 27,        // 几何数据接收
            rfid_recv = 28,            // rfid数据接收
            taskname_recv = 29,        // 任务名称接收
            display_mvb_data = 30,     // 界面展示mvb数据
            record_state = 31,         // 录像状态
            hardware_resource = 32,    // 硬件资源(cpu, 内存)占用
            logical_drive = 33,        // 硬盘资源占用
            camera_status_recv = 34,   // 摄像头状态接收
            maxtemp_recv = 35,         // 最高温度接收
            displaylog_send = 36,      // 显示日志
            radar_device_state = 37,   // 雷达设备状态接收
            radar_overrun_info = 38,   // 雷达超限信息接收
            radar_overrun_config = 39, // 雷达超限配置接收
            edit_state = 40,           // 配置程序修改状态
            save_path_send = 41,       // 保存路径发送
            database_state = 42,       // 数据库状态
            video_state = 43,          // 视频连接状态
            location_state = 44,       // 定位连接状态
            rfid_count_send = 45,      // rfid数量
            electri_data_send = 46,    // 电流数据发送
            press_data_send = 47,      // 压力数据发送
            spot_data_send = 48,       // 硬点数据发送
            voltage_data_send = 49,      // 电压数据发送

        };
    } // namespace QmlCommunication
} // namespace TIS_Info
