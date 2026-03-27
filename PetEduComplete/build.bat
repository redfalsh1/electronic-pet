@echo off
REM PetEdu Windows Build Script
REM 要求：已安装 Qt5 和 CMake

echo ========================================
echo PetEdu - 电子宠物教学系统 构建脚本
echo ========================================
echo.

REM 检查 CMake
where cmake >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [错误] 未找到 CMake，请先安装 CMake 3.16+
    pause
    exit /b 1
)

REM 检查 Qt 环境变量
if "%QT5_DIR%"=="" (
    echo [提示] QT5_DIR 环境变量未设置，尝试使用默认路径...
)

REM 创建构建目录
if not exist build mkdir build
cd build

echo [1/3] 配置项目...
cmake .. -G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=Release
if %ERRORLEVEL% NEQ 0 (
    echo [错误] CMake 配置失败
    cd ..
    pause
    exit /b 1
)

echo.
echo [2/3] 编译项目...
cmake --build . --config Release
if %ERRORLEVEL% NEQ 0 (
    echo [错误] 编译失败
    cd ..
    pause
    exit /b 1
)

echo.
echo [3/3] 构建完成！
echo.
echo 可执行文件位置：build\Release\PetEdu.exe
echo.
echo 是否运行程序？(Y/N)
set /p RUN_APP=
if /i "%RUN_APP%"=="Y" (
    start Release\PetEdu.exe
)

cd ..
pause
