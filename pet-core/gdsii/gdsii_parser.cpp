#include "gdsii_parser.h"
#include <sstream>
#include <cstring>
#include <iostream>

namespace gdsii {

// GDSII 实数格式：8 字节，IEEE754 双精度
// 但 GDSII 使用自己的格式，需要转换
static double convertGdsiiReal(const uint8_t* data) {
    // GDSII 实数格式：
    // 1 bit sign, 7 bit exponent (base 16), 56 bit mantissa
    // 公式：value = sign * mantissa * 16^(exponent-64)
    
    uint64_t value = 0;
    for (int i = 0; i < 8; ++i) {
        value = (value << 8) | data[i];
    }
    
    if (value == 0) return 0.0;
    
    int sign = (value & 0x8000000000000000ULL) ? -1 : 1;
    int exponent = ((value >> 56) & 0x7F) - 64;
    uint64_t mantissa = value & 0x00FFFFFFFFFFFFFFULL;
    
    double m = static_cast<double>(mantissa) / static_cast<double>(1ULL << 56);
    double result = sign * m;
    
    // 转换为以 16 为底的指数
    for (int i = 0; i < exponent; ++i) result *= 16.0;
    for (int i = 0; i < -exponent; ++i) result /= 16.0;
    
    return result;
}

std::unique_ptr<Library> GdsiiParser::parse(const std::string& filename, 
                                             const ParserOptions& options) {
    auto library = std::make_unique<Library>();
    
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw GdsiiException("无法打开文件：" + filename);
    }
    
    Structure* currentStructure = nullptr;
    Boundary* currentBoundary = nullptr;
    Path* currentPath = nullptr;
    Text* currentText = nullptr;
    
    uint16_t recordType, dataLength;
    
