# EDA Desktop Application - Root CMakeLists.txt
# 统一构建配置

cmake_minimum_required(VERSION 3.16)

project(EDA_Desktop_Application
    VERSION 1.0.0
    DESCRIPTION "High-performance EDA desktop application for IC/PCB design"
    LANGUAGES CXX
)

# C++17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 构建类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

# 编译选项
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(EDA_BUILD_TESTS "Build tests" ON)
option(EDA_BUILD_DOCS "Build documentation" OFF)

# 输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 子项目
add_subdirectory(pet-core)
add_subdirectory(PetEdu)
add_subdirectory(PetEduComplete)

# 测试启用
if(EDA_BUILD_TESTS)
    enable_testing()
    message(STATUS "Tests enabled")
    
    # 添加测试子目录（如果存在）
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tests)
        add_subdirectory(tests)
    endif()
endif()

# 文档构建（可选）
if(EDA_BUILD_DOCS)
    find_package(Doxygen QUIET)
    if(DOXYGEN_FOUND)
        message(STATUS "Doxygen found - documentation can be built")
        # 配置 Doxygen
        set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/docs)
        doxygen_add_docs(docs ALL ${CMAKE_CURRENT_SOURCE_DIR})
    else()
        message(WARNING "Doxygen not found - documentation will not be built")
    endif()
endif()

# 安装配置
include(GNUInstallDirs)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/pet-core/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/pet-core
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)

# 打包配置
set(CPACK_PACKAGE_NAME "EDA-Desktop")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VENDOR "EDA Team")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PROJECT_DESCRIPTION})

if(WIN32)
    set(CPACK_GENERATOR "ZIP;NSIS")
elseif(UNIX AND NOT APPLE)
    set(CPACK_GENERATOR "TGZ;DEB")
endif()

include(CPack)

# 打印配置摘要
message(STATUS "")
message(STATUS "=== Configuration Summary ===")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Install Prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Build Tests: ${EDA_BUILD_TESTS}")
message(STATUS "Build Docs: ${EDA_BUILD_DOCS}")
message(STATUS "")
