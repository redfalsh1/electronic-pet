#include "exchangesystem.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>

namespace pet {

std::weak_ptr<ExchangeSystem> ExchangeSystem::instance_;

ExchangeSystem::ExchangeSystem() {
    todayStart_ = std::chrono::system_clock::now();
    initializeDefaultRules();
}

std::shared_ptr<ExchangeSystem> ExchangeSystem::getInstance() {
    auto inst = instance_.lock();
    if (!inst) {
        inst = std::make_shared<ExchangeSystem>();
        instance_ = inst;
    }
    return inst;
}

void ExchangeSystem::initializeDefaultRules() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // 规则 1: 10 积分兑换 1 单位粮食
    ExchangeRule rule1;
    rule1.id = "points_to_food_1";
    rule1.type = ExchangeType::PointsToFood;
    rule1.name = "积分换粮食";
    rule1.cost = 10;
    rule1.reward = 1;
    rule1.costUnit = "积分";
    rule1.rewardUnit = "粮食";
    rule1.dailyLimit = 50;  // 每日最多 50 次
    rule1.totalLimit = -1;
    rule1.enabled = true;
    rule1.description = "10 积分兑换 1 单位宠物粮食";
    rules_[rule1.id] = rule1;
    
    // 规则 2: 1 单位粮食兑换 5 点经验值
    ExchangeRule rule2;
    rule2.id = "food_to_exp_1";
    rule2.type = ExchangeType::FoodToExperience;
    rule2.name = "粮食换经验";
    rule2.cost = 1;
    rule2.reward = 5;
    rule2.costUnit = "粮食";
    rule2.rewardUnit = "经验";
    rule2.dailyLimit = 100;  // 每日最多 100 次
    rule2.totalLimit = -1;
    rule2.enabled = true;
    rule2.description = "1 单位粮食兑换 5 点经验值";
    rules_[rule2.id] = rule2;
    
    // 规则 3: 50 积分兑换 10 单位粮食（批量优惠）
    ExchangeRule rule3;
    rule3.id = "points_to_food_bulk";
    rule3.type = ExchangeType::PointsToFood;
    rule3.name = "批量换粮食";
    rule3.cost = 50;
    rule3.reward = 6;  // 优惠：原本 50 积分换 5 粮食，现在换 6 粮食
    rule3.costUnit = "积分";
    rule3.rewardUnit = "粮食";
    rule3.dailyLimit = 10;
    rule3.totalLimit = -1;
    rule3.enabled = true;
    rule3.description = "50 积分兑换 6 单位宠物粮食（优惠）";
    rules_[rule3.id] = rule3;
}

bool ExchangeSystem::executeExchange(const std::string& ruleId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    checkAndUpdateTodayCount();
    
    auto it = rules_.find(ruleId);
    if (it == rules_.end()) {
        return false;  // 规则不存在
    }
    
    const auto& rule = it->second;
    if (!rule.enabled) {
        return false;  // 规则已禁用
    }
    
    // 检查每日限制
    if (rule.dailyLimit >= 0) {
        auto countIt = todayExchangeCount_.find(ruleId);
        int currentCount = (countIt != todayExchangeCount_.end()) ? countIt->second : 0;
        if (currentCount >= rule.dailyLimit) {
            return false;  // 超出每日限制
        }
    }
    
    // 检查总限制
    if (rule.totalLimit >= 0) {
        auto countIt = totalExchangeCount_.find(ruleId);
        int currentCount = (countIt != totalExchangeCount_.end()) ? countIt->second : 0;
        if (currentCount >= rule.totalLimit) {
            return false;  // 超出总限制
        }
    }
    
    // 执行回调检查（如积分是否足够）
    if (pointsDeductCallback_ && rule.type == ExchangeType::PointsToFood) {
        if (!pointsDeductCallback_(rule.cost, "兑换:" + rule.name)) {
            return false;  // 回调返回失败（如积分不足）
        }
    }
    
    // 创建兑换记录
    ExchangeRecord record;
    record.id = nextRecordId_++;
    record.ruleId = ruleId;
    record.type = rule.type;
    record.costAmount = rule.cost;
    record.rewardAmount = rule.reward;
    record.timestamp = std::chrono::system_clock::now();
    record.success = true;
    
    // 执行兑换效果
    switch (rule.type) {
        case ExchangeType::PointsToFood:
            if (pointsDeductCallback_) {
                pointsDeductCallback_(rule.cost, "兑换:" + rule.name);
            }
            if (foodAddCallback_) {
                foodAddCallback_(rule.reward);
            }
            break;
            
        case ExchangeType::FoodToExperience:
            if (expAddCallback_) {
                expAddCallback_(rule.reward);
            }
            break;
            
        case ExchangeType::PointsToItem:
            // 道具兑换，暂不处理
            break;
    }
    
    // 更新计数
    todayExchangeCount_[ruleId]++;
    totalExchangeCount_[ruleId]++;
    
    // 添加记录
    addRecord(record);
    
    return true;
}