    while (readRecordHeader(file, recordType, dataLength)) {
        auto type = static_cast<RecordType>(recordType);
        
        if (options.verbose) {
            std::cout << "记录类型：0x" << std::hex << recordType 
                      << std::dec << ", 长度：" << dataLength << std::endl;
        }
        
        switch (type) {
            case RecordType::HEADER:
                parseHeader(file, dataLength, *library);
                break;
                
            case RecordType::BGNLIB:
                parseBgnlib(file, dataLength, *library);
                break;
                
            case RecordType::LIBNAME:
                parseLibname(file, dataLength, *library);
                break;
                
            case RecordType::UNITS:
                parseUnits(file, dataLength, *library);
                break;
                
            case RecordType::BGNSTR:
                parseBgnstr(file, dataLength, *library->structures.emplace_back());
                currentStructure = &library->structures.back();
                break;
                
            case RecordType::STRNAME:
                if (currentStructure) {
                    parseStrname(file, dataLength, *currentStructure);
                }
                break;
                
            case RecordType::BGNXY:
            case RecordType::BGNBOXY:
                // 开始边界
                if (currentStructure) {
                    currentBoundary = new Boundary();
                }
                // 读取坐标（在 BGNXY 中）
                if (dataLength > 0) {
                    auto coords = readInt32Array(file, dataLength);
                    if (currentBoundary && coords.size() >= 2) {
                        currentBoundary->points.emplace_back(coords[0], coords[1]);
                    }
                }
                break;
                
            case RecordType::ENDXY:
                // 边界坐标结束
                if (dataLength > 0 && currentBoundary) {
                    auto coords = readInt32Array(file, dataLength);
                    for (size_t i = 0; i + 1 < coords.size(); i += 2) {
                        currentBoundary->points.emplace_back(coords[i], coords[i + 1]);
                    }
                }
                break;
                
            case RecordType::LAYER:
                {
                    auto layers = readInt16Array(file, dataLength);
                    if (!layers.empty()) {
                        if (currentBoundary) currentBoundary->layer = layers[0];
                        if (currentPath) currentPath->layer = layers[0];
                        if (currentText) currentText->layer = layers[0];
                    }
                }
                break;
                
            case RecordType::DATATYPE:
                {
                    auto datatypes = readInt16Array(file, dataLength);
                    if (!datatypes.empty()) {
                        if (currentBoundary) currentBoundary->datatype = datatypes[0];
                        if (currentPath) currentPath->datatype = datatypes[0];
                    }
                }
                break;
                
            case RecordType::TEXTTYPE:
                {
                    auto texttypes = readInt16Array(file, dataLength);
                    if (!texttypes.empty() && currentText) {
                        currentText->texttype = texttypes[0];
                    }
                }
                break;
                
            case RecordType::BGNPATH:
                if (currentStructure) {
                    currentPath = new Path();
                }
                break;
                
            case RecordType::PATH:
                if (currentPath && dataLength > 0) {
                    auto coords = readInt32Array(file, dataLength);
                    for (size_t i = 0; i + 1 < coords.size(); i += 2) {
                        currentPath->points.emplace_back(coords[i], coords[i + 1]);
                    }
                }
                break;
                
            case RecordType::PATHTYPE:
                {
                    auto types = readInt16Array(file, dataLength);
                    if (!types.empty() && currentPath) {
                        currentPath->pathtype = types[0];
                    }
                }
                break;
                
            case RecordType::PATHWIDTH:
                {
                    auto widths = readInt32Array(file, dataLength);
                    if (!widths.empty() && currentPath) {
                        currentPath->width = widths[0];
                    }
                }
                break;
                
            case RecordType::ENDPATH:
                if (currentPath && currentStructure) {
                    if (options.filter.isEmpty() || 
                        isLayerFiltered(currentPath->layer, currentPath->datatype, options.filter)) {
                        currentStructure->elements.emplace_back(std::move(*currentPath));
                        if (options.collectLayerInfo) {
                            updateLayerInfo(*library, currentPath->layer, currentPath->datatype);
                        }
                    }
                    delete currentPath;
                    currentPath = nullptr;
                }
                break;
                
            case RecordType::BGNTEXT:
                if (currentStructure) {
                    currentText = new Text();
                }
                break;
                
            case RecordType::TEXT:
                if (currentText && dataLength > 0) {
                    currentText->text = readString(file, dataLength);
                }
                break;
                
            case RecordType::XY:
                if (currentText && dataLength > 0) {
                    auto coords = readInt32Array(file, dataLength);
                    for (size_t i = 0; i + 1 < coords.size(); i += 2) {
                        currentText->points.emplace_back(coords[i], coords[i + 1]);
                    }
                }
                break;
                
            case RecordType::PRESENTATION:
                {
                    auto presentations = readInt16Array(file, dataLength);
                    if (!presentations.empty() && currentText) {
                        currentText->presentation = presentations[0];
                    }
                }
                break;
                
            case RecordType::ENDTEXT:
                if (currentText && currentStructure) {
                    if (options.filter.isEmpty() || 
                        isLayerFiltered(currentText->layer, currentText->texttype, options.filter)) {
                        currentStructure->elements.emplace_back(std::move(*currentText));
                        if (options.collectLayerInfo) {
                            updateLayerInfo(*library, currentText->layer, currentText->texttype);
                        }
                    }
                    delete currentText;
                    currentText = nullptr;
                }
                break;
                
            case RecordType::ENDEL:
                // 元素结束，清理
                if (currentBoundary && currentStructure) {
                    if (options.filter.isEmpty() || 
                        isLayerFiltered(currentBoundary->layer, currentBoundary->datatype, options.filter)) {
                        currentStructure->elements.emplace_back(std::move(*currentBoundary));
                        if (options.collectLayerInfo) {
                            updateLayerInfo(*library, currentBoundary->layer, currentBoundary->datatype);
                        }
                    }
                    delete currentBoundary;
                    currentBoundary = nullptr;
                }
                break;
                
            case RecordType::ENDSTR:
                currentStructure = nullptr;
                break;
                
            case RecordType::ENDLIB:
                // 文件结束
                goto done;
                
            default:
                // 跳过未知记录
                if (dataLength > 0) {
                    file.seekg(dataLength, std::ios::cur);
                }
                break;
        }
    }
    
done:
    // 清理未完成的元素
    delete currentBoundary;
    delete currentPath;
    delete currentText;
    
    return library;
}

