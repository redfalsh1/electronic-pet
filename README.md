# EDA Desktop Application

高性能 EDA（电子设计自动化）桌面软件，用于集成电路/PCB 设计。

## 技术栈

- **语言**: C++17/20
- **框架**: Qt5.15+
- **构建**: CMake
- **平台**: Windows 10/11, Linux (Ubuntu 20.04+)

## 团队结构

| 角色 | 职责 |
|------|------|
| 技术负责人 | 需求分析、任务分配、代码审查、技术决策 |
| C++ 开发工程师 | 核心算法、数据结构、性能优化 |
| Qt 界面工程师 | UI 设计、交互实现、自定义控件 |
| 质量工程师 | 测试用例、自动化测试、Bug 验证 |
| DevOps 工程师 | Git 管理、CI/CD、构建发布 |

## 开发规范

### Git 分支策略
```
main          - 稳定版本，只有 release 时合并
dev           - 开发主分支，功能完成后合并
feature/*     - 功能分支，从 dev 分出，完成后合并回 dev
release/*     - 发布分支，用于测试和 bug 修复
hotfix/*      - 紧急修复分支，从 main 分出
```

### 提交信息规范
```
feat: 新功能
fix: Bug 修复
docs: 文档更新
style: 代码格式调整（不影响功能）
refactor: 重构（不是新功能或修复）
test: 测试相关
chore: 构建/工具/配置更新
```

## 快速开始

```bash
# 克隆仓库
git clone <repository-url>
cd eda-app

# 创建构建目录
mkdir build && cd build

# 配置 CMake
cmake ..

# 构建
cmake --build .
```

## License

待确认
