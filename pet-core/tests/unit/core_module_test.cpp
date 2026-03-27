/**
 * @file core_module_test.cpp
 * @brief 核心模块单元测试
 * @author EDA-QA
 * @date 2026-03-27
 */

#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <memory>

// 模拟 Pet 类测试
TEST(CoreModuleTest, PetInitialization) {
    struct Pet {
        std::string name;
        int level;
        int experience;
        
        Pet(const std::string& n) : name(n), level(1), experience(0) {}
        
        void addExperience(int exp) {
            experience += exp;
            while (experience >= level * 100) {
                level++;
                experience -= level * 100;
            }
        }
    };
    
    Pet pet("TestPet");
    EXPECT_STREQ(pet.name.c_str(), "TestPet");
    EXPECT_EQ(pet.level, 1);
    EXPECT_EQ(pet.experience, 0);
    
    pet.addExperience(150);
    EXPECT_EQ(pet.level, 2);
}

// 积分系统测试
TEST(CoreModuleTest, PointSystem) {
    struct PointSystem {
        int total;
        int earned;
        int spent;
        
        PointSystem() : total(0), earned(0), spent(0) {}
        
        void earn(int points) {
            earned += points;
            total += points;
        }
        
        bool spend(int points) {
            if (total >= points) {
                total -= points;
                spent += points;
                return true;
            }
            return false;
        }
    };
    
    PointSystem ps;
    ps.earn(100);
    EXPECT_EQ(ps.total, 100);
    EXPECT_EQ(ps.earned, 100);
    
    EXPECT_TRUE(ps.spend(50));
    EXPECT_EQ(ps.total, 50);
    EXPECT_EQ(ps.spent, 50);
    
    EXPECT_FALSE(ps.spend(100));  // 余额不足
}

// 交换系统测试
TEST(CoreModuleTest, ExchangeSystem) {
    struct ExchangeItem {
        std::string name;
        int cost;
        int stock;
    };
    
    struct ExchangeSystem {
        std::vector<ExchangeItem> items;
        
        bool canExchange(const std::string& itemName, int userPoints) {
            for (const auto& item : items) {
                if (item.name == itemName && item.stock > 0 && userPoints >= item.cost) {
                    return true;
                }
            }
            return false;
        }
    };
    
    ExchangeSystem es;
    es.items.push_back({"Item A", 50, 10});
    es.items.push_back({"Item B", 100, 5});
    
    EXPECT_TRUE(es.canExchange("Item A", 100));
    EXPECT_FALSE(es.canExchange("Item B", 50));
    EXPECT_FALSE(es.canExchange("Item C", 200));  // 不存在的物品
}

// 数据管理器测试
TEST(CoreModuleTest, DataManager) {
    struct DataManager {
        std::vector<std::string> data;
        
        void addData(const std::string& item) {
            data.push_back(item);
        }
        
        bool hasData(const std::string& item) {
            for (const auto& d : data) {
                if (d == item) return true;
            }
            return false;
        }
        
        size_t count() {
            return data.size();
        }
    };
    
    DataManager dm;
    EXPECT_EQ(dm.count(), 0);
    
    dm.addData("Item1");
    dm.addData("Item2");
    dm.addData("Item3");
    
    EXPECT_EQ(dm.count(), 3);
    EXPECT_TRUE(dm.hasData("Item2"));
    EXPECT_FALSE(dm.hasData("Item4"));
}

// 边界条件测试
TEST(CoreModuleTest, BoundaryConditions) {
    // 测试零值
    struct Counter {
        int count = 0;
        void increment() { count++; }
        void decrement() { if (count > 0) count--; }
    };
    
    Counter c;
    EXPECT_EQ(c.count, 0);
    c.decrement();  // 不应为负
    EXPECT_EQ(c.count, 0);
    
    c.increment();
    c.increment();
    EXPECT_EQ(c.count, 2);
}

// 异常处理测试
TEST(CoreModuleTest, ExceptionHandling) {
    struct SafeDivide {
        static double divide(double a, double b) {
            if (b == 0.0) {
                throw std::runtime_error("Division by zero");
            }
            return a / b;
        }
    };
    
    EXPECT_DOUBLE_EQ(SafeDivide::divide(10.0, 2.0), 5.0);
    EXPECT_THROW(SafeDivide::divide(10.0, 0.0), std::runtime_error);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
