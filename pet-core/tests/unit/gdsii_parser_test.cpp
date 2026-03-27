/**
 * @file gdsii_parser_test.cpp
 * @brief GDSII 解析器单元测试
 * @author EDA-CPP-Dev
 * @date 2026-03-27
 * 
 * 测试覆盖率目标：> 80%
 */

#include <gtest/gtest.h>
#include <fstream>
#include <memory>
#include <vector>
#include <string>
#include <filesystem>

#include "gdsii/gdsii_parser.h"
#include "gdsii/gdsii_types.h"

namespace fs = std::filesystem;

using namespace gdsii;

/**
 * @brief 测试辅助类：创建临时 GDSII 文件
 */
class GdsiiTestFile {
public:
    GdsiiTestFile() {
        // 创建临时文件路径
        tempPath_ = fs::temp_directory_path() / "test_gdsii_" + 
                    std::to_string(reinterpret_cast<uintptr_t>(this)) + ".gds";
    }
    
    ~GdsiiTestFile() {
        // 清理临时文件
        if (fs::exists(tempPath_)) {
            fs::remove(tempPath_);
        }
    }
    
    const std::string& path() const { return tempPath_.string(); }
    
    /**
     * @brief 写入 GDSII 记录头
     */
    void writeRecordHeader(std::ofstream& file, RecordType type, uint16_t dataLength) {
        uint16_t totalLength = dataLength + 4;  // 包含头部
        uint8_t header[4];
        header[0] = (totalLength >> 8) & 0xFF;
        header[1] = totalLength & 0xFF;
        header[2] = (static_cast<uint16_t>(type) >> 8) & 0xFF;
        header[3] = static_cast<uint16_t>(type) & 0xFF;
        file.write(reinterpret_cast<char*>(header), 4);
    }
    
    /**
     * @brief 写入 16 位整数数组
     */
    void writeInt16Array(std::ofstream& file, const std::vector<int16_t>& data) {
        for (int16_t value : data) {
            uint8_t bytes[2];
            bytes[0] = (value >> 8) & 0xFF;
            bytes[1] = value & 0xFF;
            file.write(reinterpret_cast<char*>(bytes), 2);
        }
    }
    
    /**
     * @brief 写入 32 位整数数组
     */
    void writeInt32Array(std::ofstream& file, const std::vector<int32_t>& data) {
        for (int32_t value : data) {
            uint8_t bytes[4];
            bytes[0] = (value >> 24) & 0xFF;
            bytes[1] = (value >> 16) & 0xFF;
            bytes[2] = (value >> 8) & 0xFF;
            bytes[3] = value & 0xFF;
            file.write(reinterpret_cast<char*>(bytes), 4);
        }
    }
    
    /**
     * @brief 写入字符串
     */
    void writeString(std::ofstream& file, const std::string& str) {
        // GDSII 字符串需要偶数长度
        std::string padded = str;
        if (padded.length() % 2 != 0) {
            padded += '\0';
        }
        file.write(padded.c_str(), padded.length());
    }
    
    /**
     * @brief 创建简单的测试 GDSII 文件
     */
    void createSimpleGdsii() {
        std::ofstream file(tempPath_, std::ios::binary);
        ASSERT_TRUE(file.is_open());
        
        // HEADER
        writeRecordHeader(file, RecordType::HEADER, 6);
        writeInt16Array(file, {6, 0, 0});  // 版本号
        
        // BGNLIB
        writeRecordHeader(file, RecordType::BGNLIB, 12);
        writeInt32Array(file, {0, 0, 0});  // 时间戳
        
        // LIBNAME
        writeRecordHeader(file, RecordType::LIBNAME, 10);
        writeString(file, "TEST_LIB");
        
        // UNITS
        writeRecordHeader(file, RecordType::UNITS, 16);
        // 写入简单的单位值（简化）
        writeInt32Array(file, {0, 1, 0, 0});
        
        // BGNSTR
        writeRecordHeader(file, RecordType::BGNSTR, 12);
        writeInt32Array(file, {0, 0, 0});
        
        // STRNAME
        writeRecordHeader(file, RecordType::STRNAME, 10);
        writeString(file, "CELL_A");
        
        // BOUNDARY - Layer 1
        writeRecordHeader(file, RecordType::BGNXY, 8);
        writeInt32Array(file, {0, 0});
        
        writeRecordHeader(file, RecordType::LAYER, 2);
        writeInt16Array(file, {1});
        
        writeRecordHeader(file, RecordType::DATATYPE, 2);
        writeInt16Array(file, {0});
        
        writeRecordHeader(file, RecordType::ENDXY, 32);
        writeInt32Array(file, {1000, 0, 1000, 1000, 0, 1000, 0, 0});
        
        // ENDEL
        writeRecordHeader(file, RecordType::ENDEL, 0);
        
        // ENDSTR
        writeRecordHeader(file, RecordType::ENDSTR, 0);
        
        // ENDLIB
        writeRecordHeader(file, RecordType::ENDLIB, 0);
        
        file.close();
    }
    
