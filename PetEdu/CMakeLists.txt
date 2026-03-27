cmake_minimum_required(VERSION 3.16)
project(PetEdu VERSION 1.0.0 LANGUAGES CXX)

# C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Qt5 配置
find_package(Qt5 REQUIRED COMPONENTS Widgets Sql)

# 源文件列表
set(SOURCES
    src/main.cpp
    # core 模块 (cpp_dev 负责)
    # src/core/pet.cpp
    # src/core/pet_manager.cpp
    # src/core/database.cpp
    # ui 模块 (qt_dev 负责)
    # src/ui/mainwindow.cpp
    # src/ui/pet_widget.cpp
)

# 头文件列表
set(HEADERS
    # src/core/pet.h
    # src/core/pet_manager.h
    # src/core/database.h
    # src/ui/mainwindow.h
    # src/ui/pet_widget.h
)

# 资源文件
set(RESOURCES
    resources/resources.qrc
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${RESOURCES})

# 链接 Qt5 库
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Widgets
    Qt5::Sql
)

# 包含目录
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
)

# 编译选项
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX-)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()

# 安装配置
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# 打印配置信息
message(STATUS "PetEdu 配置完成:")
message(STATUS "  - C++ 标准：C++17")
message(STATUS "  - Qt 版本：${Qt5Core_VERSION}")
message(STATUS "  - 构建类型：${CMAKE_BUILD_TYPE}")
