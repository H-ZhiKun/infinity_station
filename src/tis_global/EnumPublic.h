#pragma once

namespace TIS_Info
{
    namespace LocationCommunication
    {
        enum LineType
        {
            UNKNOWN_LINE = 0,
            UP_LINE = 1,
            DOWN_LINE = 2,
        };

        enum PositionType
        {
            PT_NONE = 0,    // 无检测结果
            PT_LINE = 1,    // 普通导线
            PT_DIST = 2,    // 跨距定位
            PT_ZIG = 3,     // 拉出值定位
            PT_IMG = 4,     // 图片定位
            PT_HAND = 5,    // 手动定位
            PT_RFID = 6,    // RFID定位
            PT_STATION = 7, // 站区定位
        };
    } // namespace LocationCommunication
    namespace EnumPublic
    {
        enum class AVPixelFormat
        {
            AV_PIX_FMT_NONE = -1,

            AV_PIX_FMT_YUV420P = 0,
            AV_PIX_FMT_YUYV422 = 1,
            AV_PIX_FMT_RGB24 = 2,
            AV_PIX_FMT_BGR24 = 3,
            AV_PIX_FMT_YUV422P = 4,
            AV_PIX_FMT_YUV444P = 5,
            AV_PIX_FMT_YUV410P = 6,
            AV_PIX_FMT_YUV411P = 7,
            AV_PIX_FMT_GRAY8 = 8,
            AV_PIX_FMT_MONOWHITE = 9,
            AV_PIX_FMT_MONOBLACK = 10,
            AV_PIX_FMT_PAL8 = 11,
            AV_PIX_FMT_YUVJ420P = 12,
            AV_PIX_FMT_YUVJ422P = 13,
            AV_PIX_FMT_YUVJ444P = 14,
            AV_PIX_FMT_UYVY422 = 15,
            AV_PIX_FMT_UYYVYY411 = 16,
            AV_PIX_FMT_BGR8 = 17,
            AV_PIX_FMT_BGR4 = 18,
            AV_PIX_FMT_BGR4_BYTE = 19,
            AV_PIX_FMT_RGB8 = 20,
            AV_PIX_FMT_RGB4 = 21,
            AV_PIX_FMT_RGB4_BYTE = 22,
            AV_PIX_FMT_NV12 = 23,
            AV_PIX_FMT_NV21 = 24,

            AV_PIX_FMT_ARGB = 25,
            AV_PIX_FMT_RGBA = 26,
            AV_PIX_FMT_ABGR = 27,
            AV_PIX_FMT_BGRA = 28,

            AV_PIX_FMT_GRAY16BE = 29,
            AV_PIX_FMT_GRAY16LE = 30,
            AV_PIX_FMT_YUV440P = 31,
            AV_PIX_FMT_YUVJ440P = 32,
            AV_PIX_FMT_YUVA420P = 33,
            AV_PIX_FMT_RGB48BE = 34,
            AV_PIX_FMT_RGB48LE = 35,

            AV_PIX_FMT_RGB565BE = 36,
            AV_PIX_FMT_RGB565LE = 37,
            AV_PIX_FMT_RGB555BE = 38,
            AV_PIX_FMT_RGB555LE = 39,

            AV_PIX_FMT_BGR565BE = 40,
            AV_PIX_FMT_BGR565LE = 41,
            AV_PIX_FMT_BGR555BE = 42,
            AV_PIX_FMT_BGR555LE = 43,

            AV_PIX_FMT_VAAPI = 44,

            AV_PIX_FMT_YUV420P16LE = 45,
            AV_PIX_FMT_YUV420P16BE = 46,
            AV_PIX_FMT_YUV422P16LE = 47,
            AV_PIX_FMT_YUV422P16BE = 48,
            AV_PIX_FMT_YUV444P16LE = 49,
            AV_PIX_FMT_YUV444P16BE = 50,
            AV_PIX_FMT_DXVA2_VLD = 51,

            AV_PIX_FMT_RGB444LE = 52,
            AV_PIX_FMT_RGB444BE = 53,
            AV_PIX_FMT_BGR444LE = 54,
            AV_PIX_FMT_BGR444BE = 55,
            AV_PIX_FMT_YA8 = 56,

            AV_PIX_FMT_BGR48BE = 57,
            AV_PIX_FMT_BGR48LE = 58,