    /**
     * @brief 创建包含多个图层的测试文件
     */
    void createMultiLayerGdsii() {
        std::ofstream file(tempPath_, std::ios::binary);
        ASSERT_TRUE(file.is_open());
        
        // HEADER
        writeRecordHeader(file, RecordType::HEADER, 6);
        writeInt16Array(file, {6, 0, 0});
        
        // BGNLIB
        writeRecordHeader(file, RecordType::BGNLIB, 12);
        writeInt32Array(file, {0, 0, 0});
        
        // LIBNAME
        writeRecordHeader(file, RecordType::LIBNAME, 12);
        writeString(file, "MULTI_LAYER");
        
        // UNITS
        writeRecordHeader(file, RecordType::UNITS, 16);
        writeInt32Array(file, {0, 1, 0, 0});
        
        // 创建多个结构，每个结构在不同图层
        for (int layer = 1; layer <= 5; ++layer) {
            // BGNSTR
            writeRecordHeader(file, RecordType::BGNSTR, 12);
            writeInt32Array(file, {0, 0, 0});
            
            // STRNAME
            std::string strName = "CELL_L" + std::to_string(layer);
            writeRecordHeader(file, RecordType::STRNAME, static_cast<uint16_t>(strName.length() + (strName.length() % 2)));
            writeString(file, strName);
            
            // BOUNDARY
            writeRecordHeader(file, RecordType::BGNXY, 8);
            writeInt32Array(file, {layer * 100, layer * 100});
            
            writeRecordHeader(file, RecordType::LAYER, 2);
            writeInt16Array(file, {static_cast<int16_t>(layer)});
            
            writeRecordHeader(file, RecordType::DATATYPE, 2);
            writeInt16Array(file, {0});
            
            writeRecordHeader(file, RecordType::ENDXY, 32);
            writeInt32Array(file, {
                layer * 100 + 100, layer * 100,
                layer * 100 + 100, layer * 100 + 100,
                layer * 100, layer * 100 + 100,
                layer * 100, layer * 100
            });
            
            // ENDEL
            writeRecordHeader(file, RecordType::ENDEL, 0);
            
            // ENDSTR
            writeRecordHeader(file, RecordType::ENDSTR, 0);
        }
        
        // ENDLIB
        writeRecordHeader(file, RecordType::ENDLIB, 0);
        
        file.close();
    }
    
