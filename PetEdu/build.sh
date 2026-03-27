#!/bin/bash
# ============================================
# PetEdu Linux 构建脚本
# ============================================

set -e

echo "============================================"
echo "PetEdu 构建脚本 (Linux)"
echo "============================================"
echo

# 检查 CMake 是否安装
if ! command -v cmake &> /dev/null; then
    echo "[错误] 未找到 CMake，请先安装 CMake 3.16+"
    echo "Ubuntu/Debian: sudo apt install cmake"
    echo "Fedora: sudo dnf install cmake"
    echo "Arch: sudo pacman -S cmake"
    exit 1
fi

# 检查 Qt5 是否安装
if ! pkg-config --exists Qt5Core; then
    echo "[警告] 未检测到 Qt5，请确保已安装 Qt5 Widgets 和 Sql 模块"
    echo "Ubuntu/Debian: sudo apt install qtbase5-dev qtbase5-dev-tools"
    echo "Fedora: sudo dnf install qt5-qtbase-devel"
    echo "Arch: sudo pacman -S qt5-base"
fi

# 创建构建目录
if [ ! -d "build" ]; then
    echo "[信息] 创建构建目录..."
    mkdir -p build
fi

cd build

# 配置 CMake
echo "[信息] 配置 CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local

if [ $? -ne 0 ]; then
    echo "[错误] CMake 配置失败"
    cd ..
    exit 1
fi

# 构建项目
echo "[信息] 开始构建..."
cmake --build . -j$(nproc)

if [ $? -ne 0 ]; then
    echo "[错误] 构建失败"
    cd ..
    exit 1
fi

echo
echo "============================================"
echo "构建成功!"
echo "可执行文件：build/PetEdu"
echo "安装：sudo cmake --install ."
echo "============================================"

cd ..
