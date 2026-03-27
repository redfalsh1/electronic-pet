cmake_minimum_required(VERSION 3.15)
project(PetCore VERSION 1.0.0 LANGUAGES CXX)

# C++17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 编译选项
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(PET_CORE_BUILD_TESTS "Build tests" OFF)

# 源文件
set(PET_CORE_SOURCES
    src/pet.cpp
    src/pointsystem.cpp
    src/exchangesystem.cpp
    src/datamanager.cpp
    src/gdsii/gdsii_parser.cpp
)

# 头文件
set(PET_CORE_HEADERS
    include/pet.h
    include/pointsystem.h
    include/exchangesystem.h
    include/datamanager.h
    include/gdsii/gdsii_parser.h
    include/gdsii/gdsii_types.h
)

# 创建库
add_library(pet_core STATIC ${PET_CORE_SOURCES} ${PET_CORE_HEADERS})

# 包含目录
target_include_directories(pet_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# 编译定义
target_compile_definitions(pet_core
    PRIVATE
        $<$<CONFIG:Debug>:PET_CORE_DEBUG>
)

# 编译选项（平台特定）
if(MSVC)
    target_compile_options(pet_core PRIVATE /W4 /WX-)
    target_compile_options(pet_core PRIVATE /utf-8)
else()
    target_compile_options(pet_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

# 线程支持
find_package(Threads REQUIRED)
target_link_libraries(pet_core PUBLIC Threads::Threads)

# 文件系统支持（C++17 已包含，但某些编译器需要额外链接）
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    target_link_libraries(pet_core PUBLIC stdc++fs)
endif()

# 安装规则
include(GNUInstallDirs)

install(TARGETS pet_core
    EXPORT pet_core_targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES ${PET_CORE_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/pet_core
)

install(EXPORT pet_core_targets
    FILE pet_core_targets.cmake
    NAMESPACE pet_core::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pet_core
)

# 测试（可选）
if(PET_CORE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# 默认启用测试构建
option(PET_CORE_BUILD_TESTS "Build tests" ON)

# 导出包配置
include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/pet_core-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/pet_core-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pet_core
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/pet_core-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/pet_core-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/pet_core-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pet_core
)
