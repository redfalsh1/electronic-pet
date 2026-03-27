#include "datamanager.h"
#include "pet.h"
#include "pointsystem.h"
#include "exchangesystem.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <ctime>
#include <algorithm>

namespace pet {

namespace fs = std::filesystem;

std::weak_ptr<DataManager> DataManager::instance_;

DataManager::DataManager() = default;

DataManager::~DataManager() {
    shutdown();
}

std::shared_ptr<DataManager> DataManager::getInstance() {
    auto inst = instance_.lock();
    if (!inst) {
        inst = std::make_shared<DataManager>();
        instance_ = inst;
    }
    return inst;
}

bool DataManager::initialize(const std::string& dbPath) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (initialized_) {
        return true;  // 已初始化
    }
    
    dbPath_ = dbPath;
    
    // 确保目录存在
    fs::path path(dbPath_);
    fs::path dir = path.parent_path();
    if (!dir.empty() && !fs::exists(dir)) {
        fs::create_directories(dir);
    }
    
    // 初始化数据库表
    if (!initializeTables()) {
        return false;
    }
    
    initialized_ = true;
    lastSaveTime_ = std::chrono::system_clock::now();
    lastAutoSaveTime_ = lastSaveTime_;
    
    // 加载设置缓存
    auto settingsFile = dbPath_ + ".settings";
    if (fs::exists(settingsFile)) {
        std::ifstream file(settingsFile);
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                auto pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = line.substr(0, pos);
                    std::string value = line.substr(pos + 1);
                    settingsCache_[key] = value;
                }
            }
        }
    }
    
    return true;
}

void DataManager::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (initialized_) {
        // 保存所有数据
        saveAll();
        initialized_ = false;
    }
}

bool DataManager::initializeTables() {
    // 创建数据文件（模拟 SQLite 表结构）
    // pet_data - 宠物数据
    // points_data - 积分数据
    // exchange_data - 兑换数据
    // settings - 设置数据
    
    // 检查并创建文件
    std::vector<std::string> files = {
        dbPath_ + ".pet",
        dbPath_ + ".points",
        dbPath_ + ".exchange",
        dbPath_ + ".settings"
    };
    
    for (const auto& file : files) {
        if (!fs::exists(file)) {
            std::ofstream ofs(file);
            if (!ofs.is_open()) {
                return false;
            }
        }
    }
    
    return true;
}

bool DataManager::executeSQL(const std::string& sql, std::string* errorMsg) {
    // 简化实现，实际项目中应使用真正的 SQLite
    // 这里仅记录 SQL 到日志文件（调试用）
    std::ofstream logFile(dbPath_ + ".log", std::ios::app);
    if (logFile.is_open()) {
        logFile << "[" << getCurrentTimestamp() << "] " << sql << std::endl;
    }
    return true;
}

SaveResult DataManager::saveAll() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    SaveResult result;
    result.timestamp = getCurrentTimestamp();
    result.success = true;
    
    auto now = std::chrono::system_clock::now();
    
    // 保存宠物数据
    auto pet = Pet::getInstance();
    if (pet) {
        auto petResult = savePet(pet);
        if (!petResult.success) {
            result.success = false;
            result.message += "宠物保存失败; ";
        }
    }
    
    // 保存积分数据
    auto points = PointSystem::getInstance();
    if (points) {
        auto pointsResult = savePoints(points->serialize());
        if (!pointsResult.success) {
            result.success = false;
            result.message += "积分保存失败; ";
        }
    }
    
    // 保存兑换数据
    auto exchange = ExchangeSystem::getInstance();
    if (exchange) {
        auto exchangeResult = saveExchange(exchange->serialize());
        if (!exchangeResult.success) {
            result.success = false;
            result.message += "兑换保存失败; ";
        }
    }
    
    // 保存设置缓存
    auto settingsFile = dbPath_ + ".settings";
    std::ofstream settingsOut(settingsFile);
    if (settingsOut.is_open()) {
        for (const auto& pair : settingsCache_) {
            settingsOut << pair.first << "=" << pair.second << std::endl;
        }
    }
    
    if (result.success) {
        result.message = "所有数据保存成功";
    }
    
    lastSaveTime_ = now;
    lastAutoSaveTime_ = now;
    
    onSaveComplete(result);
    
    return result;
}

SaveResult DataManager::savePet(const std::shared_ptr<Pet>& pet) {
    SaveResult result;
    result.timestamp = getCurrentTimestamp();
    
    if (!pet) {
        result.success = false;
        result.message = "宠物对象为空";
        return result;
    }
    
    std::string petFile = dbPath_ + ".pet";
    std::ofstream file(petFile);
    if (!file.is_open()) {
        result.success = false;
        result.message = "无法打开宠物数据文件";
        return result;
    }
    
    file << pet->serialize();
    file.close();
    
    result.success = true;
    result.message = "宠物数据保存成功";
    
    onDataChange(DataType::Pet);
    
    return result;
}

