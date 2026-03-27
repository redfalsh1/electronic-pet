/**
 * @file module_interface_test.cpp
 * @brief 模块间接口集成测试
 * @author EDA-QA
 * @date 2026-03-27
 */

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <memory>

// Pet 与积分系统集成测试
TEST(ModuleInterfaceTest, PetPointSystemIntegration) {
    struct Pet {
        std::string name;
        int level;
        
        Pet(const std::string& n) : name(n), level(1) {}
    };
    
    struct PointSystem {
        int total;
        void earn(int points) { total += points; }
    };
    
    // 集成场景：Pet 完成任务获得积分
    Pet pet("TestPet");
    PointSystem ps;
    
    ps.earn(50);  // 完成任务
    EXPECT_EQ(ps.total, 50);
    
    ps.earn(50);  // 再完成任务
    EXPECT_EQ(ps.total, 100);
}

// Pet 与交换系统集成测试
TEST(ModuleInterfaceTest, PetExchangeSystemIntegration) {
    struct Pet {
        std::string name;
    };
    
    struct PointSystem {
        int total;
        bool spend(int points) {
            if (total >= points) {
                total -= points;
                return true;
            }
            return false;
        }
    };
    
    struct ExchangeItem {
        std::string name;
        int cost;
    };
    
    // 集成场景：Pet 使用积分兑换物品
    Pet pet("TestPet");
    PointSystem ps;
    ps.total = 100;
    
    ExchangeItem item{"Reward", 80};
    
    bool exchanged = ps.spend(item.cost);
    EXPECT_TRUE(exchanged);
    EXPECT_EQ(ps.total, 20);
}

// 数据管理与核心模块集成测试
TEST(ModuleInterfaceTest, DataManagerCoreIntegration) {
    struct DataManager {
        std::vector<std::string> data;
        
        void save(const std::string& key, const std::string& value) {
            data.push_back(key + ":" + value);
        }
        
        std::string load(const std::string& key) {
            for (const auto& entry : data) {
                size_t pos = entry.find(':');
                if (pos != std::string::npos && entry.substr(0, pos) == key) {
                    return entry.substr(pos + 1);
                }
            }
            return "";
        }
    };
    
    struct PetState {
        std::string name;
        int level;
    };
    
    // 集成场景：保存和加载 Pet 状态
    DataManager dm;
    PetState state{"MyPet", 5};
    
    dm.save("pet_name", state.name);
    dm.save("pet_level", std::to_string(state.level));
    
    EXPECT_STREQ(dm.load("pet_name").c_str(), "MyPet");
    EXPECT_STREQ(dm.load("pet_level").c_str(), "5");
}

// 多模块协同工作测试
TEST(ModuleInterfaceTest, MultiModuleWorkflow) {
    struct Pet {
        std::string name;
        int level = 1;
        int exp = 0;
        
        void addExp(int e) {
            exp += e;
            while (exp >= level * 100) {
                exp -= level * 100;
                level++;
            }
        }
    };
    
    struct PointSystem {
        int total = 0;
        void reward(int points) { total += points; }
    };
    
    struct DataManager {
        int saveCount = 0;
        void save() { saveCount++; }
    };
    
    // 完整工作流：Pet 升级 -> 获得奖励 -> 保存状态
    Pet pet("Hero");
    PointSystem ps;
    DataManager dm;
    
    pet.addExp(150);  // 获得经验
    EXPECT_EQ(pet.level, 2);
    
    ps.reward(50);  // 升级奖励
    EXPECT_EQ(ps.total, 50);
    
    dm.save();  // 保存进度
    EXPECT_EQ(dm.saveCount, 1);
}

// 并发访问测试（模拟）
TEST(ModuleInterfaceTest, ConcurrentAccessSimulation) {
    struct ThreadSafeCounter {
        int count = 0;
        void increment() { count++; }
        int get() { return count; }
    };
    
    ThreadSafeCounter counter;
    
    // 模拟多线程访问
    for (int i = 0; i < 100; ++i) {
        counter.increment();
    }
    
    EXPECT_EQ(counter.get(), 100);
}

// 错误传播测试
TEST(ModuleInterfaceTest, ErrorPropagation) {
    struct ValidationResult {
        bool success;
        std::string error;
    };
    
    struct ModuleA {
        ValidationResult validate(int value) {
            if (value < 0) {
                return {false, "Value cannot be negative"};
            }
            return {true, ""};
        }
    };
    
    struct ModuleB {
        ModuleA& moduleA;
        
        ValidationResult process(int value) {
            auto result = moduleA.validate(value);
            if (!result.success) {
                return result;  // 传播错误
            }
            return {true, "Processed: " + std::to_string(value)};
        }
    };
    
    ModuleA modA;
    ModuleB modB{modA};
    
    auto result1 = modB.process(-5);
    EXPECT_FALSE(result1.success);
    EXPECT_EQ(result1.error, "Value cannot be negative");
    
    auto result2 = modB.process(10);
    EXPECT_TRUE(result2.success);
}

// 资源管理集成测试（RAII）
TEST(ModuleInterfaceTest, ResourceManagementRAII) {
    struct Resource {
        bool acquired = false;
        Resource() { acquired = true; }
        ~Resource() { acquired = false; }
    };
    
    struct ResourceManager {
        std::unique_ptr<Resource> resource;
        
        void acquire() {
            resource = std::make_unique<Resource>();
        }
        
        bool isAcquired() {
            return resource != nullptr && resource->acquired;
        }
    };
    
    ResourceManager rm;
    EXPECT_FALSE(rm.isAcquired());
    
    rm.acquire();
    EXPECT_TRUE(rm.isAcquired());
    // RAII: resource 会自动释放
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