bool ExchangeSystem::exchangePointsToFood(int points, int& foodAmount) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // 使用默认兑换率：10 积分 = 1 粮食
    auto it = rules_.find("points_to_food_1");
    if (it == rules_.end() || !it->second.enabled) {
        return false;
    }
    
    int exchangeRate = it->second.cost;  // 10 积分
    foodAmount = points / exchangeRate;
    
    if (foodAmount <= 0) {
        return false;  // 积分不足
    }
    
    // 实际扣除积分
    int actualCost = foodAmount * exchangeRate;
    if (pointsDeductCallback_) {
        if (!pointsDeductCallback_(actualCost, "兑换粮食")) {
            return false;
        }
    }
    
    if (foodAddCallback_) {
        foodAddCallback_(foodAmount);
    }
    
    return true;
}

bool ExchangeSystem::exchangeFoodToExperience(int foodAmount, int& expAmount) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // 使用默认兑换率：1 粮食 = 5 经验
    auto it = rules_.find("food_to_exp_1");
    if (it == rules_.end() || !it->second.enabled) {
        return false;
    }
    
    int exchangeRate = it->second.reward;  // 5 经验
    expAmount = foodAmount * exchangeRate;
    
    if (expAmount <= 0) {
        return false;  // 粮食不足
    }
    
    if (expAddCallback_) {
        expAddCallback_(expAmount);
    }
    
    return true;
}

void ExchangeSystem::addRule(const ExchangeRule& rule) {
    std::lock_guard<std::mutex> lock(mutex_);
    rules_[rule.id] = rule;
}

bool ExchangeSystem::removeRule(const std::string& ruleId) {
    std::lock_guard<std::mutex> lock(mutex_);
    return rules_.erase(ruleId) > 0;
}

bool ExchangeSystem::updateRule(const ExchangeRule& rule) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = rules_.find(rule.id);
    if (it == rules_.end()) {
        return false;
    }
    it->second = rule;
    return true;
}

std::vector<ExchangeRule> ExchangeSystem::getAllRules() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<ExchangeRule> result;
    for (const auto& pair : rules_) {
        result.push_back(pair.second);
    }
    return result;
}

std::vector<ExchangeRule> ExchangeSystem::getRulesByType(ExchangeType type) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<ExchangeRule> result;
    for (const auto& pair : rules_) {
        if (pair.second.type == type) {
            result.push_back(pair.second);
        }
    }
    return result;
}

ExchangeRule ExchangeSystem::getRule(const std::string& ruleId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = rules_.find(ruleId);
    if (it != rules_.end()) {
        return it->second;
    }
    return ExchangeRule{};
}

bool ExchangeSystem::setRuleEnabled(const std::string& ruleId, bool enabled) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = rules_.find(ruleId);
    if (it == rules_.end()) {
        return false;
    }
    it->second.enabled = enabled;
    return true;
}

std::vector<ExchangeRecord> ExchangeSystem::getAllRecords() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return records_;
}

std::vector<ExchangeRecord> ExchangeSystem::getRecentRecords(size_t count) const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (records_.size() <= count) {
        return records_;
    }
    return std::vector<ExchangeRecord>(records_.end() - count, records_.end());
}

int ExchangeSystem::getTodayExchangeCount(const std::string& ruleId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = todayExchangeCount_.find(ruleId);
    return (it != todayExchangeCount_.end()) ? it->second : 0;
}

int ExchangeSystem::getTotalExchangeCount(const std::string& ruleId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = totalExchangeCount_.find(ruleId);
    return (it != totalExchangeCount_.end()) ? it->second : 0;
}

bool ExchangeSystem::canExchange(const std::string& ruleId, std::string& reason) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = rules_.find(ruleId);
    if (it == rules_.end()) {
        reason = "兑换规则不存在";
        return false;
    }
    
    const auto& rule = it->second;
    if (!rule.enabled) {
        reason = "兑换规则已禁用";
        return false;
    }
    
    // 检查每日限制
    if (rule.dailyLimit >= 0) {
        auto countIt = todayExchangeCount_.find(ruleId);
        int currentCount = (countIt != todayExchangeCount_.end()) ? countIt->second : 0;
        if (currentCount >= rule.dailyLimit) {
            reason = "已达到今日兑换次数上限";
            return false;
        }
    }
    
    // 检查总限制
    if (rule.totalLimit >= 0) {
        auto countIt = totalExchangeCount_.find(ruleId);
        int currentCount = (countIt != totalExchangeCount_.end()) ? countIt->second : 0;
        if (currentCount >= rule.totalLimit) {
            reason = "已达到总兑换次数上限";
            return false;
        }
    }
    
    reason = "可以兑换";
    return true;
}

void ExchangeSystem::checkAndUpdateTodayCount() {
    auto now = std::chrono::system_clock::now();
    auto nowTime = std::chrono::system_clock::to_time_t(now);
    struct tm tm_info;
    localtime_s(&tm_info, &nowTime);
    tm_info.tm_hour = 0;
    tm_info.tm_min = 0;
    tm_info.tm_sec = 0;
    auto todayStart = std::chrono::system_clock::from_time_t(mktime(&tm_info));
    
    // 如果已经是新的一天，重置每日计数
    if (todayStart > todayStart_) {
        todayExchangeCount_.clear();
        todayStart_ = todayStart;
    }
}

