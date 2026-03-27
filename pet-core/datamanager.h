#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <string>
#include <memory>
#include <mutex>
#include <chrono>
#include <functional>
#include <map>

namespace pet {

// 前向声明
class Pet;
class PointSystem;
class ExchangeSystem;

// 数据类型
enum class DataType {
    Pet,
    Points,
    Exchange,
    Settings
};

// 保存回调结果
struct SaveResult {
    bool success = false;
    std::string message;
    int64_t timestamp = 0;
};

// 数据管理器（单例模式）
class DataManager {
public:
    // 获取单例实例
    static std::shared_ptr<DataManager> getInstance();
    
    // 禁止拷贝和移动
    DataManager(const DataManager&) = delete;
    DataManager& operator=(const DataManager&) = delete;
    DataManager(DataManager&&) = delete;
    DataManager& operator=(DataManager&&) = delete;
    
    // ========== 初始化 ==========
    // 初始化数据管理器（指定数据库路径）
    bool initialize(const std::string& dbPath = "pet_data.db");
    
    // 关闭数据库连接
    void shutdown();
    
    // 检查是否已初始化
    bool isInitialized() const { return initialized_; }
    
    // ========== 自动保存配置 ==========
    // 启用/禁用自动保存
    void setAutoSaveEnabled(bool enabled) { autoSaveEnabled_ = enabled; }
    bool isAutoSaveEnabled() const { return autoSaveEnabled_; }
    
    // 设置自动保存间隔（秒）
    void setAutoSaveInterval(int seconds) { autoSaveInterval_ = seconds; }
    int getAutoSaveInterval() const { return autoSaveInterval_; }
    
    // 手动触发保存
    SaveResult saveAll();
    
    // ========== 宠物数据 ==========
    // 保存宠物数据
    SaveResult savePet(const std::shared_ptr<Pet>& pet);
    
    // 加载宠物数据
    std::shared_ptr<Pet> loadPet();
    
    // 检查是否存在宠物数据
    bool hasPetData() const;
    
    // ========== 积分数据 ==========
    // 保存积分数据
    SaveResult savePoints(const std::string& json);
    
    // 加载积分数据
    std::string loadPoints();
    
    // ========== 兑换数据 ==========
    // 保存兑换数据
    SaveResult saveExchange(const std::string& json);
    
    // 加载兑换数据
    std::string loadExchange();
    
    // ========== 设置数据 ==========
    // 保存设置
    SaveResult saveSetting(const std::string& key, const std::string& value);
    
    // 加载设置
    std::string loadSetting(const std::string& key, const std::string& defaultValue = "");
    
    // 删除设置
    bool deleteSetting(const std::string& key);
    
    // 获取所有设置
    std::map<std::string, std::string> getAllSettings() const;
    
    // ========== 备份与恢复 ==========
    // 创建备份
    SaveResult createBackup(const std::string& backupPath = "");
    
    // 从备份恢复
    SaveResult restoreFromBackup(const std::string& backupPath);
    
    // 获取备份列表
    std::vector<std::string> getBackupList() const;
    
    // ========== 回调设置 ==========
    // 设置数据变更回调
    using DataChangeCallback = std::function<void(DataType type)>;
    void setDataChangeCallback(DataChangeCallback callback) {
        dataChangeCallback_ = callback;
    }
    
    // 设置保存完成回调
    using SaveCompleteCallback = std::function<void(const SaveResult&)>;
    void setSaveCompleteCallback(SaveCompleteCallback callback) {
        saveCompleteCallback_ = callback;
    }
    
    // ========== 数据库信息 ==========
    // 获取数据库路径
    std::string getDatabasePath() const { return dbPath_; }
    
    // 获取数据库大小（字节）
    int64_t getDatabaseSize() const;
    
    // 获取最后保存时间
    std::chrono::system_clock::time_point getLastSaveTime() const { return lastSaveTime_; }
    
    // 获取记录数量
    int getRecordCount(DataType type) const;
    
    // ========== 工具函数 ==========
    // 清空所有数据
    void clearAllData();
    
    // 导出所有数据为 JSON
    std::string exportAllData() const;
    
    // 从 JSON 导入所有数据
    bool importAllData(const std::string& json);

private:
    DataManager();
    ~DataManager();
    
    mutable std::mutex mutex_;
    std::string dbPath_;
    bool initialized_ = false;
    bool autoSaveEnabled_ = true;
    int autoSaveInterval_ = 60;  // 默认 60 秒自动保存
    
    std::chrono::system_clock::time_point lastSaveTime_;
    std::chrono::system_clock::time_point lastAutoSaveTime_;
    
    // 回调函数
    DataChangeCallback dataChangeCallback_;
    SaveCompleteCallback saveCompleteCallback_;
    
    // 设置缓存
    std::map<std::string, std::string> settingsCache_;
    
    static std::weak_ptr<DataManager> instance_;
    
    // 初始化数据库表
    bool initializeTables();
    
    // 执行 SQL 查询
    bool executeSQL(const std::string& sql, std::string* errorMsg = nullptr);
    
    // 触发数据变更回调
    void onDataChange(DataType type);
    
    // 触发保存完成回调
    void onSaveComplete(const SaveResult& result);
    
    // 检查是否需要自动保存
    void checkAutoSave();
    
    // 获取当前时间戳
    int64_t getCurrentTimestamp() const;
};

// 数据类型名称映射
inline std::string getDataTypeName(DataType type) {
    switch (type) {
        case DataType::Pet: return "宠物";
        case DataType::Points: return "积分";
        case DataType::Exchange: return "兑换";
        case DataType::Settings: return "设置";
        default: return "未知";
    }
}

} // namespace pet

#endif // DATAMANAGER_H
