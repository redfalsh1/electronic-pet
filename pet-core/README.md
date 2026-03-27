# Pet Core - 电子宠物核心逻辑库

## 概述

Pet Core 是一个 C++17 电子宠物游戏的核心逻辑库，提供宠物状态管理、积分系统、兑换系统和数据持久化功能。

## 功能模块

### 1. Pet 类 (`pet.h` / `pet.cpp`)

宠物状态管理核心类：

- **成长系统**: 等级、经验值、6 个成长阶段（蛋→幼年→童年→青少年→成年→老年）
- **状态管理**: 饥饿度、心情值、健康值（0-100）
- **时间系统**: 随时间自动衰减的饥饿和心情
- **序列化**: JSON 格式的序列化/反序列化支持

```cpp
auto pet = pet::Pet::getInstance();
pet->addExperience(50);      // 增加经验
pet->feed(20);               // 喂食
pet->play(15);               // 玩耍
pet->updateOverTime();       // 更新时间相关状态
```

### 2. PointSystem 类 (`pointsystem.h` / `pointsystem.cpp`)

积分管理系统（单例模式）：

- **积分类型**: 答题积分、作业积分、表现积分、奖励积分
- **历史记录**: 完整的积分流水记录
- **统计查询**: 今日积分、类型统计、最近记录
- **线程安全**: 内置互斥锁保护

```cpp
auto points = pet::PointSystem::getInstance();
points->addQuizPoints(10, "答对 5 道题");
points->addHomeworkPoints(20, "完成作业");
int total = points->getTotalPoints();
```

### 3. ExchangeSystem 类 (`exchangesystem.h` / `exchangesystem.cpp`)

兑换系统（单例模式）：

- **兑换规则**: 积分→粮食、粮食→经验、积分→道具
- **限制管理**: 每日限制、总限制
- **回调机制**: 可设置积分扣除、粮食增加、经验增加回调
- **规则配置**: 动态添加、修改、启用/禁用规则

```cpp
auto exchange = pet::ExchangeSystem::getInstance();
exchange->setPointsDeductCallback([](int points, const std::string& desc) {
    // 扣除积分逻辑
    return true;  // 返回是否成功
});
exchange->executeExchange("points_to_food_1");
```

### 4. DataManager 类 (`datamanager.h` / `datamanager.cpp`)

数据持久化管理（单例模式）：

- **文件存储**: 基于文件的持久化（可轻松迁移到 SQLite）
- **自动保存**: 可配置间隔的自动保存
- **备份恢复**: 自动备份和手动恢复功能
- **数据导出**: JSON 格式的全量导出/导入

```cpp
auto manager = pet::DataManager::getInstance();
manager->initialize("data/pet_data.db");
manager->setAutoSaveInterval(60);  // 60 秒自动保存
manager->savePet(pet);
manager->createBackup();  // 创建备份
```

## 编译说明

### 前置要求

- C++17 兼容编译器（GCC 9+, Clang 9+, MSVC 2019+）
- CMake 3.15+

### 编译步骤

```bash
# 创建构建目录
mkdir build && cd build

# 配置
cmake .. -DCMAKE_BUILD_TYPE=Release

# 编译
cmake --build .

# 安装（可选）
cmake --install . --prefix /usr/local
```

### 使用示例

```cmake
find_package(pet_core REQUIRED)
target_link_libraries(your_target PRIVATE pet_core::pet_core)
```

## 项目结构

```
pet-core/
├── pet.h                 # Pet 类头文件
├── pet.cpp               # Pet 类实现
├── pointsystem.h         # PointSystem 类头文件
├── pointsystem.cpp       # PointSystem 类实现
├── exchangesystem.h      # ExchangeSystem 类头文件
├── exchangesystem.cpp    # ExchangeSystem 类实现
├── datamanager.h         # DataManager 类头文件
├── datamanager.cpp       # DataManager 类实现
├── CMakeLists.txt        # CMake 构建配置
├── cmake/
│   └── pet_core-config.cmake.in  # CMake 配置模板
└── README.md             # 本文件
```

## 设计特点

### 1. 单例模式
PointSystem、ExchangeSystem、DataManager 均采用单例模式，确保全局状态一致性。

### 2. 智能指针
全程使用 `std::shared_ptr` 和 `std::weak_ptr`，避免内存泄漏。

### 3. RAII 原则
资源获取即初始化，确保异常安全。

### 4. 线程安全
所有单例类内置 `std::mutex` 保护，支持多线程访问。

### 5. 可扩展性
- 回调机制支持自定义行为
- 兑换规则可动态配置
- 数据层可轻松迁移到 SQLite

## 使用示例

```cpp
#include "pet.h"
#include "pointsystem.h"
#include "exchangesystem.h"
#include "datamanager.h"

int main() {
    // 初始化数据管理器
    auto manager = pet::DataManager::getInstance();
    manager->initialize("data/pet_data.db");
    
    // 获取或创建宠物
    auto pet = pet::Pet::getInstance();
    
    // 加载已保存的宠物数据
    if (manager->hasPetData()) {
        auto loadedPet = manager->loadPet();
        if (loadedPet) {
            pet = loadedPet;
        }
    }
    
    // 获取积分系统
    auto points = pet::PointSystem::getInstance();
    
    // 获取兑换系统
    auto exchange = pet::ExchangeSystem::getInstance();
    
    // 设置兑换回调
    exchange->setPointsDeductCallback([points](int pts, const std::string& desc) {
        return points->deductPoints(pts, desc);
    });
    
    // 游戏循环
    while (true) {
        // 用户答题获得积分
        points->addQuizPoints(10, "答对题目");
        
        // 用户兑换粮食
        int foodAmount = 0;
        exchange->exchangePointsToFood(100, foodAmount);
        
        // 喂食宠物
        pet->feed(foodAmount);
        
        // 保存数据
        manager->savePet(pet);
        
        // 更新时间状态
        pet->updateOverTime();
    }
    
    return 0;
}
```

## 许可证

MIT License

## 版本历史

### v1.0.0 (2026-03-26)
- 初始版本
- 实现 Pet、PointSystem、ExchangeSystem、DataManager 四个核心类
- 支持 C++17 标准
- 提供完整的序列化/反序列化功能
- 支持自动保存和备份恢复
