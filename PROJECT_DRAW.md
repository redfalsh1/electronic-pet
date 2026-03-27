# PROJECT_DRAW.md - 基础绘画软件

## 项目概述
- **项目名称**: SimplePaint - 基础绘画工具
- **技术栈**: C++17, Qt5.15+, CMake
- **目标平台**: Windows 10/11, Linux
- **开发周期**: 1 周 Sprint

## 功能需求

### 1. 基础绘画功能
- 自由手绘（鼠标/触摸板）
- 直线绘制
- 矩形绘制
- 椭圆绘制
- 橡皮擦功能

### 2. 画笔设置
- 画笔粗细调节（1-50px）
- 颜色选择器
- 填充/描边模式切换

### 3. 画布操作
- 新建画布（自定义尺寸）
- 清空画布
- 保存为 PNG/JPG/BMP
- 打开图片文件

### 4. 界面布局
- 工具栏（画笔、直线、矩形、椭圆、橡皮擦）
- 属性面板（粗细、颜色）
- 主画布区域
- 状态栏（坐标、画布尺寸）

## 技术架构
```
┌─────────────────────────────────────┐
│           UI Layer (Qt)             │
│  ┌─────────┐ ┌─────────┐ ┌────────┐ │
│  │Toolbar  │ │Canvas   │ │Property│ │
│  │(Tools)  │ │(Paint)  │ │ Panel  │ │
│  └─────────┘ └─────────┘ └────────┘ │
└─────────────────────────────────────┘
        │
┌───────┴─────────────────────────────┐
│        Core Logic Layer             │
│  ┌─────────┐ ┌─────────┐ ┌────────┐ │
│  │Canvas   │ │Shape    │ │Draw    │ │
│  │Manager  │ │Factory  │ │Engine  │ │
│  └─────────┘ └─────────┘ └────────┘ │
└─────────────────────────────────────┘
```

## Sprint 任务分配

### Phase 1: 核心框架
- [ ] C++ 核心：画布管理、形状基类
- [ ] Qt UI：主窗口、工具栏布局

### Phase 2: 绘图功能
- [ ] C++ 核心：绘图引擎、形状绘制
- [ ] Qt UI：Canvas 控件、鼠标事件处理

### Phase 3: 文件与优化
- [ ] C++ 核心：图片保存/加载
- [ ] Qt UI：颜色选择器、属性面板
- [ ] QA：功能测试

## 接口设计

### CanvasWidget (Qt)
```cpp
class CanvasWidget : public QWidget {
    Q_OBJECT
public:
    void setTool(ToolType tool);
    void setPenWidth(int width);
    void setPenColor(QColor color);
    void clear();
    bool saveImage(const QString& path);
    bool loadImage(const QString& path);
    
signals:
    void mousePositionChanged(QPoint pos);
    void imageModified();
};
```

### Shape 基类 (C++)
```cpp
class Shape {
public:
    virtual void draw(QPainter& painter) = 0;
    virtual QRect boundingRect() const = 0;
    virtual ShapeType type() const = 0;
};
```

## 文件结构
```
/data/draw-project/
├── src/
│   ├── core/           # C++ 核心层
│   │   ├── CanvasManager
│   │   ├── Shape.h/cpp
│   │   ├── FreehandShape
│   │   ├── LineShape
│   │   ├── RectShape
│   │   └── EllipseShape
│   │
│   └── ui/             # Qt UI 层
│       ├── MainWindow
│       ├── CanvasWidget
│       ├── Toolbar
│       ├── PropertyPanel
│       └── ColorPicker
│
├── CMakeLists.txt
└── resources/
    └── icons/
```