std::shared_ptr<Pet> DataManager::loadPet() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::string petFile = dbPath_ + ".pet";
    if (!fs::exists(petFile)) {
        return nullptr;
    }
    
    std::ifstream file(petFile);
    if (!file.is_open()) {
        return nullptr;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string json = buffer.str();
    
    if (json.empty()) {
        return nullptr;
    }
    
    return Pet::deserialize(json);
}

bool DataManager::hasPetData() const {
    std::string petFile = dbPath_ + ".pet";
    return fs::exists(petFile);
}

SaveResult DataManager::savePoints(const std::string& json) {
    SaveResult result;
    result.timestamp = getCurrentTimestamp();
    
    std::string pointsFile = dbPath_ + ".points";
    std::ofstream file(pointsFile);
    if (!file.is_open()) {
        result.success = false;
        result.message = "无法打开积分数据文件";
        return result;
    }
    
    file << json;
    file.close();
    
    result.success = true;
    result.message = "积分数据保存成功";
    
    onDataChange(DataType::Points);
    
    return result;
}

std::string DataManager::loadPoints() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::string pointsFile = dbPath_ + ".points";
    if (!fs::exists(pointsFile)) {
        return "";
    }
    
    std::ifstream file(pointsFile);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

SaveResult DataManager::saveExchange(const std::string& json) {
    SaveResult result;
    result.timestamp = getCurrentTimestamp();
    
    std::string exchangeFile = dbPath_ + ".exchange";
    std::ofstream file(exchangeFile);
    if (!file.is_open()) {
        result.success = false;
        result.message = "无法打开兑换数据文件";
        return result;
    }
    
    file << json;
    file.close();
    
    result.success = true;
    result.message = "兑换数据保存成功";
    
    onDataChange(DataType::Exchange);
    
    return result;
}

std::string DataManager::loadExchange() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::string exchangeFile = dbPath_ + ".exchange";
    if (!fs::exists(exchangeFile)) {
        return "";
    }
    
    std::ifstream file(exchangeFile);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

SaveResult DataManager::saveSetting(const std::string& key, const std::string& value) {
    SaveResult result;
    result.timestamp = getCurrentTimestamp();
    
    std::lock_guard<std::mutex> lock(mutex_);
    settingsCache_[key] = value;
    
    // 立即保存到文件
    auto settingsFile = dbPath_ + ".settings";
    std::ofstream file(settingsFile);
    if (!file.is_open()) {
        result.success = false;
        result.message = "无法打开设置文件";
        return result;
    }
    
    for (const auto& pair : settingsCache_) {
        file << pair.first << "=" << pair.second << std::endl;
    }
    file.close();
    
    result.success = true;
    result.message = "设置保存成功";
    
    onDataChange(DataType::Settings);
    
    return result;
}

std::string DataManager::loadSetting(const std::string& key, const std::string& defaultValue) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = settingsCache_.find(key);
    if (it != settingsCache_.end()) {
        return it->second;
    }
    
    return defaultValue;
}

bool DataManager::deleteSetting(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = settingsCache_.find(key);
    if (it == settingsCache_.end()) {
        return false;
    }
    
    settingsCache_.erase(it);
    
    // 保存到文件
    auto settingsFile = dbPath_ + ".settings";
    std::ofstream file(settingsFile);
    if (file.is_open()) {
        for (const auto& pair : settingsCache_) {
            file << pair.first << "=" << pair.second << std::endl;
        }
    }
    
    onDataChange(DataType::Settings);
    
    return true;
}

std::map<std::string, std::string> DataManager::getAllSettings() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return settingsCache_;
}

SaveResult DataManager::createBackup(const std::string& backupPath) {
    SaveResult result;
    result.timestamp = getCurrentTimestamp();
    
    std::string actualBackupPath = backupPath;
    if (actualBackupPath.empty()) {
        // 生成带时间戳的备份文件名
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        struct tm tm_info;
        localtime_s(&tm_info, &now);
        char buffer[64];
        strftime(buffer, sizeof(buffer), "_backup_%Y%m%d_%H%M%S", &tm_info);
        actualBackupPath = dbPath_ + buffer;
    }
    
    // 复制所有数据文件
    std::vector<std::string> extensions = {".pet", ".points", ".exchange", ".settings"};
    
    for (const auto& ext : extensions) {
        std::string srcFile = dbPath_ + ext;
        std::string dstFile = actualBackupPath + ext;
        
        if (fs::exists(srcFile)) {
            try {
                fs::copy_file(srcFile, dstFile, fs::copy_options::overwrite_existing);
            } catch (const std::exception& e) {
                result.success = false;
                result.message = "备份失败：" + std::string(e.what());
                return result;
            }
        }
    }
    
    result.success = true;
    result.message = "备份创建成功：" + actualBackupPath;
    
    return result;
}