void GdsiiParser::parseStream(const std::string& filename,
                               std::function<void(const Structure&, const GeometryElement&)> elementCallback,
                               const ParserOptions& options) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw GdsiiException("无法打开文件：" + filename);
    }
    
    Structure currentStructure;
    Boundary currentBoundary;
    Path currentPath;
    Text currentText;
    
    enum class ElementState { None, Boundary, Path, Text };
    ElementState state = ElementState::None;
    
    uint16_t recordType, dataLength;
    
    while (readRecordHeader(file, recordType, dataLength)) {
        auto type = static_cast<RecordType>(recordType);
        
        switch (type) {
            case RecordType::BGNSTR:
                currentStructure = Structure();
                break;
                
            case RecordType::STRNAME:
                currentStructure.name = readString(file, dataLength);
                break;
                
            case RecordType::BGNXY:
            case RecordType::BGNBOXY:
                state = ElementState::Boundary;
                currentBoundary = Boundary();
                if (dataLength > 0) {
                    auto coords = readInt32Array(file, dataLength);
                    if (coords.size() >= 2) {
                        currentBoundary.points.emplace_back(coords[0], coords[1]);
                    }
                }
                break;
                
            case RecordType::ENDXY:
                if (state == ElementState::Boundary && dataLength > 0) {
                    auto coords = readInt32Array(file, dataLength);
                    for (size_t i = 0; i + 1 < coords.size(); i += 2) {
                        currentBoundary.points.emplace_back(coords[i], coords[i + 1]);
                    }
                }
                break;
                
            case RecordType::LAYER:
                {
                    auto layers = readInt16Array(file, dataLength);
                    if (!layers.empty()) {
                        if (state == ElementState::Boundary) currentBoundary.layer = layers[0];
                        if (state == ElementState::Path) currentPath.layer = layers[0];
                        if (state == ElementState::Text) currentText.layer = layers[0];
                    }
                }
                break;
                
            case RecordType::DATATYPE:
                {
                    auto datatypes = readInt16Array(file, dataLength);
                    if (!datatypes.empty() && state == ElementState::Boundary) {
                        currentBoundary.datatype = datatypes[0];
                    }
                    if (!datatypes.empty() && state == ElementState::Path) {
                        currentPath.datatype = datatypes[0];
                    }
                }
                break;
                
            case RecordType::BGNPATH:
                state = ElementState::Path;
                currentPath = Path();
                break;
                
            case RecordType::PATH:
                if (state == ElementState::Path && dataLength > 0) {
                    auto coords = readInt32Array(file, dataLength);
                    for (size_t i = 0; i + 1 < coords.size(); i += 2) {
                        currentPath.points.emplace_back(coords[i], coords[i + 1]);
                    }
                }
                break;
                
            case RecordType::PATHTYPE:
                {
                    auto types = readInt16Array(file, dataLength);
                    if (!types.empty() && state == ElementState::Path) {
                        currentPath.pathtype = types[0];
                    }
                }
                break;
                
            case RecordType::PATHWIDTH:
                {
                    auto widths = readInt32Array(file, dataLength);
                    if (!widths.empty() && state == ElementState::Path) {
                        currentPath.width = widths[0];
                    }
                }
                break;
                
            case RecordType::ENDPATH:
                if (state == ElementState::Path) {
                    if (options.filter.isEmpty() || 
                        isLayerFiltered(currentPath.layer, currentPath.datatype, options.filter)) {
                        elementCallback(currentStructure, currentPath);
                    }
                    state = ElementState::None;
                }
                break;
                
            case RecordType::BGNTEXT:
                state = ElementState::Text;
                currentText = Text();
                break;
                
            case RecordType::TEXT:
                if (state == ElementState::Text && dataLength > 0) {
                    currentText.text = readString(file, dataLength);
                }
                break;
                
            case RecordType::XY:
                if (state == ElementState::Text && dataLength > 0) {
                    auto coords = readInt32Array(file, dataLength);
                    for (size_t i = 0; i + 1 < coords.size(); i += 2) {
                        currentText.points.emplace_back(coords[i], coords[i + 1]);
                    }
                }
                break;
                
            case RecordType::TEXTTYPE:
                {
                    auto texttypes = readInt16Array(file, dataLength);
                    if (!texttypes.empty() && state == ElementState::Text) {
                        currentText.texttype = texttypes[0];
                    }
                }
                break;
                
            case RecordType::ENDTEXT:
                if (state == ElementState::Text) {
                    if (options.filter.isEmpty() || 
                        isLayerFiltered(currentText.layer, currentText.texttype, options.filter)) {
                        elementCallback(currentStructure, currentText);
                    }
                    state = ElementState::None;
                }
                break;
                
            case RecordType::ENDEL:
                if (state == ElementState::Boundary) {
                    if (options.filter.isEmpty() || 
                        isLayerFiltered(currentBoundary.layer, currentBoundary.datatype, options.filter)) {
                        elementCallback(currentStructure, currentBoundary);
                    }
                    state = ElementState::None;
                }
                break;
                
            case RecordType::ENDSTR:
                currentStructure = Structure();
                break;
                
            case RecordType::ENDLIB:
                goto done;
                
            default:
                if (dataLength > 0) {
                    file.seekg(dataLength, std::ios::cur);
                }
                break;
        }
    }
    
done:
    return;
}

std::vector<LayerInfo> GdsiiParser::getLayerList(const Library& library) const {
    return library.layerInfos;
}

