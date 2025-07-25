#ifndef _SICK_SKEW_DATA_H_
#define _SICK_SKEW_DATA_H_

#include "DeviceData.hpp"
#include "SkewDeviceData.hpp"
#include <cmath>
#include <qvectornd.h>
#include <vector>

namespace _Modules
{
    class SkewData : public SkewDeviceData
    {
      public:
        SkewData() = default;

        inline float isFloatZero(float value, float eps = 1e-6f)
        {
            return std::fabs(value) < eps ? 0.0f : value;
        }

        void OperateSkewData(std::vector<TIS_Info::DeviceSingleData> &device_singledata, float &angle_radians) override final
        {
            float radians;

            if (m_last_angle == angle_radians)
            {
                radians = m_last_radians; // 如果角度没有变化，使用上次的弧度
            }
            else
            {
                m_last_angle = angle_radians;                               // 更新上次角度
                m_last_radians = radians = angle_radians * (M_PI / 180.0f); // 转换为弧度
            }

            for (auto &point : device_singledata)
            {
                // 保存原始x值，因为计算y时会用到
                float original_x = point.mf_x;

                // 旋转矩阵变换
                point.mf_x = (original_x * isFloatZero(cos(radians)) - point.mf_y * isFloatZero(sin(radians)));
                point.mf_y = (original_x * isFloatZero(sin(radians)) + point.mf_y * isFloatZero(cos(radians)));
            }
        }
        float IsDeviceSkew(std::vector<TIS_Info::DeviceSingleData> &device_singledata, float &angle_radians) override final
        {
            if (device_singledata.empty())
            {
                return angle_radians;
            }
            float sum_x = 0.0f;
            float sum_y = 0.0f;
            float sum_xx = 0.0f;
            float sum_xy = 0.0f;
            for (const auto &point : device_singledata)
            {
                sum_x += point.mf_x;
                sum_y += point.mf_y;
                sum_xx += point.mf_x * point.mf_x;
                sum_xy += point.mf_x * point.mf_y;
            }
            float n = static_cast<float>(device_singledata.size());
            float denominator = n * sum_xx - sum_x * sum_x;
            if (isFloatZero(denominator))
            {
                return 0;
            }

            float m = (n * sum_xy - sum_x * sum_y) / denominator; // 斜率
            float b = (sum_y - m * sum_x) / n;                    // 截距
            float angle = std::atan2(m, 1);                       // 弧度
            float angle_degrees = angle * (180.0f / M_PI);

            return angle_degrees;
        }
        std::vector<TIS_Info::DeviceSingleData> GetParallelData(std::vector<TIS_Info::DeviceSingleData> &device_singledata,
                                                                float &height) override final
        {
            float threshold = 0.1; // 设置精度平行线精度

            // 突变的一系列点
            std::vector<TIS_Info::DeviceSingleData> mutational_points;
            if (device_singledata.empty())
            {
                // qDebug() << "device_singledata is empty";

                return {};
            }
            std::copy_if(device_singledata.begin(),
                         device_singledata.end(),
                         std::back_inserter(mutational_points),
                         [height](const TIS_Info::DeviceSingleData &point) {
                             return abs(point.mf_y) <= (height + 1.0) && abs(point.mf_y) > (height - 2.0);
                         });

            return mutational_points;
        }

        SkewData(const SkewData &) = delete;
        virtual ~SkewData() = default;

      private:
        float m_last_angle = 0.0f;   // 上次计算的角度
        float m_last_radians = 0.0f; // 上次计算的弧度
    };
} // namespace _Modules

#endif