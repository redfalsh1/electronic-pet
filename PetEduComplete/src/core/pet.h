#ifndef PET_H
#define PET_H

#include <string>
#include <memory>
#include <chrono>
#include <functional>

namespace pet {

// 宠物成长阶段
enum class GrowthStage {
    Egg,        // 蛋
    Baby,       // 幼年期
    Child,      // 童年期
    Teenager,   // 青少年期
    Adult,      // 成年期
    Elder       // 老年期
};

// 宠物状态结构
struct PetState {
    int level = 1;              // 等级
    int experience = 0;         // 经验值
    int maxExperience = 100;    // 升级所需最大经验
    GrowthStage stage = GrowthStage::Egg;  // 成长阶段
    int hunger = 100;           // 饥饿度 (0-100, 100 为饱腹)
    int mood = 100;             // 心情值 (0-100, 100 为开心)
    int health = 100;           // 健康值 (0-100, 100 为健康)
    std::chrono::system_clock::time_point lastFeedTime;   // 上次喂食时间
    std::chrono::system_clock::time_point lastPlayTime;   // 上次玩耍时间
    std::chrono::system_clock::time_point createTime;     // 创建时间
};

// 宠物类
class Pet {
public:
    // 静态方法获取单例
    static std::shared_ptr<Pet> getInstance();
    
    // 构造函数
    Pet();
    explicit Pet(const PetState& state);
    
    // 禁止拷贝
    Pet(const Pet&) = delete;
    Pet& operator=(const Pet&) = delete;
    
    // 允许移动
    Pet(Pet&&) = default;
    Pet& operator=(Pet&&) = default;
    
    // ========== 状态获取 ==========
    int getLevel() const { return state_.level; }
    int getExperience() const { return state_.experience; }
    int getMaxExperience() const { return state_.maxExperience; }
    GrowthStage getStage() const { return state_.stage; }
    int getHunger() const { return state_.hunger; }
    int getMood() const { return state_.mood; }
    int getHealth() const { return state_.health; }
    const PetState& getState() const { return state_; }
    
    // ========== 状态修改 ==========
    // 增加经验值，可能触发升级
    bool addExperience(int exp);
    
    // 喂食，减少饥饿度
    void feed(int foodAmount);
    
    // 玩耍，提升心情值
    void play(int joyAmount);
    
    // 休息，恢复健康值
    void rest(int healthRecovery);
    
    // ========== 时间相关 ==========
    // 更新随时间变化的状态（饥饿、心情等）
    void updateOverTime();
    
    // 检查并更新成长阶段
    void checkGrowthStage();
    
    // ========== 序列化/反序列化 ==========
    // 序列化为 JSON 字符串
    std::string serialize() const;
    
    // 从 JSON 字符串反序列化
    static std::shared_ptr<Pet> deserialize(const std::string& json);
    
    // 序列化为 PetState
    PetState toState() const { return state_; }
    
    // 从 PetState 加载
    void loadFromState(const PetState& state) { state_ = state; }
    
    // ========== 工具函数 ==========
    // 获取成长阶段名称
    static std::string getStageName(GrowthStage stage);
    
    // 判断是否存活
    bool isAlive() const { return state_.health > 0; }
    
    // 判断是否可以升级
    bool canLevelUp() const { return state_.experience >= state_.maxExperience; }

private:
    PetState state_;
    static std::weak_ptr<Pet> instance_;  // 单例弱引用
    
    // 升级处理
    void levelUp();
    
    // 计算成长阶段
    GrowthStage calculateStage() const;
};

// 成长阶段名称映射
inline std::string getGrowthStageName(GrowthStage stage) {
    switch (stage) {
        case GrowthStage::Egg: return "蛋";
        case GrowthStage::Baby: return "幼年期";
        case GrowthStage::Child: return "童年期";
        case GrowthStage::Teenager: return "青少年期";
        case GrowthStage::Adult: return "成年期";
        case GrowthStage::Elder: return "老年期";
        default: return "未知";
    }
}

} // namespace pet

#endif // PET_H
