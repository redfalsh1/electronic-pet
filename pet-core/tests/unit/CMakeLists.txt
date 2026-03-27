# 单元测试配置
cmake_minimum_required(VERSION 3.15)

# GDSII 解析器测试
add_executable(gdsii_parser_test
    gdsii_parser_test.cpp
)

target_link_libraries(gdsii_parser_test
    PRIVATE
        pet_core
        GTest::gtest
        GTest::gtest_main
)

add_test(
    NAME GDSIIParserTest
    COMMAND gdsii_parser_test
)

# 核心模块测试
add_executable(core_module_test
    core_module_test.cpp
)

target_link_libraries(core_module_test
    PRIVATE
        pet_core
        GTest::gtest
        GTest::gtest_main
)

add_test(
    NAME CoreModuleTest
    COMMAND core_module_test
)

# 设置测试输出
set_target_properties(gdsii_parser_test core_module_test
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/tests
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/tests
)