    /**
     * @brief 创建包含路径元素的测试文件
     */
    void createPathGdsii() {
        std::ofstream file(tempPath_, std::ios::binary);
        ASSERT_TRUE(file.is_open());
        
        // HEADER
        writeRecordHeader(file, RecordType::HEADER, 6);
        writeInt16Array(file, {6, 0, 0});
        
        // BGNLIB
        writeRecordHeader(file, RecordType::BGNLIB, 12);
        writeInt32Array(file, {0, 0, 0});
        
        // LIBNAME
        writeRecordHeader(file, RecordType::LIBNAME, 10);
        writeString(file, "PATH_TEST");
        
        // UNITS
        writeRecordHeader(file, RecordType::UNITS, 16);
        writeInt32Array(file, {0, 1, 0, 0});
        
        // BGNSTR
        writeRecordHeader(file, RecordType::BGNSTR, 12);
        writeInt32Array(file, {0, 0, 0});
        
        // STRNAME
        writeRecordHeader(file, RecordType::STRNAME, 10);
        writeString(file, "PATH_CELL");
        
        // BGNPATH
        writeRecordHeader(file, RecordType::BGNPATH, 0);
        
        // LAYER
        writeRecordHeader(file, RecordType::LAYER, 2);
        writeInt16Array(file, {2});
        
        // DATATYPE
        writeRecordHeader(file, RecordType::DATATYPE, 2);
        writeInt16Array(file, {1});
        
        // PATHTYPE
        writeRecordHeader(file, RecordType::PATHTYPE, 2);
        writeInt16Array(file, {0});
        
        // PATHWIDTH
        writeRecordHeader(file, RecordType::PATHWIDTH, 4);
        writeInt32Array(file, {100});
        
        // PATH
        writeRecordHeader(file, RecordType::PATH, 24);
        writeInt32Array(file, {0, 0, 500, 0, 500, 500});
        
        // ENDPATH
        writeRecordHeader(file, RecordType::ENDPATH, 0);
        
        // ENDEL
        writeRecordHeader(file, RecordType::ENDEL, 0);
        
        // ENDSTR
        writeRecordHeader(file, RecordType::ENDSTR, 0);
        
        // ENDLIB
        writeRecordHeader(file, RecordType::ENDLIB, 0);
        
        file.close();
    }
    
    /**
     * @brief 创建包含文本元素的测试文件
     */
    void createTextGdsii() {
        std::ofstream file(tempPath_, std::ios::binary);
        ASSERT_TRUE(file.is_open());
        
        // HEADER
        writeRecordHeader(file, RecordType::HEADER, 6);
        writeInt16Array(file, {6, 0, 0});
        
        // BGNLIB
        writeRecordHeader(file, RecordType::BGNLIB, 12);
        writeInt32Array(file, {0, 0, 0});
        
        // LIBNAME
        writeRecordHeader(file, RecordType::LIBNAME, 10);
        writeString(file, "TEXT_TEST");
        
        // UNITS
        writeRecordHeader(file, RecordType::UNITS, 16);
        writeInt32Array(file, {0, 1, 0, 0});
        
        // BGNSTR
        writeRecordHeader(file, RecordType::BGNSTR, 12);
        writeInt32Array(file, {0, 0, 0});
        
        // STRNAME
        writeRecordHeader(file, RecordType::STRNAME, 12);
        writeString(file, "TEXT_CELL");
        
        // BGNTEXT
        writeRecordHeader(file, RecordType::BGNTEXT, 0);
        
        // LAYER
        writeRecordHeader(file, RecordType::LAYER, 2);
        writeInt16Array(file, {3});
        
        // TEXTTYPE
        writeRecordHeader(file, RecordType::TEXTTYPE, 2);
        writeInt16Array(file, {0});
        
        // PRESENTATION
        writeRecordHeader(file, RecordType::PRESENTATION, 2);
        writeInt16Array(file, {0});
        
        // TEXT
        writeRecordHeader(file, RecordType::TEXT, 6);
        writeString(file, "VDD");
        
        // XY
        writeRecordHeader(file, RecordType::XY, 8);
        writeInt32Array(file, {1000, 1000});
        
        // ENDTEXT
        writeRecordHeader(file, RecordType::ENDTEXT, 0);
        
        // ENDEL
        writeRecordHeader(file, RecordType::ENDEL, 0);
        
        // ENDSTR
        writeRecordHeader(file, RecordType::ENDSTR, 0);
        
        // ENDLIB
        writeRecordHeader(file, RecordType::ENDLIB, 0);
        
        file.close();
    }

private:
    fs::path tempPath_;
};

// ==================== 测试用例 ====================

/**
 * @brief 测试：解析简单 GDSII 文件
 */
