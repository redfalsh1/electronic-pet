# 快速开始指南

## 1. 准备图片资源

在 `images/` 目录创建占位图片（或使用真实资源）：

### PowerShell 快速创建占位图
```powershell
cd images

# 使用 ImageMagick 创建占位图 (需要先安装)
convert -size 300x300 xc:#2d2d2d -fill #4a9eff -gravity center -pointsize 40 -annotate 0 "PET" pet_placeholder.png
convert -size 64x64 xc:#2d2d2d -fill #90EE90 -gravity center -pointsize 12 -annotate 0 "FOOD" food_placeholder.png
convert -size 64x64 xc:#2d2d2d -fill #ffd700 -gravity center -pointsize 12 -annotate 0 "PROP" prop_placeholder.png
```

### 或手动创建
暂时可以跳过图片，程序会使用文字占位符。

## 2. 编译项目

```bash
# 进入项目目录
cd ui/pet-desktop

# 创建构建目录
mkdir build
cd build

# 配置 (Windows + Qt5.15 MSVC2019)
cmake .. -G "Visual Studio 16 2019" -A x64 -DCMAKE_PREFIX_PATH="C:/Qt/5.15.2/msvc2019_64"

# 或者使用 Qt Creator 打开 CMakeLists.txt

# 编译
cmake --build . --config Release
```

## 3. 运行程序

```bash
# Windows
cd Release
PetDesktop.exe

# Linux
./PetDesktop
```

## 4. 集成到主项目

### 方式 1：作为子模块
```cmake
# 在主项目 CMakeLists.txt 中添加
add_subdirectory(ui/pet-desktop)
target_link_libraries(YourMainApp PRIVATE PetDesktop)
```

### 方式 2：复制源文件
将 `.h` 和 `.cpp` 文件复制到主项目的 `src/ui/` 目录。

## 5. 后端对接示例

```cpp
// 主程序中连接后端
MainWindow *mainWin = new MainWindow();

// 连接后端数据源
connect(backend, &Backend::petDataUpdated, mainWin, &MainWindow::updatePetStatus);
connect(backend, &Backend::growthUpdated, mainWin, &MainWindow::updateGrowthProgress);

// 处理兑换请求
connect(mainWin, &MainWindow::requestExchange, backend, &Backend::processExchange);

mainWin->show();
```

## 常见问题

### Q: 编译时找不到 Qt
A: 确保 `CMAKE_PREFIX_PATH` 指向 Qt 安装目录，如 `C:/Qt/5.15.2/msvc2019_64`

### Q: 图片不显示
A: 检查 `resources.qrc` 中的路径是否正确，运行 `rcc` 重新编译资源

### Q: 样式不生效
A: 确保在 `main()` 中加载了样式表：
```cpp
QFile styleFile(":styles/dark_theme.qss");
styleFile.open(QFile::ReadOnly);
qApp->setStyleSheet(styleFile.readAll());
```

---

**开发完成时间**: 45 分钟内 ✅
**交付物**: 全部完成
