#include "HardwareWindows.h"
#include "kits/common/log/CRossLogger.h"
#include <Windows.h>
#include <psapi.h>

#pragma comment(lib, "psapi.lib")

namespace _Kits
{

    HardwareWindows::HardwareWindows()
    {
    }

    HardwareWindows::~HardwareWindows() noexcept
    {
    }

    QVariantMap HardwareWindows::resourcesInfo()
    {
        QVariantMap result;

        // ===== 内存采集 =====
        HANDLE hProcess = GetCurrentProcess();
        PROCESS_MEMORY_COUNTERS_EX pmc;
        MEMORYSTATUSEX memInfo = {sizeof(memInfo)};

        if (GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS *)&pmc, sizeof(pmc)) && GlobalMemoryStatusEx(&memInfo))
        {
            // PrivateUsage 是任务管理器“详细信息”页对应的内存占用（单位：字节）
            double memoryCostMB = pmc.PrivateUsage / 1024.0 / 1024.0;               // 当前进程专用内存（MiB）
            double memoryTotalGB = memInfo.ullTotalPhys / 1024.0 / 1024.0 / 1024.0; // 系统总物理内存（GiB）

            result["memory_cost"] = memoryCostMB;
            result["memory_total"] = memoryTotalGB;
        }

        // ===== CPU 占用率采样（1秒） =====
        FILETIME ftSysIdle1, ftSysKernel1, ftSysUser1;
        FILETIME ftProcCreate, ftProcExit, ftProcKernel1, ftProcUser1;

        GetSystemTimes(&ftSysIdle1, &ftSysKernel1, &ftSysUser1);
        GetProcessTimes(hProcess, &ftProcCreate, &ftProcExit, &ftProcKernel1, &ftProcUser1);

        Sleep(1000); // 采样间隔 1 秒

        FILETIME ftSysIdle2, ftSysKernel2, ftSysUser2;
        FILETIME ftProcKernel2, ftProcUser2;
        GetSystemTimes(&ftSysIdle2, &ftSysKernel2, &ftSysUser2);
        GetProcessTimes(hProcess, &ftProcCreate, &ftProcExit, &ftProcKernel2, &ftProcUser2);

        auto fileTimeToUInt64 = [](const FILETIME &ft) -> ULONGLONG { return ((ULONGLONG)ft.dwHighDateTime << 32) | ft.dwLowDateTime; };

        ULONGLONG sysKernelDelta = fileTimeToUInt64(ftSysKernel2) - fileTimeToUInt64(ftSysKernel1);
        ULONGLONG sysUserDelta = fileTimeToUInt64(ftSysUser2) - fileTimeToUInt64(ftSysUser1);
        ULONGLONG sysTotalDelta = sysKernelDelta + sysUserDelta;

        ULONGLONG procKernelDelta = fileTimeToUInt64(ftProcKernel2) - fileTimeToUInt64(ftProcKernel1);
        ULONGLONG procUserDelta = fileTimeToUInt64(ftProcUser2) - fileTimeToUInt64(ftProcUser1);
        ULONGLONG procTotalDelta = procKernelDelta + procUserDelta;

        double cpuPercent = 0.0;
        if (sysTotalDelta > 0)
            cpuPercent = (double)procTotalDelta / sysTotalDelta * 100.0;

        result["cpu_percent"] = cpuPercent;

        return result;
    }

    QVariantList HardwareWindows::logicalDriveInfo()
    {
        wchar_t buffer[256] = {0};
        QVariantList listInfo;
        DWORD length = GetLogicalDriveStringsW(sizeof(buffer), buffer);

        if (length == 0)
        {
            DWORD error = GetLastError();
            LogError("Failed to get logical drive strings. Error: {}", error);
            return {};
        }

        wchar_t *drive = buffer;
        while (*drive)
        {
            ULARGE_INTEGER freeBytesAvailable = {}, totalBytes = {}, totalFreeBytes = {};

            if (GetDiskFreeSpaceExW(drive, &freeBytesAvailable, &totalBytes, &totalFreeBytes))
            {
                auto wstrDisk = std::wstring(drive);
                if (wstrDisk.back() == L'\\')
                {
                    wstrDisk.pop_back();
                }
                QVariantMap itemDisk;
                itemDisk["drive"] = wstringToString(std::wstring(wstrDisk)).c_str();
                itemDisk["total_size"] = totalBytes.QuadPart / (1024 * 1024 * 1024);
                itemDisk["free_space"] = totalFreeBytes.QuadPart / (1024 * 1024 * 1024);
                listInfo.push_back(itemDisk);
            }
            else
            {
                DWORD error = GetLastError();
                LogError("Failed to get space for drive: {}, Error: {}", wstringToString(drive), error);
                return {};
            }

            drive += wcslen(drive) + 1; // 下一个盘符
        }
        return listInfo;
    }

    std::string HardwareWindows::wstringToString(const std::wstring &wstr)
    {
        if (wstr.empty())
            return {};

        int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), nullptr, 0, nullptr, nullptr);
        std::string result(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), &result[0], size_needed, nullptr, nullptr);
        return result;
    }
} // namespace _Kits
