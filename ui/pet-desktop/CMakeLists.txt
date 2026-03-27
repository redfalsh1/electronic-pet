cmake_minimum_required(VERSION 3.16)
project(PetDesktop VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 查找 Qt5 组件
find_package(Qt5 15.0 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
)

# 源文件
set(SOURCES
    mainwindow.cpp
    teacherpanel.cpp
    studentview.cpp
    exchangedialog.cpp
)

# 头文件
set(HEADERS
    mainwindow.h
    teacherpanel.h
    studentview.h
    exchangedialog.h
)

# Qt 资源文件
set(RESOURCES
    resources.qrc
)

# 创建可执行文件
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

# 链接 Qt 库
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
)

# 包含当前目录
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Windows 特定设置
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

# 复制样式表到构建目录
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/styles/dark_theme.qss
     DESTINATION ${CMAKE_BINARY_DIR}/styles/)
