# 集成测试配置
cmake_minimum_required(VERSION 3.15)

# 模块间接口测试
add_executable(integration_test
    module_interface_test.cpp
)

target_link_libraries(integration_test
    PRIVATE
        pet_core
        GTest::gtest
        GTest::gtest_main
)

add_test(
    NAME IntegrationTest
    COMMAND integration_test
)

# 设置测试输出
set_target_properties(integration_test
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/tests
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/tests
)
