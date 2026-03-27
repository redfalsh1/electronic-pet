# 测试配置 - Google Test 集成
cmake_minimum_required(VERSION 3.15)

# 下载并配置 Google Test
include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)

# 对于 Windows: 防止覆盖父项目的编译器/链接器设置
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# 启用测试
enable_testing()

# 添加测试子目录
add_subdirectory(unit)
add_subdirectory(integration)