TEST(GdsiiParserTest, ParseSimpleFile) {
    GdsiiTestFile testFile;
    testFile.createSimpleGdsii();
    
    GdsiiParser parser;
    ParserOptions options;
    options.verbose = false;
    
    auto library = parser.parse(testFile.path(), options);
    
    ASSERT_NE(library, nullptr);
    EXPECT_EQ(library->name, "TEST_LIB");
    EXPECT_EQ(library->structures.size(), 1);
    
    if (!library->structures.empty()) {
        EXPECT_EQ(library->structures[0].name, "CELL_A");
        EXPECT_GT(library->structures[0].elements.size(), 0);
    }
}

/**
 * @brief 测试：解析不存在的文件应抛出异常
 */
TEST(GdsiiParserTest, ParseNonExistentFile) {
    GdsiiParser parser;
    
    EXPECT_THROW(parser.parse("/nonexistent/path/file.gds"), GdsiiException);
}

/**
 * @brief 测试：图层过滤功能
 */
TEST(GdsiiParserTest, LayerFilter) {
    GdsiiTestFile testFile;
    testFile.createMultiLayerGdsii();
    
    GdsiiParser parser;
    ParserOptions options;
    
    // 只过滤图层 2 和 4
    options.filter.layers = std::vector<int16_t>{2, 4};
    
    auto library = parser.parse(testFile.path(), options);
    
    ASSERT_NE(library, nullptr);
    
    // 验证过滤结果
    size_t filteredCount = 0;
    for (const auto& structure : library->structures) {
        for (const auto& element : structure.elements) {
            if (std::holds_alternative<Boundary>(element)) {
                const auto& boundary = std::get<Boundary>(element);
                if (boundary.layer == 2 || boundary.layer == 4) {
                    filteredCount++;
                }
            }
        }
    }
    
    // 应该只有图层 2 和 4 的元素
    EXPECT_GT(filteredCount, 0);
}

/**
 * @brief 测试：解析路径元素
 */
TEST(GdsiiParserTest, ParsePathElement) {
    GdsiiTestFile testFile;
    testFile.createPathGdsii();
    
    GdsiiParser parser;
    auto library = parser.parse(testFile.path());
    
    ASSERT_NE(library, nullptr);
    EXPECT_EQ(library->structures.size(), 1);
    
    if (!library->structures.empty()) {
        bool foundPath = false;
        for (const auto& element : library->structures[0].elements) {
            if (std::holds_alternative<Path>(element)) {
                const auto& path = std::get<Path>(element);
                foundPath = true;
                EXPECT_EQ(path.layer, 2);
                EXPECT_EQ(path.datatype, 1);
                EXPECT_EQ(path.width, 100);
                EXPECT_EQ(path.pathtype, 0);
                EXPECT_EQ(path.points.size(), 3);
            }
        }
        EXPECT_TRUE(foundPath);
    }
}

/**
 * @brief 测试：解析文本元素
 */
TEST(GdsiiParserTest, ParseTextElement) {
    GdsiiTestFile testFile;
    testFile.createTextGdsii();
    
    GdsiiParser parser;
    auto library = parser.parse(testFile.path());
    
    ASSERT_NE(library, nullptr);
    
    if (!library->structures.empty()) {
        bool foundText = false;
        for (const auto& element : library->structures[0].elements) {
            if (std::holds_alternative<Text>(element)) {
                const auto& text = std::get<Text>(element);
                foundText = true;
                EXPECT_EQ(text.text, "VDD");
                EXPECT_EQ(text.layer, 3);
                EXPECT_EQ(text.points.size(), 1);
                EXPECT_EQ(text.points[0].x, 1000);
                EXPECT_EQ(text.points[0].y, 1000);
            }
        }
        EXPECT_TRUE(foundText);
    }
}

/**
 * @brief 测试：图层信息收集
 */
