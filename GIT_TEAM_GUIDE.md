# EDA 团队 Git 协作指南（共享账号模式）

## 📋 账号信息

**共享 GitHub 账号**
- 用户名：`redfalsh1`
- 仓库：https://github.com/redfalsh1/electronic-pet
- 认证方式：HTTPS + Token / SSH

---

## 🔧 本地配置（每位成员首次使用时设置）

### 方案 1：配置本地 Git 身份

```bash
# 进入项目目录
cd electronic-pet

# C++ 开发成员配置
git config user.name "EDA-CPP-Dev"
git config user.email "cpp-dev@eda-team.local"

# Qt 开发成员配置
git config user.name "EDA-Qt-Dev"
git config user.email "qt-dev@eda-team.local"

# QA 成员配置
git config user.name "EDA-QA"
git config user.email "qa@eda-team.local"

# DevOps 成员配置
git config user.name "EDA-DevOps"
git config user.email "devops@eda-team.local"

# 技术负责人配置
git config user.name "EDA-TechLead"
git config user.email "tech-lead@eda-team.local"
```

### 方案 2：提交时临时指定

```bash
git commit --author="EDA-CPP-Dev <cpp-dev@eda-team.local>" -m "feat: 功能描述"
```

---

## 📝 提交信息规范

### 格式
```
[角色] type: 描述内容
```

### 角色前缀
| 前缀 | 角色 | 示例 |
|------|------|------|
| `[CPP]` | C++ 开发 | `[CPP] feat: 实现 GDSII 解析器` |
| `[QT]` | Qt 界面 | `[QT] feat: 设计导入对话框` |
| `[QA]` | 质量测试 | `[QA] test: 添加单元测试` |
| `[DEVOPS]` | DevOps | `[DEVOPS] chore: 配置 CI/CD` |
| `[LEAD]` | 技术负责人 | `[LEAD] docs: 更新架构文档` |

### 提交类型
- `feat`: 新功能
- `fix`: Bug 修复
- `docs`: 文档更新
- `style`: 代码格式调整
- `refactor`: 重构
- `test`: 测试相关
- `chore`: 构建/工具/配置更新

---

## 🏷️ 使用标签标记重要提交

### 为提交打标签（标识成员工作）

```bash
# 查看提交历史
git log --oneline

# 为特定提交打标签
git tag -a cpp-dev-20260327 -m "C++ 开发：GDSII 解析器完成" <commit-hash>

# 推送标签到远程
git push origin --tags

# 查看标签
git tag -l "cpp-dev-*"
git tag -l "qt-dev-*"
git tag -l "qa-*"
```

### 标签命名规范
```
<角色>-<日期>-<序号>
示例：
- cpp-dev-20260327-01
- qt-dev-20260328-01
- qa-20260329-01
```

---

## 🔄 标准开发流程

### 1. 拉取最新代码
```bash
git checkout dev
git pull origin dev
```

### 2. 创建功能分支
```bash
# 分支命名：feature/<角色>/<功能描述>
git checkout -b feature/cpp/gdsii-parser
git checkout -b feature/qt/import-dialog
git checkout -b feature/qa/unit-tests
```

### 3. 开发和提交
```bash
# 确认本地身份配置
git config user.name
git config user.email

# 提交代码（带角色前缀）
git add .
git commit -m "[CPP] feat: 实现 GDSII 解析器核心逻辑"

# 或临时指定作者
git commit --author="EDA-CPP-Dev <cpp-dev@eda-team.local>" -m "feat: 实现 GDSII 解析器核心逻辑"
```

### 4. 推送分支
```bash
git push -u origin feature/cpp/gdsii-parser
```

### 5. 创建 Pull Request
- 在 GitHub 上创建 PR
- 在 PR 描述中标注负责人
- 等待代码审查

### 6. 合并后清理
```bash
# 删除已合并的分支
git branch -d feature/cpp/gdsii-parser
git push origin --delete feature/cpp/gdsii-parser
```

---

## 📊 查看成员贡献

### 按作者查看提交
```bash
# 查看特定成员的提交
git log --author="EDA-CPP-Dev" --oneline
git log --author="EDA-Qt-Dev" --oneline

# 查看提交统计
git shortlog -sn --all
```

### 按标签查看
```bash
# 查看特定角色的标签
git tag -l "cpp-dev-*"
git tag -l "qt-dev-*"

# 查看标签详情
git show cpp-dev-20260327-01
```

### GitHub 贡献图
- 所有提交会显示在共享账号下
- 通过提交信息前缀区分成员
- 通过标签标记里程碑

---

## ⚠️ 注意事项

1. **拉取代码前先提交** - 避免覆盖他人的本地配置
2. **分支命名规范** - 清晰标识负责人和功能
3. **提交信息完整** - 必须包含角色前缀
4. **定期推送标签** - 确保标签同步到远程
5. **代码审查** - 重要功能需要他人审查

---

## 🆘 常见问题

### Q: 如何修改已提交的作者信息？
```bash
# 修改最后一次提交
git commit --amend --author="EDA-CPP-Dev <cpp-dev@eda-team.local>"

# 修改多个历史提交
git rebase -i HEAD~3
# 在编辑器中修改 author 行
```

### Q: 如何查看某个文件的提交历史？
```bash
git log --oneline path/to/file
git log --author="EDA-CPP-Dev" --oneline path/to/file
```

### Q: 如何撤销已推送的提交？
```bash
# 撤销最后一次推送（保留本地）
git reset --soft HEAD~1
git push --force-with-lease

# 或创建修复提交
git revert <commit-hash>
git push origin dev
```

---

## 📞 技术支持

遇到问题请联系技术负责人或查看：
- GitHub 文档：https://docs.github.com
- Git 手册：https://git-scm.com/doc

---

_最后更新：2026-03-27_
_团队：EDA Desktop Application Development Team_