            AV_PIX_FMT_YUV420P9BE = 59,
            AV_PIX_FMT_YUV420P9LE = 60,
            AV_PIX_FMT_YUV420P10BE = 61,
            AV_PIX_FMT_YUV420P10LE = 62,
            AV_PIX_FMT_YUV422P10BE = 63,
            AV_PIX_FMT_YUV422P10LE = 64,
            AV_PIX_FMT_YUV444P9BE = 65,
            AV_PIX_FMT_YUV444P9LE = 66,
            AV_PIX_FMT_YUV444P10BE = 67,
            AV_PIX_FMT_YUV444P10LE = 68,
            AV_PIX_FMT_YUV422P9BE = 69,
            AV_PIX_FMT_YUV422P9LE = 70,
            AV_PIX_FMT_GBRP = 71,
            AV_PIX_FMT_GBR24P = 71,
            AV_PIX_FMT_GBRP9BE = 72,
            AV_PIX_FMT_GBRP9LE = 73,
            AV_PIX_FMT_GBRP10BE = 74,
            AV_PIX_FMT_GBRP10LE = 75,
            AV_PIX_FMT_GBRP16BE = 76,
            AV_PIX_FMT_GBRP16LE = 77,
            AV_PIX_FMT_YUVA422P = 78,
            AV_PIX_FMT_YUVA444P = 79,
            AV_PIX_FMT_YUVA420P9BE = 80,
            AV_PIX_FMT_YUVA420P9LE = 81,
            AV_PIX_FMT_YUVA422P9BE = 82,
            AV_PIX_FMT_YUVA422P9LE = 83,
            AV_PIX_FMT_YUVA444P9BE = 84,
            AV_PIX_FMT_YUVA444P9LE = 85,
            AV_PIX_FMT_YUVA420P10BE = 86,
            AV_PIX_FMT_YUVA420P10LE = 87,
            AV_PIX_FMT_YUVA422P10BE = 88,
            AV_PIX_FMT_YUVA422P10LE = 89,
            AV_PIX_FMT_YUVA444P10BE = 90,
            AV_PIX_FMT_YUVA444P10LE = 91,
            AV_PIX_FMT_YUVA420P16BE = 92,
            AV_PIX_FMT_YUVA420P16LE = 93,
            AV_PIX_FMT_YUVA422P16BE = 94,
            AV_PIX_FMT_YUVA422P16LE = 95,
            AV_PIX_FMT_YUVA444P16BE = 96,
            AV_PIX_FMT_YUVA444P16LE = 97,

            AV_PIX_FMT_VDPAU = 98,

            AV_PIX_FMT_XYZ12LE = 99,
            AV_PIX_FMT_XYZ12BE = 100,
            AV_PIX_FMT_NV16 = 101,
            AV_PIX_FMT_NV20LE = 102,
            AV_PIX_FMT_NV20BE = 103,

            AV_PIX_FMT_RGBA64BE = 104,
            AV_PIX_FMT_RGBA64LE = 105,
            AV_PIX_FMT_BGRA64BE = 106,
            AV_PIX_FMT_BGRA64LE = 107,

            AV_PIX_FMT_YVYU422 = 108,

            AV_PIX_FMT_YA16BE = 109,
            AV_PIX_FMT_YA16LE = 110,

            AV_PIX_FMT_GBRAP = 111,
            AV_PIX_FMT_GBRAP16BE = 112,
            AV_PIX_FMT_GBRAP16LE = 113,

            AV_PIX_FMT_QSV = 114,

            AV_PIX_FMT_MMAL = 115,

            AV_PIX_FMT_D3D11VA_VLD = 116,

            AV_PIX_FMT_CUDA = 117,

            AV_PIX_FMT_0RGB = 118,
            AV_PIX_FMT_RGB0 = 119,
            AV_PIX_FMT_0BGR = 120,
            AV_PIX_FMT_BGR0 = 121,

