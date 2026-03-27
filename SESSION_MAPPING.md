# 成员会话 ID 映射

> 用于 TechLead 在群聊中使用 sessions_send 下发任务

---

## 📋 成员会话映射

| 成员 | Agent ID | 飞书 Open ID | 会话 ID |
|------|----------|-------------|---------|
| **eda_qt_dev** | `eda_qt_dev` | `ou_5732e4e18a54851b265c635df05c40f7` | `agent:eda_qt_dev:feishu:direct:ou_5732e4e18a54851b265c635df05c40f7` |
| **eda_cpp_dev** | `eda_cpp_dev` | `ou_e04084e05f2f602716abbb82750cdcd7` | `agent:eda_cpp_dev:feishu:direct:ou_e04084e05f2f602716abbb82750cdcd7` |
| **eda_qa** | `eda_qa` | `ou_d0c36c62d4d6afa911c37d3f30066424` | `agent:eda_qa:feishu:direct:ou_d0c36c62d4d6afa911c37d3f30066424` |
| **eda_devops** | `eda_devops` | `ou_aff27cfe7d91aaa86cf5a0ee6179825d` | `agent:eda_devops:feishu:direct:ou_aff27cfe7d91aaa86cf5a0ee6179825d` |

---

## 🔧 使用说明

### sessions_send 格式

```bash
sessions_send "<会话 ID>" "任务内容"
```

### 示例

```bash
# 通知 eda_qt_dev
sessions_send "agent:eda_qt_dev:feishu:direct:ou_719c3e8c6626bae4c1c2f75479edd7a0" "
【任务分配】GDSII 导入对话框 UI

需求：
1. 文件选择器
2. 图层列表（树形）
3. 多选功能

截止时间：后天 12:00

请确认收到并开始执行。"
```

---

## 📝 获取 Open ID 方法

### 方法 1：查看配对请求

```powershell
openclaw pairing list feishu
```

### 方法 2：查看网关日志

```powershell
openclaw logs --follow
# 让成员给机器人发消息，日志中会显示 open_id
```

输出示例：
```json
{"level":"info","channel":"feishu","sender_id":"ou_719c3e8c6626bae4c1c2f75479edd7a0",...}
```

### 方法 3：飞书开放平台

1. 登录 https://open.feishu.cn
2. 进入应用管理
3. 查看用户列表
4. 找到对应成员的 open_id

---

## ⚠️ 注意事项

1. **Open ID 格式**：`ou_xxx`（飞书用户唯一标识）
2. **会话 ID 格式**：`agent:<agent_id>:feishu:direct:<open_id>`
3. **更新时机**：成员变化时及时更新此文件
4. **保密**：Open ID 属于敏感信息，不要公开分享

---

## 🔄 更新记录

| 日期 | 成员 | Open ID | 备注 |
|------|------|---------|------|
| 2026-03-26 | eda_qt_dev | ou_719c3e8c6626bae4c1c2f75479edd7a0 | 初始配置 |
| 待更新 | eda_cpp_dev | ou_xxx | 待获取 |
| 待更新 | eda_qa | ou_yyy | 待获取 |
| 待更新 | eda_devops | ou_zzz | 待获取 |

---

**最后更新**：2026-03-26