TEST(GdsiiParserTest, LayerInfoCollection) {
    GdsiiTestFile testFile;
    testFile.createMultiLayerGdsii();
    
    GdsiiParser parser;
    ParserOptions options;
    options.collectLayerInfo = true;
    
    auto library = parser.parse(testFile.path(), options);
    
    ASSERT_NE(library, nullptr);
    EXPECT_GT(library->layerInfos.size(), 0);
    
    // 验证图层信息
    std::vector<int16_t> foundLayers;
    for (const auto& info : library->layerInfos) {
        foundLayers.push_back(info.layer);
    }
    
    // 应该包含图层 1-5
    for (int layer = 1; layer <= 5; ++layer) {
        EXPECT_TRUE(std::find(foundLayers.begin(), foundLayers.end(), layer) != foundLayers.end());
    }
}

/**
 * @brief 测试：流式解析
 */
TEST(GdsiiParserTest, StreamParsing) {
    GdsiiTestFile testFile;
    testFile.createSimpleGdsii();
    
    GdsiiParser parser;
    
    size_t elementCount = 0;
    std::string lastStructureName;
    
    parser.parseStream(testFile.path(),
        [&elementCount, &lastStructureName](const Structure& structure, const GeometryElement& element) {
            elementCount++;
            lastStructureName = structure.name;
        });
    
    EXPECT_GT(elementCount, 0);
    EXPECT_EQ(lastStructureName, "CELL_A");
}

/**
 * @brief 测试：智能指针使用（无内存泄漏）
 */
TEST(GdsiiParserTest, SmartPointerUsage) {
    GdsiiTestFile testFile;
    testFile.createSimpleGdsii();
    
    GdsiiParser parser;
    
    // 多次解析，验证 unique_ptr 正确管理内存
    for (int i = 0; i < 10; ++i) {
        auto library = parser.parse(testFile.path());
        ASSERT_NE(library, nullptr);
        // library 会自动释放
    }
}

/**
 * @brief 测试：边界元素解析
 */
TEST(GdsiiParserTest, BoundaryElementParsing) {
    GdsiiTestFile testFile;
    testFile.createSimpleGdsii();
    
    GdsiiParser parser;
    auto library = parser.parse(testFile.path());
    
    ASSERT_NE(library, nullptr);
    
    if (!library->structures.empty()) {
        bool foundBoundary = false;
        for (const auto& element : library->structures[0].elements) {
            if (std::holds_alternative<Boundary>(element)) {
                const auto& boundary = std::get<Boundary>(element);
                foundBoundary = true;
                EXPECT_EQ(boundary.layer, 1);
                EXPECT_EQ(boundary.datatype, 0);
                EXPECT_GE(boundary.points.size(), 4);  // 至少 4 个点形成矩形
            }
        }
        EXPECT_TRUE(foundBoundary);
    }
}

/**
 * @brief 测试：空过滤器（不过滤）
 */
TEST(GdsiiParserTest, EmptyLayerFilter) {
    LayerFilter filter;
    
    EXPECT_TRUE(filter.isEmpty());
    EXPECT_TRUE(filter.matches(1, 0));
    EXPECT_TRUE(filter.matches(100, 255));
}

/**
 * @brief 测试：图层过滤器匹配
 */
TEST(GdsiiParserTest, LayerFilterMatching) {
    LayerFilter filter;
    filter.layers = std::vector<int16_t>{1, 3, 5};
    
    EXPECT_TRUE(filter.matches(1, 0));
    EXPECT_TRUE(filter.matches(3, 0));
    EXPECT_TRUE(filter.matches(5, 0));
    EXPECT_FALSE(filter.matches(2, 0));
    EXPECT_FALSE(filter.matches(4, 0));
}

/**
 * @brief 测试：数据类型过滤器
 */
TEST(GdsiiParserTest, DatatypeFilter) {
    LayerFilter filter;
    filter.datatypes = std::vector<int16_t>{0, 2};
    
    EXPECT_TRUE(filter.matches(1, 0));
    EXPECT_TRUE(filter.matches(1, 2));
    EXPECT_FALSE(filter.matches(1, 1));
    EXPECT_FALSE(filter.matches(1, 3));
}

/**
 * @brief 测试：组合过滤器
 */
