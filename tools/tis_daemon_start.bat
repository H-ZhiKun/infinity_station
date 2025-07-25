@echo off
setlocal enabledelayedexpansion

:: 动态获取当前脚本所在目录（批处理文件所在路径）
set "WORKDIR=%~dp0"

:: 进入当前脚本所在目录
cd /d "%WORKDIR%"

:: 创建本地文件（自动继承当前路径）
echo. > tis_daemon.exe.local
echo. > togee_infinity_station.exe.local

:: 强制环境配置
set __COMPAT_LAYER=WinXP
set PATH=%WORKDIR%;%PATH%

:: 启动监控进程（使用动态路径）
start "" "%WORKDIR%tis_daemon.exe"

endlocal