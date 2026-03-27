# CI/CD 配置文档

## 概述

本项目使用 GitHub Actions 实现自动化 CI/CD 流水线，支持 Windows 和 Linux 双平台构建。

## 工作流文件

- **位置**: `.github/workflows/ci-cd.yml`
- **触发条件**: Push、Pull Request、Release

## 流水线阶段

### 1. 代码检查 (Code Check)
**触发**: 仅 PR
- C++ 代码风格检查 (clang-format)
- CMake 格式检查

### 2. Windows 构建
**触发**: Push / PR / Release
- 安装 Qt5.15.2
- CMake 配置
- 编译构建
- 运行单元测试
- 上传构建产物

### 3. Linux 构建
**触发**: Push / PR / Release
- 安装依赖 (Qt5, CMake, g++)
- CMake 配置
- 编译构建
- 运行单元测试
- 上传构建产物

### 4. 发布 (Release)
**触发**: 仅 Release
- 下载 Windows/Linux 构建产物
- 打包 (ZIP/TAR.GZ)
- 上传到 GitHub Release

### 5. 构建汇总
**触发**: 所有构建完成后
- 汇总各平台构建状态
- 生成构建报告

## 触发规则

| 事件 | 分支/标签 | 触发动作 |
|------|----------|----------|
| Push | main, dev | 构建 + 测试 |
| Pull Request | main, dev | 构建 + 测试 + 代码检查 |
| Release | v* 标签 | 打包 + 发布 |

## 构建产物

### 保留策略
- 常规构建产物：30 天
- 构建日志（失败时）：7 天

### 下载位置
- GitHub Actions 页面 → 选择对应 workflow run → Artifacts

## 本地构建

### Windows
```powershell
# 配置
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DPET_CORE_BUILD_TESTS=ON

# 构建
cmake --build build --config Release --parallel

# 测试
ctest --test-dir build -C Release --output-on-failure

# 安装
cmake --install build --config Release
```

### Linux
```bash
# 安装依赖
sudo apt-get install -y qt5-default qtbase5-dev cmake g++

# 配置
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DPET_CORE_BUILD_TESTS=ON

# 构建
cmake --build build --parallel $(nproc)

# 测试
ctest --test-dir build --output-on-failure

# 安装
sudo cmake --install build
```

## 环境变量

| 变量 | 值 | 说明 |
|------|-----|------|
| BUILD_TYPE | Release | 构建类型 |
| QT_VERSION | 5.15.2 | Qt 版本 |

## 故障排查

### 构建失败
1. 查看 GitHub Actions 日志
2. 下载 `build-logs-*`  artifacts
3. 本地复现构建

### 测试失败
1. 查看 `ctest` 输出
2. 检查测试用例
3. 本地运行测试

### 依赖问题
- Windows: 确保 Qt 已正确安装
- Linux: 运行 `sudo apt-get install -y qt5-default qtbase5-dev`

## 提交规范

DevOps 相关提交请使用前缀：`[DEVOPS]`

示例:
```
[DEVOPS] chore: 配置 GitHub Actions CI/CD 流水线
[DEVOPS] fix: 修复 Windows 构建路径问题
[DEVOPS] feat: 添加 Linux AppImage 打包
```

## 维护说明

- 定期更新 GitHub Actions 版本
- 监控 Qt 版本兼容性
- 根据项目发展调整构建配置