            AV_PIX_FMT_YUV420P12BE = 122,
            AV_PIX_FMT_YUV420P12LE = 123,
            AV_PIX_FMT_YUV420P14BE = 124,
            AV_PIX_FMT_YUV420P14LE = 125,
            AV_PIX_FMT_YUV422P12BE = 126,
            AV_PIX_FMT_YUV422P12LE = 127,
            AV_PIX_FMT_YUV422P14BE = 128,
            AV_PIX_FMT_YUV422P14LE = 129,
            AV_PIX_FMT_YUV444P12BE = 130,
            AV_PIX_FMT_YUV444P12LE = 131,
            AV_PIX_FMT_YUV444P14BE = 132,
            AV_PIX_FMT_YUV444P14LE = 133,
            AV_PIX_FMT_GBRP12BE = 134,
            AV_PIX_FMT_GBRP12LE = 135,
            AV_PIX_FMT_GBRP14BE = 136,
            AV_PIX_FMT_GBRP14LE = 137,
            AV_PIX_FMT_YUVJ411P = 138,

            AV_PIX_FMT_BAYER_BGGR8 = 139,
            AV_PIX_FMT_BAYER_RGGB8 = 140,
            AV_PIX_FMT_BAYER_GBRG8 = 141,
            AV_PIX_FMT_BAYER_GRBG8 = 142,
            AV_PIX_FMT_BAYER_BGGR16LE = 143,
            AV_PIX_FMT_BAYER_BGGR16BE = 144,
            AV_PIX_FMT_BAYER_RGGB16LE = 145,
            AV_PIX_FMT_BAYER_RGGB16BE = 146,
            AV_PIX_FMT_BAYER_GBRG16LE = 147,
            AV_PIX_FMT_BAYER_GBRG16BE = 148,
            AV_PIX_FMT_BAYER_GRBG16LE = 149,
            AV_PIX_FMT_BAYER_GRBG16BE = 150,

            AV_PIX_FMT_YUV440P10LE = 151,
            AV_PIX_FMT_YUV440P10BE = 152,
            AV_PIX_FMT_YUV440P12LE = 153,
            AV_PIX_FMT_YUV440P12BE = 154,
            AV_PIX_FMT_AYUV64LE = 155,
            AV_PIX_FMT_AYUV64BE = 156,

            AV_PIX_FMT_VIDEOTOOLBOX = 157,

            AV_PIX_FMT_P010LE = 158,
            AV_PIX_FMT_P010BE = 159,

            AV_PIX_FMT_GBRAP12BE = 160,
            AV_PIX_FMT_GBRAP12LE = 161,

            AV_PIX_FMT_GBRAP10BE = 162,
            AV_PIX_FMT_GBRAP10LE = 163,

            AV_PIX_FMT_MEDIACODEC = 164,

            AV_PIX_FMT_GRAY12BE = 165,
            AV_PIX_FMT_GRAY12LE = 166,
            AV_PIX_FMT_GRAY10BE = 167,
            AV_PIX_FMT_GRAY10LE = 168,

            AV_PIX_FMT_P016LE = 169,
            AV_PIX_FMT_P016BE = 170,

            AV_PIX_FMT_D3D11 = 171,

            AV_PIX_FMT_GRAY9BE = 172,
            AV_PIX_FMT_GRAY9LE = 173,

            AV_PIX_FMT_GBRPF32BE = 174,
            AV_PIX_FMT_GBRPF32LE = 175,
            AV_PIX_FMT_GBRAPF32BE = 176,
            AV_PIX_FMT_GBRAPF32LE = 177,

            AV_PIX_FMT_DRM_PRIME = 178,

            AV_PIX_FMT_OPENCL = 179,

            AV_PIX_FMT_GRAY14BE = 180,
            AV_PIX_FMT_GRAY14LE = 181,

            AV_PIX_FMT_GRAYF32BE = 182,
            AV_PIX_FMT_GRAYF32LE = 183,

            AV_PIX_FMT_YUVA422P12BE = 184,
            AV_PIX_FMT_YUVA422P12LE = 185,
            AV_PIX_FMT_YUVA444P12BE = 186,
            AV_PIX_FMT_YUVA444P12LE = 187,

            AV_PIX_FMT_NV24 = 188,
            AV_PIX_FMT_NV42 = 189,

            AV_PIX_FMT_VULKAN = 190,

            AV_PIX_FMT_Y210BE = 191,
            AV_PIX_FMT_Y210LE = 192,

            AV_PIX_FMT_X2RGB10LE = 193,
            AV_PIX_FMT_X2RGB10BE = 194,
            AV_PIX_FMT_X2BGR10LE = 195,
            AV_PIX_FMT_X2BGR10BE = 196,

            AV_PIX_FMT_P210BE = 197,
            AV_PIX_FMT_P210LE = 198,

