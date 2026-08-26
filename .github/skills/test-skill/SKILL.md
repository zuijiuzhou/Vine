---
name: test-skill
description: |
  **DEMO SKILL** — 查询并列举当前环境中所有可用的 skill。
  Use when: 用户问"有哪些 skill"、"列出所有 skill"、"skill 列表"、"show skills"、"list skills"；
  需要查看当前工作区安装的自定义 skill。
---

# Test Skill: 技能列表查询

## 触发条件

当用户提到：有哪些 skill、列出 skill、skill 列表、show skills、list skills、可用技能 等。

## 工作流程

收到请求后，执行以下步骤：

### Step 1: 扫描 skill 目录

使用 `file_search` 工具搜索以下路径下的 `SKILL.md` 文件：

**工作区级别**（项目自定义）：
1. `.github/skills/**/SKILL.md`
2. `.agents/skills/**/SKILL.md`
3. `.claude/skills/**/SKILL.md`

**系统级别**（VS Code 内置）：
4. `~/.vscode-server/bin/*/extensions/copilot/assets/prompts/skills/**/SKILL.md`
5. `~/.vscode/extensions/*/assets/prompts/skills/**/SKILL.md`

> 注意：系统路径中的 `*` 是版本 hash，每次 VS Code 更新会变化，因此必须用 glob 通配。

**补充**：同时检查系统提示 `<skills>` 块中已列出的 skill，与文件扫描结果合并去重。

### Step 2: 读取每个 skill 的 frontmatter

对找到的每个 `SKILL.md`，使用 `read_file` 读取前 10 行，提取 YAML frontmatter 中的：
- `name` — skill 标识名
- `description` — 触发条件和使用说明

### Step 3: 输出 skill 清单

使用以下 Markdown 表格格式输出：

```markdown
## 📋 当前环境 Skill 清单

| 名称 | 来源 | 描述 |
|------|------|------|
| `<name>` | 工作区 / 系统内置 | `<description 摘要>` |

**总计**: N 个 skill
```

按来源分组显示（内置 vs 工作区自定义），让用户一目了然。

## 演示完毕

此 skill 展示了：
1. ✅ 文件系统搜索（`file_search` + `read_file`）
2. ✅ YAML frontmatter 解析
3. ✅ 结构化表格输出
4. ✅ 工作区 skill 与内置 skill 的区分

