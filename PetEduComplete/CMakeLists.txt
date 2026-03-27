cmake_minimum_required(VERSION 3.16)
project(PetEdu VERSION 1.0.0 LANGUAGES CXX)

# C++17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt5 配置
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets Sql)

# 源文件
set(SOURCES
    src/main.cpp
    src/core/pet.cpp
    src/core/pet.h
    src/core/pointsystem.cpp
    src/core/pointsystem.h
    src/core/exchangesystem.cpp
    src/core/exchangesystem.h
    src/core/datamanager.cpp
    src/core/datamanager.h
    src/ui/mainwindow.cpp
    src/ui/mainwindow.h
    src/ui/teacherpanel.cpp
    src/ui/teacherpanel.h
    src/ui/studentview.cpp
    src/ui/studentview.h
    src/ui/exchangedialog.cpp
    src/ui/exchangedialog.h
)

# 资源文件
set(RESOURCES
    resources.qrc
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES} ${RESOURCES})

# 链接库
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

# Windows 特定配置
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# 安装规则
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
