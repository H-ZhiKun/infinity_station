#include "GetAllSickDeviceData.h"

using namespace SICK;
using namespace _Modules;
using namespace _Kits;
using namespace TIS_Info;
// 修复后的代码
void GetAllSickDeviceData::ChangeToNormalData(DeviceSurfaceData &device_surface_data,
                                              const std::vector<SICK::Point3D> &vec_sick_data,
                                              const ChangeCenterData &change_center_data)
{
    int iloop_size_sick = vec_sick_data.size();
    int iloop_size_local = device_surface_data.mvec_data_surface.size();

    for (int i = 0; i < iloop_size_sick && i < iloop_size_local; i++)
    {
        device_surface_data.mvec_data_surface[i].mf_x = vec_sick_data[i].X;
        device_surface_data.mvec_data_surface[i].mf_y = vec_sick_data[i].Y;
    }

    ChangeCenterPoint(device_surface_data.mvec_data_surface, change_center_data);
}
void GetAllSickDeviceData::GetSingleDeviceData(DeviceSurfaceData &device_surface_data,
                                               SICK::SICKLidarSensor &sick_device,
                                               std::vector<SICK::Point3D> &single_data,
                                               const ChangeCenterData &change_center_data)
{
    SICK::CommonError common_error;

    sick_device.getSingleProfile(single_data, common_error);

    m_err_code.code = (int)common_error.code;
    m_err_code.prompt = common_error.prompt;

    ChangeToNormalData(device_surface_data, single_data, change_center_data);
    single_data.clear();
}