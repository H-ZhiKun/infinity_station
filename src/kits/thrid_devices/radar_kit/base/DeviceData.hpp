#pragma once

#include <map>
#include <qvectornd.h>
#include <string>
#include <vector>
#include "tis_global/Struct.h"
#include "kits/common/object_pool/ObjectPool.h"

namespace _Kits
{
    struct ChangeCenterData
    {
      public:
        float mf_change_x;
        float mf_change_y;

        ChangeCenterData() = default;
        ChangeCenterData(float x, float y) : mf_change_x(x), mf_change_y(y)
        {
        }
    };

    struct DeviceSurfaceData
    {
      public:
        std::vector<TIS_Info::DeviceSingleData> mvec_data_surface; // 一帧全部的数据
        unsigned int mus_index = 0;    // 锚   也是设备Index     为了能够完全异步    在超限中为unordered_map的index
        double mus_angle_start = 0;    // 角度开始         在超限中为相对中心点的偏移角度
        double mus_angle_end = 0;      // 角度结束
        long long mll_time_buffer = 0; // 时间戳 可能需要
        QString mstr_createtime = "";
        QString mstr_updatetime = "";
        bool mb_isMatch = false;     // 用于判断超限
        QString mi_location_id = ""; // 位置时间戳
        int mi_line_id = -1;         // 线路位置ID
        int mi_zSize = 0;

        DeviceSurfaceData() = default;

        DeviceSurfaceData(int nSize)
        {
            mvec_data_surface.resize(nSize);
        }

        DeviceSurfaceData(
            std::vector<TIS_Info::DeviceSingleData> &&data_surface, int index, double angle_start, double angle_end, long long time_buffer)
            : mvec_data_surface(std::move(data_surface)), mus_index(index), mus_angle_start(angle_start), mus_angle_end(angle_end),
              mll_time_buffer(time_buffer)
        {
        }

        std::unique_ptr<DeviceSurfaceData> clone() const
        {
            return std::make_unique<DeviceSurfaceData>(*this); // 依赖拷贝构造
        }

        void clear()
        {
            mvec_data_surface.clear();
            mus_index = 0;
            mus_angle_start = 0.0;
            mus_angle_end = 0.0;
            mll_time_buffer = 0;
            mstr_createtime.clear();
            mstr_updatetime.clear();
            mb_isMatch = false;
            mi_location_id = 0;
        }
    };

    static inline std::shared_ptr<_Kits::ObjectPool<DeviceSurfaceData>> gpool_device_data_all =
        std::make_shared<_Kits::ObjectPool<DeviceSurfaceData>>();

    static inline std::shared_ptr<_Kits::ObjectPool<DeviceSurfaceData>> gpool_device_data_one =
        std::make_shared<_Kits::ObjectPool<DeviceSurfaceData>>();

    struct Device3DData
    {
      public:
        std::map<float, DeviceSurfaceData> mmap_data_3d;
    };

    // 暂定
    enum DeviceErrCode
    {
        DEVICE_ERR_SUCCESS = 0,
        DEVICE_ERR_FAIL = -1,
        DEVICE_ERR_NOT_FOUND = -2,
        DEVICE_ERR_NOT_CONNECT = -3,
        DEVICE_ERR_NOT_SUPPORT = -4,
        DEVICE_ERR_NOT_SUPPORT_TYPE = -5,
        DEVICE_ERR_NOT_SUPPORT_VERSION = -6,
        DEVICE_ERR_NOT_SUPPORT_SENSOR_TYPE = -7,
    };

    struct Err
    {
      public:
        int code;
        std::string prompt;

        Err() = default;

        Err(int code, std::string prompt) : code(code), prompt(prompt) {};
    };
    struct YYPOINT_2F
    {
        float x;
        float y;
    };
    struct CYY3dResultStruct
    {
        uint64_t m_ui64DevTime;                    // 设备时间戳
        int64_t m_ft;                              // 文件时间戳
        uint64_t m_uiFrameNo;                      // 帧号
        int m_iCameraID;                           // 相机ID
        int m_iDataType;                           // 数据类型 (0: 原始数据, 1: 点云数据)
        std::vector<YYPOINT_2F> m_vecLaserPointXY; // 激光点云数据
        std::vector<float> m_vecFloat;             // 浮点数据
    };

} // namespace _Kits
