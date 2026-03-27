#ifndef GDSII_TYPES_H
#define GDSII_TYPES_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <variant>
#include <optional>

namespace gdsii {

// GDSII 记录类型定义
enum class RecordType : uint16_t {
    HEADER = 0x0002,
    BGNLIB = 0x0102,
    LIBNAME = 0x0206,
    UNITS = 0x0305,
    BGNSTR = 0x0502,
    STRNAME = 0x0606,
    BGNXY = 0x1003,
    ENDXY = 0x1103,
    BGNPATH = 0x2102,
    PATH = 0x2203,
    PATHTYPE = 0x2203,
    ENDPATH = 0x2300,
    BGNTEXT = 0x2402,
    TEXT = 0x2506,
    TEXTNODE = 0x2602,
    ENDTEXT = 0x2700,
    SREF = 0x2802,
    SREFNAME = 0x2906,
    AREF = 0x2A02,
    AREFNAME = 0x2B06,
    ENDEL = 0x2D00,
    ENDSTR = 0x2E00,
    ENDLIB = 0x2F00,
    LAYER = 0x0D02,
    DATATYPE = 0x0E02,
    TEXTTYPE = 0x1602,
    PRESENTATION = 0x1705,
    STRCLASS = 0x3002,
    XY = 0x1003,
    COLROW = 0x3303,
    NODE = 0x3402,
    TEXTNODE_NAME = 0x3506,
    BGNEXTN = 0x3603,
    ENDEXTN = 0x3703,
    TAPENUM = 0x3802,
    TAPECODE = 0x3902,
    STRNAME_SIZE = 0x3A02,
    REFLIBS = 0x3D06,
    FONTS = 0x3E06,
    PATHWIDTH = 0x3F03,
    GENERATIONS = 0x4002,
    ATTRTABLE = 0x4106,
    FORMAT = 0x4202,
    MASK = 0x4306,
    ENDMASKS = 0x4400,
    LIBDIRSIZE = 0x4502,
    SRFNAME = 0x4606,
    LIBSECUR = 0x4706,
    PROPATTR = 0x4B02,
    PROPVALUE = 0x4C06,
    BOX = 0x4D02,
    BOXY = 0x4E03,
    BGNBOXY = 0x4D02,
    ENDBOXY = 0x5000,
    ELEMENT = 0x5102,
    ELFLAGS = 0x5202,
    ELKEY = 0x5303,
    LINKTYPE = 0x5402,
    LINKKEYS = 0x5503,
    NODETYPE = 0x5602,
    PROP = 0x5700,
    UNCOMPR = 0x5802,
    UNITS2 = 0x5905,
    SIGNATURE = 0x5A01,
    FORMAT2 = 0x5B02,
    DYNAMIC = 0x5C02,
    VALID = 0x5D02,
    ENDDYNAMIC = 0x5E00,
    GOODIN = 0x5F02,
    GOODOUT = 0x6002,
    KEY = 0x6103,
    COMMENT = 0x6206,
    ENDCOMMENT = 0x6300,
    TEXT_PATH = 0x6403,
    REFLIBS2 = 0x6506,
    FONTS2 = 0x6606,
    PATHWIDTH2 = 0x6703,
    SREFPATH = 0x6802,
    SREFPATHWIDTH = 0x6903,
    AREFPATH = 0x6A02,
    AREFPATHWIDTH = 0x6B03,
    TEXT_PATH2 = 0x6C03,
    TEXT_PATHWIDTH = 0x6D03
};

// 几何点
struct Point {
    int32_t x;
    int32_t y;
    
    Point() : x(0), y(0) {}
    Point(int32_t x_, int32_t y_) : x(x_), y(y_) {}
};

// 边界（多边形）
struct Boundary {
    std::vector<Point> points;
    int16_t layer;
    int16_t datatype;
    
    Boundary() : layer(0), datatype(0) {}
};

// 路径
struct Path {
    std::vector<Point> points;
    int16_t layer;
    int16_t datatype;
    int16_t pathtype;  // 0=flush, 1=round, 2=extended
    int32_t width;
    
    Path() : layer(0), datatype(0), pathtype(0), width(0) {}
};

// 文本
struct Text {
    std::string text;
    std::vector<Point> points;  // 第一个点是位置
    int16_t layer;
    int16_t texttype;
    int16_t presentation;  // 字体、对齐等
    int32_t width;  // 字符宽度（可选）
    
    Text() : layer(0), texttype(0), presentation(0), width(0) {}
};

// 几何元素变体
using GeometryElement = std::variant<Boundary, Path, Text>;

// 图层过滤器
struct LayerFilter {
    std::optional<std::vector<int16_t>> layers;      // 图层号过滤
    std::optional<std::vector<int16_t>> datatypes;   // 数据类型过滤
    
    bool isEmpty() const {
        return !layers.has_value() && !datatypes.has_value();
    }
    
    bool matches(int16_t layer, int16_t datatype) const {
        if (layers.has_value()) {
            const auto& l = layers.value();
            if (std::find(l.begin(), l.end(), layer) == l.end()) {
                return false;
            }
        }
        if (datatypes.has_value()) {
            const auto& d = datatypes.value();
            if (std::find(d.begin(), d.end(), datatype) == d.end()) {
                return false;
            }
        }
        return true;
    }
};

// 结构单元
struct Structure {
    std::string name;
    std::vector<GeometryElement> elements;
    
    Structure() = default;
    explicit Structure(const std::string& name_) : name(name_) {}
};

// 图层信息
struct LayerInfo {
    int16_t layer;
    int16_t datatype;
    size_t elementCount;
    
    LayerInfo() : layer(0), datatype(0), elementCount(0) {}
    LayerInfo(int16_t l, int16_t d) : layer(l), datatype(d), elementCount(1) {}
};

// GDSII 库
struct Library {
    std::string name;
    double units;           // 用户单位
    double precision;       // 数据库精度（米）
    std::vector<Structure> structures;
    std::vector<LayerInfo> layerInfos;
    
    Library() : units(1.0), precision(1e-9) {}
};

} // namespace gdsii

#endif // GDSII_TYPES_H
