@echo off
REM ============================================
REM PetEdu Windows 构建脚本
REM ============================================

setlocal enabledelayedexpansion

echo ============================================
echo PetEdu 构建脚本 (Windows)
echo ============================================
echo.

REM 检查 CMake 是否安装
where cmake >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo [错误] 未找到 CMake，请先安装 CMake 3.16+
    echo 下载地址：https://cmake.org/download/
    exit /b 1
)

REM 检查 Qt 环境
if "%QT5_DIR%"=="" (
    echo [警告] QT5_DIR 环境变量未设置，请确保 Qt5 已正确安装
    echo 如使用 vcpkg: set QT5_DIR=C:\vcpkg\installed\x64-windows
)

REM 创建构建目录
if not exist build (
    echo [信息] 创建构建目录...
    mkdir build
)

cd build

REM 配置 CMake
echo [信息] 配置 CMake...
cmake .. -G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=Release

if %ERRORLEVEL% neq 0 (
    echo [错误] CMake 配置失败
    cd ..
    exit /b 1
)

REM 构建项目
echo [信息] 开始构建...
cmake --build . --config Release -- /m:%NUMBER_OF_PROCESSORS%

if %ERRORLEVEL% neq 0 (
    echo [错误] 构建失败
    cd ..
    exit /b 1
)

echo.
echo ============================================
echo 构建成功!
echo 可执行文件：build\Release\PetEdu.exe
echo ============================================

cd ..
endlocal
