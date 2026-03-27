#include "pointsystem.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctime>

namespace pet {

std::weak_ptr<PointSystem> PointSystem::instance_;

PointSystem::PointSystem() = default;

std::shared_ptr<PointSystem> PointSystem::getInstance() {
    auto inst = instance_.lock();
    if (!inst) {
        inst = std::make_shared<PointSystem>();
        instance_ = inst;
    }
    return inst;
}

int PointSystem::addQuizPoints(int points, const std::string& description) {
    std::lock_guard<std::mutex> lock(mutex_);
    int actualPoints = points > 0 ? points : quizPointBase_;
    addRecord(PointType::Quiz, actualPoints, description);
    return totalPoints_;
}

int PointSystem::addHomeworkPoints(int points, const std::string& description) {
    std::lock_guard<std::mutex> lock(mutex_);
    int actualPoints = points > 0 ? points : homeworkPointBase_;
    addRecord(PointType::Homework, actualPoints, description);
    return totalPoints_;
}

int PointSystem::addPerformancePoints(int points, const std::string& description) {
    std::lock_guard<std::mutex> lock(mutex_);
    int actualPoints = points > 0 ? points : performancePointBase_;
    addRecord(PointType::Performance, actualPoints, description);
    return totalPoints_;
}

int PointSystem::addRewardPoints(int points, const std::string& description) {
    std::lock_guard<std::mutex> lock(mutex_);
    addRecord(PointType::Reward, points, description);
    return totalPoints_;
}

bool PointSystem::deductPoints(int points, const std::string& description) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (totalPoints_ < points) {
        return false;  // 积分不足
    }
    addRecord(PointType::Exchange, -points, description);
    return true;
}

int PointSystem::getTotalPoints() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return totalPoints_;
}

std::vector<PointRecord> PointSystem::getAllRecords() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return records_;
}

std::vector<PointRecord> PointSystem::getRecordsByType(PointType type) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<PointRecord> result;
    std::copy_if(records_.begin(), records_.end(), std::back_inserter(result),
        [type](const PointRecord& record) { return record.type == type; });
    return result;
}

std::vector<PointRecord> PointSystem::getRecentRecords(size_t count) const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (records_.size() <= count) {
        return records_;
    }
    return std::vector<PointRecord>(records_.end() - count, records_.end());
}

int PointSystem::getTodayPoints() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto now = std::chrono::system_clock::now();
    auto today = std::chrono::system_clock::to_time_t(now);
    struct tm tm_info;
    localtime_s(&tm_info, &today);
    tm_info.tm_hour = 0;
    tm_info.tm_min = 0;
    tm_info.tm_sec = 0;
    auto todayStart = std::chrono::system_clock::from_time_t(mktime(&tm_info));
    
    int todayPoints = 0;
    for (const auto& record : records_) {
        if (record.timestamp >= todayStart && record.points > 0) {
            todayPoints += record.points;
        }
    }
    return todayPoints;
}

int PointSystem::getTotalPointsByType(PointType type) const {
    std::lock_guard<std::mutex> lock(mutex_);
    int total = 0;
    for (const auto& record : records_) {
        if (record.type == type && record.points > 0) {
            total += record.points;
        }
    }
    return total;
}

void PointSystem::addRecord(PointType type, int points, const std::string& description) {
    PointRecord record;
    record.id = nextRecordId_++;
    record.type = type;
    record.points = points;
    record.description = description;
    record.timestamp = std::chrono::system_clock::now();
    
    records_.push_back(record);
    totalPoints_ += points;
    
    // 限制记录数量（保留最近 1000 条）
    if (records_.size() > 1000) {
        records_.erase(records_.begin());
    }
}

std::string PointRecord::serialize() const {
    std::ostringstream oss;
    oss << id << "|"
        << static_cast<int>(type) << "|"
        << points << "|"
        << description << "|"
        << std::chrono::duration_cast<std::chrono::seconds>(
            timestamp.time_since_epoch()).count();
    return oss.str();
}

PointRecord PointRecord::deserialize(const std::string& str) {
    PointRecord record;
    std::istringstream iss(str);
    std::string token;
    
    std::getline(iss, token, '|');
    record.id = std::stoll(token);
    
    std::getline(iss, token, '|');
    record.type = static_cast<PointType>(std::stoi(token));
    
    std::getline(iss, token, '|');
    record.points = std::stoi(token);
    
    std::getline(iss, record.description, '|');
    
    std::getline(iss, token, '|');
    auto timestamp = std::stoll(token);
    record.timestamp = std::chrono::system_clock::from_time_t(
        static_cast<time_t>(timestamp));
    
    return record;
}

std::string PointSystem::serialize() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::ostringstream oss;
    oss << "{";
    oss << "\"totalPoints\":" << totalPoints_ << ",";
    oss << "\"nextRecordId\":" << nextRecordId_ << ",";
    oss << "\"quizPointBase\":" << quizPointBase_ << ",";
    oss << "\"homeworkPointBase\":" << homeworkPointBase_ << ",";
    oss << "\"performancePointBase\":" << performancePointBase_ << ",";
    oss << "\"records\":[";
    
    for (size_t i = 0; i < records_.size(); ++i) {
        if (i > 0) oss << ",";
        oss << "{\"id\":" << records_[i].id
            << ",\"type\":" << static_cast<int>(records_[i].type)
            << ",\"points\":" << records_[i].points
            << ",\"description\":\"" << records_[i].description << "\""
            << ",\"timestamp\":" << std::chrono::duration_cast<std::chrono::seconds>(
                records_[i].timestamp.time_since_epoch()).count()
            << "}";
    }
    
    oss << "]}";
    return oss.str();
}

void PointSystem::deserialize(const std::string& json) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // 简单 JSON 解析
    auto parseValue = [&json](const std::string& key, int& value) {
        std::string searchKey = "\"" + key + "\":";
        auto pos = json.find(searchKey);
        if (pos == std::string::npos) return;
        auto valueStart = pos + searchKey.length();
        auto valueEnd = json.find_first_of(",}", valueStart);
        auto valueStr = json.substr(valueStart, valueEnd - valueStart);
        value = std::stoi(valueStr);
    };
    
    parseValue("totalPoints", totalPoints_);
    parseValue("nextRecordId", reinterpret_cast<int&>(nextRecordId_));
    parseValue("quizPointBase", quizPointBase_);
    parseValue("homeworkPointBase", homeworkPointBase_);
    parseValue("performancePointBase", performancePointBase_);
    
    // 解析 records 数组（简化处理）
    records_.clear();
    auto recordsPos = json.find("\"records\":[");
    if (recordsPos != std::string::npos) {
        // 实际项目中应使用完整 JSON 解析器
        // 这里简化处理，不解析历史记录
    }
}

void PointSystem::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    totalPoints_ = 0;
    records_.clear();
    nextRecordId_ = 1;
}

} // namespace pet
