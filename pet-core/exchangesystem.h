#ifndef EXCHANGESYSTEM_H
#define EXCHANGESYSTEM_H

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <mutex>
#include <functional>

namespace pet {

// 兑换类型
enum class ExchangeType {
    PointsToFood,     // 积分 → 粮食
    FoodToExperience, // 粮食 → 经验值
    PointsToItem      // 积分 → 道具
};

// 兑换规则
struct ExchangeRule {
    std::string id;                    // 规则 ID
    ExchangeType type = ExchangeType::PointsToFood;  // 兑换类型
    std::string name;                  // 规则名称
    int cost = 0;                      // 消耗数量
    int reward = 0;                    // 获得数量
    std::string costUnit;              // 消耗单位（如"积分"、"粮食"）
    std::string rewardUnit;            // 获得单位（如"粮食"、"经验"）
    int dailyLimit = -1;               // 每日限制次数（-1 为无限制）
    int totalLimit = -1;               // 总限制次数（-1 为无限制）
    bool enabled = true;               // 是否启用
    std::string description;           // 描述
    
    // 序列化
    std::string serialize() const;
    // 反序列化
    static ExchangeRule deserialize(const std::string& str);
};

// 兑换记录
struct ExchangeRecord {
    int64_t id = 0;                      // 记录 ID
    std::string ruleId;                  // 使用的规则 ID
    ExchangeType type = ExchangeType::PointsToFood;  // 兑换类型
    int costAmount = 0;                  // 消耗数量
    int rewardAmount = 0;                // 获得数量
    std::chrono::system_clock::time_point timestamp;  // 时间戳
    bool success = true;                 // 是否成功
    
    // 序列化
    std::string serialize() const;
    // 反序列化
    static ExchangeRecord deserialize(const std::string& str);
};

// 兑换系统（单例模式）
class ExchangeSystem {
public:
    // 获取单例实例
    static std::shared_ptr<ExchangeSystem> getInstance();
    
    // 禁止拷贝和移动
    ExchangeSystem(const ExchangeSystem&) = delete;
    ExchangeSystem& operator=(const ExchangeSystem&) = delete;
    ExchangeSystem(ExchangeSystem&&) = delete;
    ExchangeSystem& operator=(ExchangeSystem&&) = delete;
    
    // ========== 初始化 ==========
    // 初始化默认兑换规则
    void initializeDefaultRules();
    
    // ========== 兑换操作 ==========
    // 执行兑换（返回是否成功）
    bool executeExchange(const std::string& ruleId);
    
    // 积分兑换粮食
    bool exchangePointsToFood(int points, int& foodAmount);
    
    // 粮食兑换经验值
    bool exchangeFoodToExperience(int foodAmount, int& expAmount);
    
    // ========== 规则管理 ==========
    // 添加兑换规则
    void addRule(const ExchangeRule& rule);
    
    // 移除兑换规则
    bool removeRule(const std::string& ruleId);
    
    // 更新兑换规则
    bool updateRule(const ExchangeRule& rule);
    
    // 获取所有规则
    std::vector<ExchangeRule> getAllRules() const;
    
    // 获取指定类型的规则
    std::vector<ExchangeRule> getRulesByType(ExchangeType type) const;
    
    // 获取指定 ID 的规则
    ExchangeRule getRule(const std::string& ruleId) const;
    
    // 启用/禁用规则
    bool setRuleEnabled(const std::string& ruleId, bool enabled);
    
    // ========== 查询 ==========
    // 获取所有兑换记录
    std::vector<ExchangeRecord> getAllRecords() const;
    
    // 获取最近 N 条记录
    std::vector<ExchangeRecord> getRecentRecords(size_t count) const;
    
    // 获取今日兑换次数（按规则 ID）
    int getTodayExchangeCount(const std::string& ruleId) const;
    
    // 获取总兑换次数（按规则 ID）
    int getTotalExchangeCount(const std::string& ruleId) const;
    
    // 检查是否可以兑换（不执行）
    bool canExchange(const std::string& ruleId, std::string& reason) const;
    
    // ========== 回调设置 ==========
    // 设置积分扣除回调
    using PointsDeductCallback = std::function<bool(int, const std::string&)>;
    void setPointsDeductCallback(PointsDeductCallback callback) {
        pointsDeductCallback_ = callback;
    }
    
    // 设置粮食增加回调
    using FoodAddCallback = std::function<void(int)>;
    void setFoodAddCallback(FoodAddCallback callback) {
        foodAddCallback_ = callback;
    }
    
    // 设置经验值增加回调
    using ExpAddCallback = std::function<void(int)>;
    void setExpAddCallback(ExpAddCallback callback) {
        expAddCallback_ = callback;
    }
    
    // ========== 序列化 ==========
    // 序列化所有数据
    std::string serialize() const;
    
    // 反序列化
    void deserialize(const std::string& json);

private:
    ExchangeSystem();
    
    mutable std::mutex mutex_;                      // 线程安全锁
    std::map<std::string, ExchangeRule> rules_;     // 兑换规则
    std::vector<ExchangeRecord> records_;           // 兑换记录
    int64_t nextRecordId_ = 1;                      // 下一条记录 ID
    
    // 每日兑换计数（ruleId -> count）
    std::map<std::string, int> todayExchangeCount_;
    std::chrono::system_clock::time_point todayStart_;
    
    // 总兑换计数（ruleId -> count）
    std::map<std::string, int> totalExchangeCount_;
    
    // 回调函数
    PointsDeductCallback pointsDeductCallback_;
    FoodAddCallback foodAddCallback_;
    ExpAddCallback expAddCallback_;
    
    static std::weak_ptr<ExchangeSystem> instance_;  // 单例弱引用
    
    // 检查并更新每日计数
    void checkAndUpdateTodayCount();
    
    // 添加兑换记录
    void addRecord(const ExchangeRecord& record);
};

// 兑换类型名称映射
inline std::string getExchangeTypeName(ExchangeType type) {
    switch (type) {
        case ExchangeType::PointsToFood: return "积分→粮食";
        case ExchangeType::FoodToExperience: return "粮食→经验";
        case ExchangeType::PointsToItem: return "积分→道具";
        default: return "未知";
    }
}

} // namespace pet

#endif // EXCHANGESYSTEM_H
