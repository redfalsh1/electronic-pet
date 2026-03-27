#include "pet.h"
#include <sstream>
#include <iomanip>
#include <ctime>

namespace pet {

std::weak_ptr<Pet> Pet::instance_;

Pet::Pet() {
    state_.createTime = std::chrono::system_clock::now();
    state_.lastFeedTime = state_.createTime;
    state_.lastPlayTime = state_.createTime;
    checkGrowthStage();
}

Pet::Pet(const PetState& state) : state_(state) {
    checkGrowthStage();
}

std::shared_ptr<Pet> Pet::getInstance() {
    auto inst = instance_.lock();
    if (!inst) {
        inst = std::make_shared<Pet>();
        instance_ = inst;
    }
    return inst;
}

bool Pet::addExperience(int exp) {
    if (!isAlive()) return false;
    
    state_.experience += exp;
    if (state_.experience < 0) state_.experience = 0;
    
    // 检查是否可以升级
    while (canLevelUp()) {
        levelUp();
    }
    
    return true;
}

void Pet::levelUp() {
    state_.level++;
    state_.experience -= state_.maxExperience;
    state_.maxExperience = static_cast<int>(state_.maxExperience * 1.5);  // 升级难度增加
    
    // 升级恢复部分状态
    state_.hunger = std::min(100, state_.hunger + 20);
    state_.mood = std::min(100, state_.mood + 20);
    state_.health = std::min(100, state_.health + 10);
    
    // 检查成长阶段
    checkGrowthStage();
}

void Pet::feed(int foodAmount) {
    if (!isAlive()) return;
    
    state_.hunger = std::min(100, state_.hunger + foodAmount);
    state_.lastFeedTime = std::chrono::system_clock::now();
    
    // 喂食提升心情
    state_.mood = std::min(100, state_.mood + 5);
}

void Pet::play(int joyAmount) {
    if (!isAlive()) return;
    
    state_.mood = std::min(100, state_.mood + joyAmount);
    state_.lastPlayTime = std::chrono::system_clock::now();
    
    // 玩耍消耗体力（饥饿度）
    state_.hunger = std::max(0, state_.hunger - 10);
}

void Pet::rest(int healthRecovery) {
    if (!isAlive()) return;
    
    state_.health = std::min(100, state_.health + healthRecovery);
    state_.mood = std::min(100, state_.mood + 5);
}

void Pet::updateOverTime() {
    auto now = std::chrono::system_clock::now();
    
    // 计算距离上次喂食的时间（分钟）
    auto feedDuration = std::chrono::duration_cast<std::chrono::minutes>(
        now - state_.lastFeedTime).count();
    
    // 计算距离上次玩耍的时间（分钟）
    auto playDuration = std::chrono::duration_cast<std::chrono::minutes>(
        now - state_.lastPlayTime).count();
    
    // 每 10 分钟减少饥饿度
    int hungerDecrease = static_cast<int>(feedDuration / 10);
    state_.hunger = std::max(0, state_.hunger - hungerDecrease);
    
    // 每 15 分钟减少心情值
    int moodDecrease = static_cast<int>(playDuration / 15);
    state_.mood = std::max(0, state_.mood - moodDecrease);
    
    // 饥饿度影响健康
    if (state_.hunger < 20) {
        state_.health = std::max(0, state_.health - 1);
    }
    
    // 心情值影响健康
    if (state_.mood < 20) {
        state_.health = std::max(0, state_.health - 1);
    }
    
    // 检查成长阶段
    checkGrowthStage();
}

void Pet::checkGrowthStage() {
    GrowthStage newStage = calculateStage();
    if (newStage != state_.stage) {
        state_.stage = newStage;
    }
}

GrowthStage Pet::calculateStage() const {
    if (state_.level < 5) return GrowthStage::Baby;
    if (state_.level < 15) return GrowthStage::Child;
    if (state_.level < 30) return GrowthStage::Teenager;
    if (state_.level < 50) return GrowthStage::Adult;
    return GrowthStage::Elder;
}

std::string Pet::serialize() const {
    std::ostringstream oss;
    oss << "{";
    oss << "\"level\":" << state_.level << ",";
    oss << "\"experience\":" << state_.experience << ",";
    oss << "\"maxExperience\":" << state_.maxExperience << ",";
    oss << "\"stage\":" << static_cast<int>(state_.stage) << ",";
    oss << "\"hunger\":" << state_.hunger << ",";
    oss << "\"mood\":" << state_.mood << ",";
    oss << "\"health\":" << state_.health << ",";
    
    // 时间戳序列化
    auto timeToTimestamp = [](const std::chrono::system_clock::time_point& tp) -> long long {
        return std::chrono::duration_cast<std::chrono::seconds>(
            tp.time_since_epoch()).count();
    };
    
    oss << "\"lastFeedTime\":" << timeToTimestamp(state_.lastFeedTime) << ",";
    oss << "\"lastPlayTime\":" << timeToTimestamp(state_.lastPlayTime) << ",";
    oss << "\"createTime\":" << timeToTimestamp(state_.createTime);
    oss << "}";
    
    return oss.str();
}

std::shared_ptr<Pet> Pet::deserialize(const std::string& json) {
    // 简单的 JSON 解析（实际项目中建议使用 nlohmann/json 等库）
    PetState state;
    
    auto parseValue = [&json](const std::string& key, int& value) {
        std::string searchKey = "\"" + key + "\":";
        auto pos = json.find(searchKey);
        if (pos == std::string::npos) return;
        
        auto valueStart = pos + searchKey.length();
        auto valueEnd = json.find_first_of(",}", valueStart);
        auto valueStr = json.substr(valueStart, valueEnd - valueStart);
        value = std::stoi(valueStr);
    };
    
    parseValue("level", state.level);
    parseValue("experience", state.experience);
    parseValue("maxExperience", state.maxExperience);
    parseValue("stage", reinterpret_cast<int&>(state.stage));
    parseValue("hunger", state.hunger);
    parseValue("mood", state.mood);
    parseValue("health", state.health);
    
    // 时间戳解析
    auto parseTime = [&json](const std::string& key) -> std::chrono::system_clock::time_point {
        std::string searchKey = "\"" + key + "\":";
        auto pos = json.find(searchKey);
        if (pos == std::string::npos) return std::chrono::system_clock::now();
        
        auto valueStart = pos + searchKey.length();
        auto valueEnd = json.find_first_of(",}", valueStart);
        auto valueStr = json.substr(valueStart, valueEnd - valueStart);
        auto timestamp = std::stoll(valueStr);
        return std::chrono::system_clock::from_time_t(static_cast<time_t>(timestamp));
    };
    
    state.lastFeedTime = parseTime("lastFeedTime");
    state.lastPlayTime = parseTime("lastPlayTime");
    state.createTime = parseTime("createTime");
    
    return std::make_shared<Pet>(state);
}

std::string Pet::getStageName(GrowthStage stage) {
    return getGrowthStageName(stage);
}

} // namespace pet