TEST(GdsiiParserTest, CombinedFilter) {
    LayerFilter filter;
    filter.layers = std::vector<int16_t>{1, 2};
    filter.datatypes = std::vector<int16_t>{0, 1};
    
    EXPECT_TRUE(filter.matches(1, 0));
    EXPECT_TRUE(filter.matches(1, 1));
    EXPECT_TRUE(filter.matches(2, 0));
    EXPECT_TRUE(filter.matches(2, 1));
    EXPECT_FALSE(filter.matches(3, 0));  // 图层不匹配
    EXPECT_FALSE(filter.matches(1, 2));  // 数据类型不匹配
}

/**
 * @brief 测试：Point 结构
 */
TEST(GdsiiParserTest, PointStruct) {
    Point p1;
    EXPECT_EQ(p1.x, 0);
    EXPECT_EQ(p1.y, 0);
    
    Point p2(100, 200);
    EXPECT_EQ(p2.x, 100);
    EXPECT_EQ(p2.y, 200);
}

/**
 * @brief 测试：Boundary 结构
 */
TEST(GdsiiParserTest, BoundaryStruct) {
    Boundary boundary;
    EXPECT_EQ(boundary.layer, 0);
    EXPECT_EQ(boundary.datatype, 0);
    EXPECT_TRUE(boundary.points.empty());
    
    boundary.points.emplace_back(0, 0);
    boundary.points.emplace_back(100, 0);
    boundary.points.emplace_back(100, 100);
    boundary.points.emplace_back(0, 100);
    
    EXPECT_EQ(boundary.points.size(), 4);
}

/**
 * @brief 测试：Path 结构
 */
TEST(GdsiiParserTest, PathStruct) {
    Path path;
    EXPECT_EQ(path.layer, 0);
    EXPECT_EQ(path.datatype, 0);
    EXPECT_EQ(path.pathtype, 0);
    EXPECT_EQ(path.width, 0);
    
    path.width = 200;
    path.pathtype = 1;
    
    EXPECT_EQ(path.width, 200);
    EXPECT_EQ(path.pathtype, 1);
}

/**
 * @brief 测试：Text 结构
 */
TEST(GdsiiParserTest, TextStruct) {
    Text text;
    EXPECT_EQ(text.layer, 0);
    EXPECT_EQ(text.texttype, 0);
    EXPECT_TRUE(text.text.empty());
    
    text.text = "TEST";
    text.points.emplace_back(500, 500);
    
    EXPECT_EQ(text.text, "TEST");
    EXPECT_EQ(text.points.size(), 1);
}

/**
 * @brief 测试：Structure 结构
 */
TEST(GdsiiParserTest, StructureStruct) {
    Structure structure("TEST_CELL");
    EXPECT_EQ(structure.name, "TEST_CELL");
    EXPECT_TRUE(structure.elements.empty());
}

/**
 * @brief 测试：Library 结构
 */
TEST(GdsiiParserTest, LibraryStruct) {
    Library library;
    EXPECT_TRUE(library.name.empty());
    EXPECT_DOUBLE_EQ(library.units, 1.0);
    EXPECT_DOUBLE_EQ(library.precision, 1e-9);
    EXPECT_TRUE(library.structures.empty());
    EXPECT_TRUE(library.layerInfos.empty());
}

/**
 * @brief 测试：GdsiiException
 */
TEST(GdsiiParserTest, GdsiiException) {
    EXPECT_THROW(throw GdsiiException("Test error"), GdsiiException);
    
    try {
        throw GdsiiException("Specific error message");
    } catch (const GdsiiException& e) {
        EXPECT_STREQ(e.what(), "Specific error message");
    }
}

/**
 * @brief 测试：ParserOptions 默认值
 */
TEST(GdsiiParserTest, ParserOptionsDefaults) {
    ParserOptions options;
    EXPECT_TRUE(options.streamMode);
    EXPECT_TRUE(options.collectLayerInfo);
    EXPECT_FALSE(options.verbose);
    EXPECT_TRUE(options.filter.isEmpty());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