SaveResult DataManager::restoreFromBackup(const std::string& backupPath) {
    SaveResult result;
    result.timestamp = getCurrentTimestamp();
    
    std::vector<std::string> extensions = {".pet", ".points", ".exchange", ".settings"};
    
    for (const auto& ext : extensions) {
        std::string srcFile = backupPath + ext;
        std::string dstFile = dbPath_ + ext;
        
        if (fs::exists(srcFile)) {
            try {
                fs::copy_file(srcFile, dstFile, fs::copy_options::overwrite_existing);
            } catch (const std::exception& e) {
                result.success = false;
                result.message = "恢复失败：" + std::string(e.what());
                return result;
            }
        }
    }
    
    // 重新加载设置缓存
    settingsCache_.clear();
    auto settingsFile = dbPath_ + ".settings";
    if (fs::exists(settingsFile)) {
        std::ifstream file(settingsFile);
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                auto pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = line.substr(0, pos);
                    std::string value = line.substr(pos + 1);
                    settingsCache_[key] = value;
                }
            }
        }
    }
    
    result.success = true;
    result.message = "数据恢复成功";
    
    return result;
}

std::vector<std::string> DataManager::getBackupList() const {
    std::vector<std::string> backups;
    
    fs::path dir = fs::path(dbPath_).parent_path();
    if (fs::exists(dir)) {
        std::string baseName = fs::path(dbPath_).filename().string();
        
        for (const auto& entry : fs::directory_iterator(dir)) {
            std::string filename = entry.path().filename().string();
            if (filename.find(baseName) == 0 && filename.find("_backup_") != std::string::npos) {
                backups.push_back(entry.path().string());
            }
        }
    }
    
    // 按时间排序（最新的在前）
    std::sort(backups.begin(), backups.end(), std::greater<std::string>());
    
    return backups;
}

int64_t DataManager::getDatabaseSize() const {
    int64_t totalSize = 0;
    
    std::vector<std::string> extensions = {".pet", ".points", ".exchange", ".settings"};
    
    for (const auto& ext : extensions) {
        std::string file = dbPath_ + ext;
        if (fs::exists(file)) {
            totalSize += static_cast<int64_t>(fs::file_size(file));
        }
    }
    
    return totalSize;
}

int DataManager::getRecordCount(DataType type) const {
    // 简化实现，返回 0
    // 实际项目中应查询数据库
    return 0;
}

void DataManager::clearAllData() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> extensions = {".pet", ".points", ".exchange", ".settings"};
    
    for (const auto& ext : extensions) {
        std::string file = dbPath_ + ext;
        if (fs::exists(file)) {
            fs::remove(file);
        }
    }
    
    settingsCache_.clear();
    initialized_ = false;
}

std::string DataManager::exportAllData() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::ostringstream oss;
    oss << "{";
    
    // 导出宠物数据
    std::string petFile = dbPath_ + ".pet";
    if (fs::exists(petFile)) {
        std::ifstream file(petFile);
        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            oss << "\"pet\":" << buffer.str() << ",";
        }
    }
    
    // 导出积分数据
    std::string pointsFile = dbPath_ + ".points";
    if (fs::exists(pointsFile)) {
        std::ifstream file(pointsFile);
        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            oss << "\"points\":" << buffer.str() << ",";
        }
    }
    
    // 导出兑换数据
    std::string exchangeFile = dbPath_ + ".exchange";
    if (fs::exists(exchangeFile)) {
        std::ifstream file(exchangeFile);
        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            oss << "\"exchange\":" << buffer.str() << ",";
        }
    }
    
    // 导出设置
    oss << "\"settings\":{";
    bool first = true;
    for (const auto& pair : settingsCache_) {
        if (!first) oss << ",";
        first = false;
        oss << "\"" << pair.first << "\":\"" << pair.second << "\"";
    }
    oss << "}";
    
    oss << "}";
    return oss.str();
}

bool DataManager::importAllData(const std::string& json) {
    // 简化实现，实际项目中应解析 JSON 并导入
    // 这里仅保存原始 JSON 到文件
    std::lock_guard<std::mutex> lock(mutex_);
    
    // 查找各个部分并保存
    // 简化处理：直接保存整个 JSON 到备份文件
    std::string importFile = dbPath_ + ".import";
    std::ofstream file(importFile);
    if (!file.is_open()) {
        return false;
    }
    file << json;
    file.close();
    
    return true;
}

void DataManager::onDataChange(DataType type) {
    if (dataChangeCallback_) {
        dataChangeCallback_(type);
    }
}

void DataManager::onSaveComplete(const SaveResult& result) {
    if (saveCompleteCallback_) {
        saveCompleteCallback_(result);
    }
}

void DataManager::checkAutoSave() {
    if (!autoSaveEnabled_ || !initialized_) {
        return;
    }
    
    auto now = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
        now - lastAutoSaveTime_).count();
    
    if (elapsed >= autoSaveInterval_) {
        saveAll();
    }
}

int64_t DataManager::getCurrentTimestamp() const {
    return std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}

} // namespace pet