            AV_PIX_FMT_P410BE = 199,
            AV_PIX_FMT_P410LE = 200,

            AV_PIX_FMT_P216BE = 201,
            AV_PIX_FMT_P216LE = 202,

            AV_PIX_FMT_P416BE = 203,
            AV_PIX_FMT_P416LE = 204,

            AV_PIX_FMT_VUYA = 205,

            AV_PIX_FMT_RGBAF16BE = 206,
            AV_PIX_FMT_RGBAF16LE = 207,

            AV_PIX_FMT_VUYX = 208,

            AV_PIX_FMT_P012LE = 209,
            AV_PIX_FMT_P012BE = 210,

            AV_PIX_FMT_Y212BE = 211,
            AV_PIX_FMT_Y212LE = 212,

            AV_PIX_FMT_XV30BE = 213,
            AV_PIX_FMT_XV30LE = 214,

            AV_PIX_FMT_XV36BE = 215,
            AV_PIX_FMT_XV36LE = 216,

            AV_PIX_FMT_RGBF32BE = 217,
            AV_PIX_FMT_RGBF32LE = 218,

            AV_PIX_FMT_RGBAF32BE = 219,
            AV_PIX_FMT_RGBAF32LE = 220,

            AV_PIX_FMT_P212BE = 221,
            AV_PIX_FMT_P212LE = 222,

            AV_PIX_FMT_P412BE = 223,
            AV_PIX_FMT_P412LE = 224,

            AV_PIX_FMT_GBRAP14BE = 225,
            AV_PIX_FMT_GBRAP14LE = 226,

            AV_PIX_FMT_D3D12 = 227,

            AV_PIX_FMT_NB = 228 // number of pixel formats, DO NOT USE THIS if you want to link with shared libav* because the number of
                                // formats might differ between versions
        };

        enum TaskState
        {
            TASK_RUNNING = 0,
            TASK_STOPPED = 1,
            TASK_PAUSED = 2
        };
        enum TaskLineDir
        {
            TASK_UP_LINE = 0,
            TASK_DOWN_LINE = 1
        };
        enum TaskTrainDir
        {
            TASK_FORWARD_TRAIN = 0,
            TASK_BACKWARD_TRAIN = 1
        };
        enum AmifAlarmType
        {
            AmifAlarmType_None = 0,
            AmifAlarmType_Temp = 1,          // 弓网接触线温度超限
            AmifAlarmType_CatOff = 2,        // 拉出值超限
            AmifAlarmType_CatHei = 3,        // 导高超限
            AmifAlarmType_Arc = 4,           // 燃弧超限
            AmifAlarmType_Str = 5,           // 受电弓结构超限
            AmifAlarmType_BowSystem = 6,     // 弓网系统
            AmifAlarmType_AbrValOutlimit = 7 // 弓网接触线磨耗值超限
        };
        enum AmifAlarmLevel
        {
            AmifAlarmLevel_None = 0,
            AmifAlarmLevel_Level1 = 1, // 严重超限
            AmifAlarmLevel_Level2 = 2, // 中等超限
            AmifAlarmLevel_Level3 = 3, // 轻微超限
        };
        enum AsatModuleType
        {
            AsatModuleType_VideoCam = 0,              // 录像模块
            AsatModuleType_GeoParMon = 1,             // 几何模块
            AsatModuleType_ArcMod = 2,                // 燃弧模块
            AsatModuleType_InfMod = 3,                // 红外温度模块
            AsatModuleType_StrMod = 4,                // 受电弓结构识别模块
            AsatModuleType_ComMod = 5,                // 补偿模块
            AsatModuleType_TcmsMod = 6,               // Tcms服务模块
            AsatModuleType_PantoMod = 7,              // 受电弓模块
            AsatModuleType_YangJiaoMod = 8,           // 羊角模块
            AsatModuleType_LocationMod = 9,           // 定位模块
            AsatModuleType_Acceleration = 10,         // 加速度模块
            AsatModuleType_SysCommunicationMode = 11, // 系统通信模块
            AsatModuleType_AbrValMod = 12             // 接触线磨耗检测模块
        };
        enum AsatModuleState
        {
            AsatModuleState_Normal = 0,
            AsatModuleState_Fault = 1
        };
    } // namespace EnumPublic

} // namespace TIS_Info