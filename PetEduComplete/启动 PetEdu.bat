@echo off
chcp 65001 >nul
cd /d "%~dp0"
echo ========================================
echo PetEdu - 电子宠物教学系统
echo ========================================
echo.
echo 正在启动...
echo.
python petedu.py
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [错误] 程序运行失败
    echo.
    echo 可能原因：
    echo 1. 未安装 Python - 请访问 https://www.python.org/
    echo 2. PyQt5 未安装 - 正在自动安装...
    echo.
    pip install PyQt5
    echo.
    echo 安装完成，请重新运行本脚本
    pause
)
