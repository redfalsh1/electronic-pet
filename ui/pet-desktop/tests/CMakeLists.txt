# UI 测试配置
cmake_minimum_required(VERSION 3.16)

# 需要 Qt5 TestLib
find_package(Qt5 REQUIRED COMPONENTS Widgets Test)

# UI 组件测试
add_executable(ui_component_test
    ui_component_test.cpp
)

target_link_libraries(ui_component_test
    PRIVATE
        Qt5::Widgets
        Qt5::Test
)

# 包含目录
target_include_directories(ui_component_test
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# 启用 Qt 元对象编译器
set_target_properties(ui_component_test
    PROPERTIES
        AUTOMOC ON
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/tests
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/tests
)

# 添加测试
add_test(
    NAME UIComponentTest
    COMMAND ui_component_test
)
