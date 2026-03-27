# AGENTS.md - EDA Tech Lead 工作区

## 项目信息

- **项目名称**: EDA Desktop Application
- **技术栈**: C++17/20, Qt5.15+, CMake
- **目标平台**: Windows 10/11, Linux (Ubuntu 20.04+)
- **代码仓库**: GitHub (待配置)

## 团队结构

| Agent ID | 角色 | 职责 |
|----------|------|------|
| `eda_tech_lead` | 技术负责人 | 需求分析、任务分配、代码审查、技术决策 |
| `eda_cpp_dev` | C++ 开发工程师 | 核心算法、数据结构、性能优化 |
| `eda_qt_dev` | Qt 界面工程师 | UI 设计、交互实现、自定义控件 |
| `eda_qa` | 质量工程师 | 测试用例、自动化测试、Bug 验证 |
| `eda_devops` | DevOps 工程师 | Git 管理、CI/CD、构建发布 |

## 开发规范

### 代码规范
- 遵循 C++ Core Guidelines
- 使用智能指针，避免裸指针
- 遵循 RAII 原则
- 关键算法必须添加单元测试

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

## 当前迭代

### Sprint 待办
- [ ] 配置 GitHub 仓库
- [ ] 设置 CI/CD 流水线
- [ ] 创建项目骨架

### 技术债务
- _暂无_

## 重要决策记录

### 2026-03-26 - 团队初始化
- 创建 5 人智能体团队
- 采用 GitHub + GitHub Actions 作为代码托管和 CI/CD
- 使用 CMake 作为构建系统

---

## 工作区说明

- **本工作区**: `~/.openclaw/workspace-eda_tech_lead` - TechLead 的专属工作区
- **共享源码**: `/data/eda-project/src` - 实际项目源码（只读挂载给开发 Agent）
- **构建目录**: `/data/eda-project/build` - 构建输出（可写）

## 常用命令

### 任务分配
```bash
# 分配 C++ 开发任务
/sessions_spawn eda_cpp_dev "实现 GDSII 解析器的图层过滤功能"

# 分配 UI 开发任务
/sessions_spawn eda_qt_dev "添加图层选择下拉框到导入对话框"

# 请求测试
/sessions_send eda_qa "请对 PR #55 运行完整测试套件"
```

### GitHub 操作
```bash
# 查看待审查的 PR
gh pr list --repo owner/eda-app --state open

# 创建 Release
gh release create v1.0.0 --generate-notes
```
