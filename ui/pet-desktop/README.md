# EDA 电子宠物 - Qt 界面模块

## 模块概述

本模块实现了电子宠物课堂激励系统的完整 UI 界面，基于 Qt5.15+ Widgets。

## 功能清单

### 1. MainWindow (主窗口)
- **宠物显示区域**: QLabel + 图片展示
- **状态信息**: 积分/粮食/经验值实时显示
- **成长进度条**: QProgressBar 可视化成长进度
- **视图切换**: 教师面板/学生视图切换

### 2. TeacherPanel (教师面板)
- **学生列表**: 可视化学生名单管理
- **积分发放**: 支持答题/作业/表现等多种原因
- **奖励发放**: 宠物粮食/装饰道具等奖励类型
- **积分排行**: 学生积分表格展示

### 3. StudentView (学生视图)
- **宠物状态**: 宠物图片/名称/等级展示
- **积分兑换**: 进入兑换商城
- **成长记录**: 历史积分获得记录表格

### 4. ExchangeDialog (兑换对话框)
- **粮食购买**: 多种粮食物品选择
- **道具购买**: 装饰/经验/特殊道具
- **数量选择**: 批量购买支持
- **确认/取消**: 二次确认机制

## 技术特性

- ✅ Qt5.15+ Widgets
- ✅ 深色主题样式表
- ✅ 响应式布局设计
- ✅ 信号槽机制与后端对接
- ✅ 高 DPI 支持
- ✅ 模块化设计

## 文件结构

```
ui/pet-desktop/
├── mainwindow.h          # 主窗口头文件
├── mainwindow.cpp        # 主窗口实现
├── teacherpanel.h        # 教师面板头文件
├── teacherpanel.cpp      # 教师面板实现
├── studentview.h         # 学生视图头文件
├── studentview.cpp       # 学生视图实现
├── exchangedialog.h      # 兑换对话框头文件
├── exchangedialog.cpp    # 兑换对话框实现
├── resources.qrc         # Qt 资源文件
├── CMakeLists.txt        # CMake 构建配置
├── README.md             # 本说明文档
├── images/               # 图片资源目录
│   └── README.md         # 图片资源说明
└── styles/               # 样式表目录
    └── dark_theme.qss    # 深色主题样式
```

## 编译说明

### 前置要求
- Qt 5.15 或更高版本
- CMake 3.16 或更高版本
- C++17 兼容编译器 (MSVC2019+, GCC 9+, Clang 10+)

### 构建步骤

```bash
# 创建构建目录
mkdir build && cd build

# 配置项目
cmake .. -DCMAKE_BUILD_TYPE=Release

# 编译
cmake --build . --config Release

# 运行
./PetDesktop  # Linux
PetDesktop.exe  # Windows
```

### Qt 环境配置

**Windows (MSVC)**:
```bash
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Release
```

**Linux**:
```bash
export QT_DIR=/opt/Qt5.15.2/5.15.2/gcc_64
cmake .. -DCMAKE_PREFIX_PATH=$QT_DIR
```

## 与后端对接

### 信号列表

#### MainWindow 发出的信号
```cpp
void requestPetData();              // 请求宠物数据
void requestExchange(int, int);     // 请求兑换
void switchToTeacherView();         // 切换到教师视图
void switchToStudentView();         // 切换到学生视图
```

#### TeacherPanel 发出的信号
```cpp
void awardPoints(const QString&, int, const QString&);  // 发放积分
void awardReward(const QString&, const QString&);       // 发放奖励
void operationComplete();                               // 操作完成
```

#### StudentView 发出的信号
```cpp
void requestExchange(int, int);     // 请求兑换
void operationComplete();           // 操作完成
```

### 槽函数接口

#### MainWindow 提供的接口
```cpp
void updatePetStatus(const QString&, int, int, int, int);  // 更新宠物状态
void updateGrowthProgress(int, int);                        // 更新成长进度
```

#### StudentView 提供的接口
```cpp
void updatePetInfo(const QString&, const QString&, int);   // 更新宠物信息
void updateStatus(int, int, int, int);                      // 更新状态
void updateGrowthRecord(const QList<QString>&);             // 更新成长记录
```

#### TeacherPanel 提供的接口
```cpp
void addStudent(const QString&, const QString&);           // 添加学生
void removeStudent(const QString&);                         // 移除学生
void updateStudentScore(const QString&, int);               // 更新学生积分
```

## 样式定制

### 修改主题颜色

编辑 `styles/dark_theme.qss`，主要颜色变量：
- `#1e1e1e` - 主背景色
- `#2d2d2d` - 次级背景色
- `#4a9eff` - 主色调 (蓝色)
- `#ffd700` - 强调色 (金色)
- `#90EE90` - 成功色 (绿色)
- `#87CEEB` - 信息色 (天蓝)

### 添加新样式类

在样式表末尾添加：
```css
.CustomClass {
    background-color: #xxx;
    color: #yyy;
}
```

在代码中应用：
```cpp
widget->setObjectName("CustomClass");
```

## 待办事项

- [ ] 替换占位图片为正式 UI 资源
- [ ] 添加动画效果 (宠物成长/升级)
- [ ] 实现宠物喂养交互
- [ ] 添加音效支持
- [ ] 多语言支持 (i18n)
- [ ] 自适应窗口大小优化

## 开发者

**eda_qt_dev** - Qt 界面工程师
完成时间：2026-03-26

## 许可证

项目内部使用，未经许可不得外传。
