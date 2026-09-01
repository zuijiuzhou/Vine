# .ai/ —— AI 辅助目录

存放面向 AI 助手（Copilot 等）的仓库内知识，随 git 版本化、团队可见。
`docs/` 保留给 GitHub Pages 项目站点，不在其中放置设计文档/笔记。

- `memory/` —— 精炼模块要点（对应 VS Code 记忆系统仓库级笔记的可提交副本）。
  - `robotics-proximity.md` —— Robotics proximity 模块要点。
- `design/` —— 完整设计文档。
  - `robotics-io-design.md` —— Robotics IO 设计（XML 序列化、VFS 打包、5 版本 API、材质库、无状态重构）。
  - `robotics-proximity-design.md` —— Robotics proximity 设计（接口清单、设计决策、VMR 对照、FCL 接入点、测试）。

约定：`memory/` 保持简短要点式；详细设计放 `design/`；两者内容互补。
