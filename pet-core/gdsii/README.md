# GDSII 解析器模块

## 概述

GDSII 解析器是一个高性能、流式的 GDSII 二进制文件格式解析库，用于 EDA 软件中读取和处理集成电路版图数据。

## 特性

- ✅ **C++17 标准**：使用现代 C++ 特性
- ✅ **智能指针**：使用 `std::unique_ptr` 和 `std::shared_ptr` 管理内存
- ✅ **流式解析**：支持大文件流式处理，避免一次性加载到内存
- ✅ **异常安全**：完善的异常处理机制
- ✅ **图层过滤**：支持按图层号和数据类型过滤元素
- ✅ **几何元素**：支持 Boundary、Path、Text 三种基本元素

## 文件结构

```
gdsii/
├── gdsii_types.h      # 数据类型定义
├── gdsii_parser.h     # 解析器接口
├── gdsii_parser.cpp   # 解析器实现
└── README.md          # 本文档
```

## 快速开始

### 基本用法

```cpp
#include "gdsii/gdsii_parser.h"

using namespace gdsii;

int main() {
    GdsiiParser parser;
    
    // 解析文件
    auto library = parser.parse("design.gds");
    
    // 访问库信息
    std::cout << "库名称：" << library->name << std::endl;
    std::cout << "结构数量：" << library->structures.size() << std::endl;
    
    // 遍历结构
    for (const auto& structure : library->structures) {
        std::cout << "结构：" << structure.name << std::endl;
        
        // 遍历元素
        for (const auto& element : structure.elements) {
            // 使用 std::visit 处理变体
            std::visit([](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, Boundary>) {
                    std::cout << "  边界：图层 " << arg.layer 
                              << ", 点数 " << arg.points.size() << std::endl;
                } else if constexpr (std::is_same_v<T, Path>) {
                    std::cout << "  路径：图层 " << arg.layer 
                              << ", 宽度 " << arg.width << std::endl;
                } else if constexpr (std::is_same_v<T, Text>) {
                    std::cout << "  文本：" << arg.text << std::endl;
                }
            }, element);
        }
    }
    
    return 0;
}
```

### 图层过滤

```cpp
ParserOptions options;

// 只解析图层 1 和 3
options.filter.layers = std::vector<int16_t>{1, 3};

auto library = parser.parse("design.gds", options);
```

### 流式解析

```cpp
GdsiiParser parser;

parser.parseStream("design.gds",
    [](const Structure& structure, const GeometryElement& element) {
        // 每个元素被解析时调用
        std::cout << "解析元素：" << structure.name << std::endl;
    });
```

## API 参考

### GdsiiParser 类

#### parse()
```cpp
std::unique_ptr<Library> parse(const std::string& filename, 
                                const ParserOptions& options = ParserOptions());
```
解析 GDSII 文件并返回库对象。

#### parseStream()
```cpp
void parseStream(const std::string& filename,
                 std::function<void(const Structure&, const GeometryElement&)> elementCallback,
                 const ParserOptions& options = ParserOptions());
```
流式解析 GDSII 文件，每个元素解析完成后调用回调函数。

#### getLayerList()
```cpp
std::vector<LayerInfo> getLayerList(const Library& library) const;
```
获取库中的所有图层信息。

### ParserOptions 结构

```cpp
struct ParserOptions {
    bool streamMode = true;          // 流式解析模式
    LayerFilter filter;              // 图层过滤器
    bool collectLayerInfo = true;    // 收集图层信息
    bool verbose = false;            // 详细输出
};
```

### LayerFilter 结构

```cpp
struct LayerFilter {
    std::optional<std::vector<int16_t>> layers;      // 图层号过滤
    std::optional<std::vector<int16_t>> datatypes;   // 数据类型过滤
    
    bool isEmpty() const;                            // 是否为空过滤器
    bool matches(int16_t layer, int16_t datatype) const;  // 是否匹配
};
```

## 数据类型

### Library
GDSII 库，包含：
- `name`: 库名称
- `units`: 用户单位
- `precision`: 数据库精度（米）
- `structures`: 结构单元列表
- `layerInfos`: 图层信息列表

### Structure
结构单元，包含：
- `name`: 结构名称
- `elements`: 几何元素列表

### Boundary
边界（多边形），包含：
- `points`: 顶点列表
- `layer`: 图层号
- `datatype`: 数据类型

### Path
路径，包含：
- `points`: 路径点列表
- `layer`: 图层号
- `datatype`: 数据类型
- `pathtype`: 路径类型（0=flush, 1=round, 2=extended）
- `width`: 路径宽度

### Text
文本，包含：
- `text`: 文本内容
- `points`: 位置点列表
- `layer`: 图层号
- `texttype`: 文本类型
- `presentation`: 显示属性

## 编译

### 作为 pet_core 的一部分

GDSII 解析器已集成到 `pet_core` 库中：

```bash
cd pet-core
mkdir build && cd build
cmake .. -DPET_CORE_BUILD_TESTS=ON
cmake --build .
```

### 运行测试

```bash
cd build
ctest --output-on-failure
```

## 单元测试

测试文件：`tests/unit/gdsii_parser_test.cpp`

测试覆盖：
- ✅ 文件解析
- ✅ 图层过滤
- ✅ 边界元素
- ✅ 路径元素
- ✅ 文本元素
- ✅ 流式解析
- ✅ 异常处理
- ✅ 内存管理（智能指针）

### 测试覆盖率

目标：> 80%

运行覆盖率测试（需要 gcov/lcov）：
```bash
cmake .. -DPET_CORE_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build .
ctest
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage
```

## 性能考虑

### 流式解析
对于大型 GDSII 文件（> 100MB），建议使用流式解析模式：

```cpp
parser.parseStream("large_design.gds", callback);
```

### 内存管理
- 使用 `std::unique_ptr<Library>` 自动管理内存
- 几何元素使用 `std::variant` 存储，避免虚函数开销
- 流式解析时不保存所有元素到内存

## 异常处理

```cpp
try {
    auto library = parser.parse("file.gds");
} catch (const GdsiiException& e) {
    std::cerr << "解析错误：" << e.what() << std::endl;
}
```

## GDSII 格式参考

GDSII 是一种二进制文件格式，用于集成电路版图数据交换。

### 记录结构
```
+--------+--------+--------+--------+
| 数据长度 (16 位) | 记录类型 (16 位) |
+--------+--------+--------+--------+
|           数据 (变长)              |
+--------+--------+--------+--------+
```

### 基本元素类型
1. **Boundary**: 多边形区域
2. **Path**: 线段路径
3. **Text**: 文本标签
4. **SREF**: 结构引用（单次）
5. **AREF**: 结构引用（阵列）

## 版本历史

- **v1.0.0** (2026-03-27): 初始版本
  - 支持 Boundary、Path、Text 解析
  - 图层过滤功能
  - 流式解析
  - 单元测试

## 作者

EDA-CPP-Dev <cpp-dev@eda-team.local>

## 许可证

与 pet_core 项目相同
