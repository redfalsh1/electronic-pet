cmake_minimum_required(VERSION 3.16)

# 项目定义
project(EDADesktop VERSION 1.0.0 LANGUAGES CXX)

# C++17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 构建选项
option(BUILD_TESTS "Build tests" ON)
option(BUILD_UI "Build UI components" ON)

# 输出目录配置
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 添加子项目
add_subdirectory(pet-core)

# UI 组件（可选）
if(BUILD_UI)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)
    
    find_package(Qt5 REQUIRED COMPONENTS Widgets Sql)
    
    if(BUILD_TESTS)
        add_subdirectory(ui/pet-desktop/tests)
    endif()
endif()

# 测试配置
if(BUILD_TESTS)
    enable_testing()
    
    # 自定义测试目标
    add_custom_target(run_tests
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        DEPENDS gdsii_parser_test core_module_test integration_test
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running all tests..."
    )
endif()

# 安装配置
include(GNUInstallDirs)

install(DIRECTORY ${CMAKE_BINARY_DIR}/bin/
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    FILES_MATCHING PATTERN "*.exe" PATTERN "*.dll"
)
