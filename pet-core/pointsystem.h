#ifndef POINTSYSTEM_H
#define POINTSYSTEM_H

#include <string>
#include <memory>
#include <vector>
#include <chrono>
#include <mutex>
#include <cstdint>

namespace pet {

// 积分类型
enum class PointType {
    Quiz,           // 答题积分
    Homework,       // 作业积分
    Performance,    // 表现积分
    Exchange,       // 兑换支出
    Reward          // 奖励积分
};

// 积分记录
struct PointRecord {
    int64_t id = 0;                     // 记录 ID
    PointType type = PointType::Reward; // 积分类型
    int points = 0;                      // 积分数量（正数为收入，负数为支出）
    std::string description;             // 描述
    std::chrono::system_clock::time_point timestamp;  // 时间戳
    
    // 序列化为字符串
    std::string serialize() const;
    // 从字符串反序列化
    static PointRecord deserialize(const std::string& str);
};

// 积分系统（单例模式）
class PointSystem {
public:
    // 获取单例实例
    static std::shared_ptr<PointSystem> getInstance();
    
    // 禁止拷贝和移动
    PointSystem(const PointSystem&) = delete;
    PointSystem& operator=(const PointSystem&) = delete;
    PointSystem(PointSystem&&) = delete;
    PointSystem& operator=(PointSystem&&) = delete;
    
    // ========== 积分操作 ==========
    // 添加答题积分
    int addQuizPoints(int points, const std::string& description = "答题奖励");
    
    // 添加作业积分
    int addHomeworkPoints(int points, const std::string& description = "作业奖励");
    
    // 添加表现积分
    int addPerformancePoints(int points, const std::string& description = "表现奖励");
    
    // 添加奖励积分（通用）
    int addRewardPoints(int points, const std::string& description = "奖励");
    
    // 扣除积分（用于兑换）
    bool deductPoints(int points, const std::string& description = "兑换支出");
    
    // ========== 查询 ==========
    // 获取当前总积分
    int getTotalPoints() const;
    
    // 获取所有积分记录
    std::vector<PointRecord> getAllRecords() const;
    
    // 获取指定类型的积分记录
    std::vector<PointRecord> getRecordsByType(PointType type) const;
    
    // 获取最近 N 条记录
    std::vector<PointRecord> getRecentRecords(size_t count) const;
    
    // 获取今日获得的积分
    int getTodayPoints() const;
    
    // 获取指定类型的总积分
    int getTotalPointsByType(PointType type) const;
    
    // ========== 配置 ==========
    // 设置答题积分基础值
    void setQuizPointBase(int base) { quizPointBase_ = base; }
    int getQuizPointBase() const { return quizPointBase_; }
    
    // 设置作业积分基础值
    void setHomeworkPointBase(int base) { homeworkPointBase_ = base; }
    int getHomeworkPointBase() const { return homeworkPointBase_; }
    
    // 设置表现积分基础值
    void setPerformancePointBase(int base) { performancePointBase_ = base; }
    int getPerformancePointBase() const { return performancePointBase_; }
    
    // ========== 序列化 ==========
    // 序列化所有数据
    std::string serialize() const;
    
    // 反序列化
    void deserialize(const std::string& json);
    
    // 重置系统（清空所有积分和记录）
    void reset();

private:
    PointSystem();
    
    mutable std::mutex mutex_;          // 线程安全锁
    int totalPoints_ = 0;               // 当前总积分
    std::vector<PointRecord> records_;  // 积分记录历史
    int64_t nextRecordId_ = 1;          // 下一条记录 ID
    
    // 积分配置
    int quizPointBase_ = 10;           // 答题积分基础值
    int homeworkPointBase_ = 20;       // 作业积分基础值
    int performancePointBase_ = 5;     // 表现积分基础值
    
    static std::weak_ptr<PointSystem> instance_;  // 单例弱引用
    
    // 添加积分记录
    void addRecord(PointType type, int points, const std::string& description);
};

// 积分类型名称映射
inline std::string getPointTypeName(PointType type) {
    switch (type) {
        case PointType::Quiz: return "答题";
        case PointType::Homework: return "作业";
        case PointType::Performance: return "表现";
        case PointType::Exchange: return "兑换";
        case PointType::Reward: return "奖励";
        default: return "未知";
    }
}

} // namespace pet

#endif // POINTSYSTEM_H
