#ifndef GDSII_PARSER_H
#define GDSII_PARSER_H

#include "gdsii_types.h"
#include <fstream>
#include <memory>
#include <functional>
#include <stdexcept>

namespace gdsii {

// GDSII 解析异常
class GdsiiException : public std::runtime_error {
public:
    explicit GdsiiException(const std::string& msg) : std::runtime_error(msg) {}
};

// 解析器选项
struct ParserOptions {
    bool streamMode = true;          // 流式解析模式
    LayerFilter filter;              // 图层过滤器
    bool collectLayerInfo = true;    // 收集图层信息
    bool verbose = false;            // 详细输出
    
    ParserOptions() = default;
};

// GDSII 解析器
class GdsiiParser {
public:
    GdsiiParser() = default;
    ~GdsiiParser() = default;
    
    // 禁止拷贝，允许移动
    GdsiiParser(const GdsiiParser&) = delete;
    GdsiiParser& operator=(const GdsiiParser&) = delete;
    GdsiiParser(GdsiiParser&&) = default;
    GdsiiParser& operator=(GdsiiParser&&) = default;
    
    /**
     * @brief 解析 GDSII 文件
     * @param filename 文件路径
     * @param options 解析选项
     * @return 解析后的库对象
     */
    std::unique_ptr<Library> parse(const std::string& filename, 
                                    const ParserOptions& options = ParserOptions());
    
    /**
     * @brief 流式解析 GDSII 文件
     * @param filename 文件路径
     * @param elementCallback 每个元素的回调函数
     * @param options 解析选项
     */
    void parseStream(const std::string& filename,
                     std::function<void(const Structure&, const GeometryElement&)> elementCallback,
                     const ParserOptions& options = ParserOptions());
    
    /**
     * @brief 获取图层列表
     * @param library 库对象
     * @return 图层信息列表
     */
    std::vector<LayerInfo> getLayerList(const Library& library) const;
    
private:
    // 读取记录头
    bool readRecordHeader(std::ifstream& file, uint16_t& recordType, uint16_t& dataLength);
    
    // 读取数据
    template<typename T>
    T readData(std::ifstream& file, uint16_t dataLength);
    
    std::vector<int16_t> readInt16Array(std::ifstream& file, uint16_t dataLength);
    std::vector<int32_t> readInt32Array(std::ifstream& file, uint16_t dataLength);
    std::string readString(std::ifstream& file, uint16_t dataLength);
    std::vector<double> readReal64Array(std::ifstream& file, uint16_t dataLength);
    
    // 解析特定记录
    void parseHeader(std::ifstream& file, uint16_t dataLength, Library& lib);
    void parseBgnlib(std::ifstream& file, uint16_t dataLength, Library& lib);
    void parseLibname(std::ifstream& file, uint16_t dataLength, Library& lib);
    void parseUnits(std::ifstream& file, uint16_t dataLength, Library& lib);
    void parseBgnstr(std::ifstream& file, uint16_t dataLength, Structure& str);
    void parseStrname(std::ifstream& file, uint16_t dataLength, Structure& str);
    
    // 解析几何元素
    std::unique_ptr<Boundary> parseBoundary(std::ifstream& file);
    std::unique_ptr<Path> parsePath(std::ifstream& file);
    std::unique_ptr<Text> parseText(std::ifstream& file);
    
    // 辅助函数
    bool isLayerFiltered(int16_t layer, int16_t datatype, const LayerFilter& filter) const;
    void updateLayerInfo(Library& lib, int16_t layer, int16_t datatype);
};

} // namespace gdsii

#endif // GDSII_PARSER_H