void ExchangeSystem::addRecord(const ExchangeRecord& record) {
    records_.push_back(record);
    
    // 限制记录数量（保留最近 500 条）
    if (records_.size() > 500) {
        records_.erase(records_.begin());
    }
}

std::string ExchangeRule::serialize() const {
    std::ostringstream oss;
    oss << id << "|"
        << static_cast<int>(type) << "|"
        << name << "|"
        << cost << "|"
        << reward << "|"
        << costUnit << "|"
        << rewardUnit << "|"
        << dailyLimit << "|"
        << totalLimit << "|"
        << (enabled ? 1 : 0) << "|"
        << description;
    return oss.str();
}

ExchangeRule ExchangeRule::deserialize(const std::string& str) {
    ExchangeRule rule;
    std::istringstream iss(str);
    std::string token;
    
    std::getline(iss, token, '|');
    rule.id = token;
    
    std::getline(iss, token, '|');
    rule.type = static_cast<ExchangeType>(std::stoi(token));
    
    std::getline(iss, rule.name, '|');
    
    std::getline(iss, token, '|');
    rule.cost = std::stoi(token);
    
    std::getline(iss, token, '|');
    rule.reward = std::stoi(token);
    
    std::getline(iss, rule.costUnit, '|');
    std::getline(iss, rule.rewardUnit, '|');
    
    std::getline(iss, token, '|');
    rule.dailyLimit = std::stoi(token);
    
    std::getline(iss, token, '|');
    rule.totalLimit = std::stoi(token);
    
    std::getline(iss, token, '|');
    rule.enabled = (token == "1");
    
    std::getline(iss, rule.description, '|');
    
    return rule;
}

std::string ExchangeRecord::serialize() const {
    std::ostringstream oss;
    oss << id << "|"
        << ruleId << "|"
        << static_cast<int>(type) << "|"
        << costAmount << "|"
        << rewardAmount << "|"
        << (success ? 1 : 0) << "|"
        << std::chrono::duration_cast<std::chrono::seconds>(
            timestamp.time_since_epoch()).count();
    return oss.str();
}

ExchangeRecord ExchangeRecord::deserialize(const std::string& str) {
    ExchangeRecord record;
    std::istringstream iss(str);
    std::string token;
    
    std::getline(iss, token, '|');
    record.id = std::stoll(token);
    
    std::getline(iss, record.ruleId, '|');
    
    std::getline(iss, token, '|');
    record.type = static_cast<ExchangeType>(std::stoi(token));
    
    std::getline(iss, token, '|');
    record.costAmount = std::stoi(token);
    
    std::getline(iss, token, '|');
    record.rewardAmount = std::stoi(token);
    
    std::getline(iss, token, '|');
    record.success = (token == "1");
    
    std::getline(iss, token, '|');
    auto timestamp = std::stoll(token);
    record.timestamp = std::chrono::system_clock::from_time_t(
        static_cast<time_t>(timestamp));
    
    return record;
}

std::string ExchangeSystem::serialize() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ostringstream oss;
    oss << "{";
    oss << "\"nextRecordId\":" << nextRecordId_ << ",";
    oss << "\"rules\":[";
    
    bool first = true;
    for (const auto& pair : rules_) {
        if (!first) oss << ",";
        first = false;
        const auto& rule = pair.second;
        oss << "{\"id\":\"" << rule.id << "\""
            << ",\"type\":" << static_cast<int>(rule.type)
            << ",\"name\":\"" << rule.name << "\""
            << ",\"cost\":" << rule.cost
            << ",\"reward\":" << rule.reward
            << ",\"costUnit\":\"" << rule.costUnit << "\""
            << ",\"rewardUnit\":\"" << rule.rewardUnit << "\""
            << ",\"dailyLimit\":" << rule.dailyLimit
            << ",\"totalLimit\":" << rule.totalLimit
            << ",\"enabled\":" << (rule.enabled ? "true" : "false")
            << ",\"description\":\"" << rule.description << "\"}";
    }
    
    oss << "],\"records\":[";
    
    first = true;
    for (const auto& record : records_) {
        if (!first) oss << ",";
        first = false;
        oss << "{\"id\":" << record.id
            << ",\"ruleId\":\"" << record.ruleId << "\""
            << ",\"type\":" << static_cast<int>(record.type)
            << ",\"costAmount\":" << record.costAmount
            << ",\"rewardAmount\":" << record.rewardAmount
            << ",\"success\":" << (record.success ? "true" : "false")
            << ",\"timestamp\":" << std::chrono::duration_cast<std::chrono::seconds>(
                record.timestamp.time_since_epoch()).count()
            << "}";
    }
    
    oss << "]}";
    return oss.str();
}

void ExchangeSystem::deserialize(const std::string& json) {
    std::lock_guard<std::mutex> lock(mutex_);
    // 简化处理，实际项目中应使用完整 JSON 解析器
    // 这里仅重置数据
    rules_.clear();
    records_.clear();
    nextRecordId_ = 1;
}

} // namespace pet