bool GdsiiParser::readRecordHeader(std::ifstream& file, uint16_t& recordType, uint16_t& dataLength) {
    uint8_t header[4];
    file.read(reinterpret_cast<char*>(header), 4);
    
    if (file.gcount() != 4) {
        return false;
    }
    
    // GDSII 使用大端字节序
    dataLength = (static_cast<uint16_t>(header[0]) << 8) | header[1];
    recordType = (static_cast<uint16_t>(header[2]) << 8) | header[3];
    
    // 数据长度至少为 4（头部本身），最多 65534
    if (dataLength < 4 || dataLength > 65534) {
        return false;
    }
    
    return true;
}

std::vector<int16_t> GdsiiParser::readInt16Array(std::ifstream& file, uint16_t dataLength) {
    std::vector<int16_t> result;
    size_t count = (dataLength - 0) / 2;  // 减去头部已在 readRecordHeader 中读取
    
    for (size_t i = 0; i < count; ++i) {
        uint8_t bytes[2];
        file.read(reinterpret_cast<char*>(bytes), 2);
        if (file.gcount() != 2) break;
        
        int16_t value = (static_cast<int16_t>(bytes[0]) << 8) | bytes[1];
        result.push_back(value);
    }
    
    return result;
}

std::vector<int32_t> GdsiiParser::readInt32Array(std::ifstream& file, uint16_t dataLength) {
    std::vector<int32_t> result;
    size_t count = dataLength / 4;
    
    for (size_t i = 0; i < count; ++i) {
        uint8_t bytes[4];
        file.read(reinterpret_cast<char*>(bytes), 4);
        if (file.gcount() != 4) break;
        
        int32_t value = (static_cast<int32_t>(bytes[0]) << 24) |
                        (static_cast<int32_t>(bytes[1]) << 16) |
                        (static_cast<int32_t>(bytes[2]) << 8) |
                        bytes[3];
        result.push_back(value);
    }
    
    return result;
}

std::string GdsiiParser::readString(std::ifstream& file, uint16_t dataLength) {
    std::vector<char> buffer(dataLength);
    file.read(buffer.data(), dataLength);
    
    // GDSII 字符串以偶数长度存储，可能包含填充字节
    std::string result(buffer.data());
    
    // 移除可能的空终止符和填充
    size_t nullPos = result.find('\0');
    if (nullPos != std::string::npos) {
        result = result.substr(0, nullPos);
    }
    
    return result;
}

std::vector<double> GdsiiParser::readReal64Array(std::ifstream& file, uint16_t dataLength) {
    std::vector<double> result;
    size_t count = dataLength / 8;
    
    for (size_t i = 0; i < count; ++i) {
        uint8_t bytes[8];
        file.read(reinterpret_cast<char*>(bytes), 8);
        if (file.gcount() != 8) break;
        
        result.push_back(convertGdsiiReal(bytes));
    }
    
    return result;
}

void GdsiiParser::parseHeader(std::ifstream& file, uint16_t dataLength, Library& lib) {
    // HEADER 记录包含格式版本号（通常是 6 字节）
    file.seekg(dataLength, std::ios::cur);
}

void GdsiiParser::parseBgnlib(std::ifstream& file, uint16_t dataLength, Library& lib) {
    // BGNLIB 包含时间戳（12 字节）
    file.seekg(dataLength, std::ios::cur);
}

void GdsiiParser::parseLibname(std::ifstream& file, uint16_t dataLength, Library& lib) {
    lib.name = readString(file, dataLength);
}

void GdsiiParser::parseUnits(std::ifstream& file, uint16_t dataLength, Library& lib) {
    auto values = readReal64Array(file, dataLength);
    if (values.size() >= 2) {
        lib.units = values[0];
        lib.precision = values[1];
    }
}

void GdsiiParser::parseBgnstr(std::ifstream& file, uint16_t dataLength, Structure& str) {
    // BGNSTR 包含时间戳（12 字节）
    file.seekg(dataLength, std::ios::cur);
}

void GdsiiParser::parseStrname(std::ifstream& file, uint16_t dataLength, Structure& str) {
    str.name = readString(file, dataLength);
}

bool GdsiiParser::isLayerFiltered(int16_t layer, int16_t datatype, const LayerFilter& filter) const {
    return filter.matches(layer, datatype);
}

void GdsiiParser::updateLayerInfo(Library& lib, int16_t layer, int16_t datatype) {
    for (auto& info : lib.layerInfos) {
        if (info.layer == layer && info.datatype == datatype) {
            info.elementCount++;
            return;
        }
    }
    lib.layerInfos.emplace_back(layer, datatype);
}

} // namespace gdsii
