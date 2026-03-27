# PetEdu - 电子宠物应用程序

🐾 一款基于 Qt5 的桌面端电子宠物应用

## 📋 功能介绍

PetEdu 是一个跨平台的电子宠物桌面应用，提供以下核心功能：

- 🐶 **宠物管理**: 创建、养育多个电子宠物
- 📊 **成长系统**: 宠物等级、经验值、属性成长
- 🎮 **互动玩法**: 喂食、玩耍、训练等互动功能
- 💾 **数据持久化**: 使用 SQLite 存储宠物数据
- 🎨 **精美界面**: 现代化 UI 设计，支持深色主题

## 🏗️ 项目结构

```
PetEdu/
├── CMakeLists.txt          # CMake 构建配置
├── README.md               # 项目说明文档
├── build.bat               # Windows 构建脚本
├── build.sh                # Linux 构建脚本
├── src/
│   ├── main.cpp            # 应用程序入口
│   ├── core/               # 核心逻辑模块 (cpp_dev 负责)
│   │   ├── pet.h           # 宠物类定义
│   │   ├── pet.cpp
│   │   ├── pet_manager.h   # 宠物管理器
│   │   ├── pet_manager.cpp
│   │   └── database.h      # 数据库操作
│   │   └── database.cpp
│   └── ui/                 # 界面模块 (qt_dev 负责)
│       ├── mainwindow.h    # 主窗口
│       ├── mainwindow.cpp
│       ├── pet_widget.h    # 宠物显示控件
│       └── pet_widget.cpp
└── resources/
    ├── images/             # 图片资源
    └── resources.qrc       # Qt 资源文件
```

## 🔧 环境要求

### 必需软件

- **CMake**: 3.16 或更高版本
- **C++ 编译器**: 
  - Windows: MSVC 2019+ 或 MinGW-w64
  - Linux: GCC 9+ 或 Clang 10+
- **Qt**: 5.15 或更高版本 (需要 Widgets 和 Sql 模块)

### 可选工具

- **Qt Creator**: 推荐的 IDE
- **vcpkg/conan**: 包管理器 (可选)

## 🚀 构建说明

### Windows

```batch
# 使用构建脚本
build.bat

# 或手动构建
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

### Linux

```bash
# 使用构建脚本
chmod +x build.sh
./build.sh

# 或手动构建
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### 构建选项

| 选项 | 说明 | 默认值 |
|------|------|--------|
| `CMAKE_BUILD_TYPE` | 构建类型 (Release/Debug) | Release |
| `CMAKE_INSTALL_PREFIX` | 安装路径 | `/usr/local` |
| `BUILD_TESTING` | 是否构建测试 | OFF |

## 📖 使用说明

### 启动应用

```bash
# Windows
.\build\Release\PetEdu.exe

# Linux
./build/PetEdu
```

### 基本操作

1. **创建宠物**: 点击"新建宠物"按钮
2. **喂食**: 选择食物类型进行喂食
3. **玩耍**: 选择玩具与宠物互动
4. **查看状态**: 在状态面板查看宠物属性

## 👥 开发团队

| 角色 | 负责人 | 职责 |
|------|--------|------|
| Tech Lead | 墨工 | 架构设计、任务分配 |
| C++ Dev | cpp_dev | 核心算法、数据结构 |
| Qt Dev | qt_dev | UI 设计、交互实现 |
| DevOps | devops | 构建配置、CI/CD |
| QA | qa | 测试用例、质量保障 |

## 📄 许可证

本项目采用 MIT 许可证 - 详见 LICENSE 文件

## 📞 联系方式

- GitHub: [待配置]
- 问题反馈: [待配置]

---

**版本**: 1.0.0  
**最后更新**: 2026-03-26
