# DockingPanes 缺陷记录与修复说明

> 整理时间：2026-08-19
> 基线：上游 KestrelRadarSensors/dockingpanes（master）
> 用途：记录在 Vine 项目（Windows 11 / WSLg）集成、使用过程中发现并修复的缺陷，供后续维护与升级时对照。

## 一、库内缺陷（已修复）

### BUG-1 深色主题下窗格不变黑（硬编码浅色）
- 现象：Windows 11 深色模式下，停靠窗格标题栏、边框、文字、图标仍是浅色。
- 根因：
  - 库内大量硬编码浅色：标题栏 `#007acc` / `#eeeef2`（`DockingPaneContainer`、`DockingPaneFlyoutWidget` 的 `setActivePane` 用样式表写死）、边框 `#cccedb` / `#aaaaaa`、标题文字黑白固定、关闭/固定按钮图标为黑色位图。
  - 标题栏背景用 `setStyleSheet` 设置，无法跟随调色板变化。
- 修复：
  - 新增 `inc/DockingPaneTheme.h`：所有颜色从 `QApplication::palette()` 派生（`Highlight` / `Window` / `HighlightedText` / `WindowText` / `Mid`，边框由 `Window` 亮度派生）。
  - 标题栏背景改为在 `paintEvent` 中用调色板绘制，不再用样式表。
  - 各绘制控件重写 `changeEvent`，收到 `QEvent::PaletteChange` / `ApplicationPaletteChange` 时 `update()`，运行中切主题自动重绘。
  - 涉及：`DockingPaneContainer`、`DockingPaneFlyoutWidget`、`DockingPaneTabbedContainer`、`DockingPaneTitleWidget`、`DockAutoHideButton`、`DockingToolButton`。

### BUG-2 标题栏拖动路径没有抓取鼠标
- 现象：拖动浮动窗格到停靠指示器上松手，有时无效。
- 根因：
  - `DockingPaneTitleWidget::mousePressEvent` 未 `grabMouse()`；而库内其它拖动路径（Tab 拖出、Flyout 拖出、边缘缩放）都抓取了鼠标。
  - 停靠指示器（中心 sticker、4 个边框 sticker、蓝色预览 `DockingTargetWidget`）都是独立的 `Qt::ToolTip | WindowStaysOnTopHint` 置顶窗口；光标压到它们上面时，move/release 事件发给了指示器窗口而不是标题栏，`titleBarEndMove` → `floatingPaneEndMove` 永远不触发。
- 修复：按下时 `grabMouse()`、松开时 `releaseMouse()`，用 `m_grabbing` 记录抓取状态。

### BUG-3 停靠窗格拖出成浮动时抓取丢失
- 现象：从停靠状态拖出一点点后，拖动时断时续、指示器图标"经常不显示"（WSLg 延迟下尤为明显）。
- 根因：拖过 5px 阈值后 `floatPane()` 会 `closePane` + `setWindowFlags` + `setParent`，**重建原生窗口**，按下时抓取的 X 指针抓取随旧窗口销毁而丢失；之后移动事件只有光标恰好压在 20px 高的标题栏上才送达。
- 修复：新增 `DockingPaneTitleWidget::reacquireGrab()`，`DockingPaneContainer::onMoveDragTitle` 在 `floatPane()` 之后重新抓取新窗口。
- 遗留：**Flyout（自动隐藏窗格）拖出路径存在同样的抓取丢失**（`beginDrag()` 隐藏 flyout 会释放抓取），尚未按同样方式修复，见"遗留问题"。

### BUG-4 floatingPaneEndMove 忽略松手位置
- 现象：快速甩动到指示器上松手，停靠无效。
- 根因：函数开头 `Q_UNUSED(cursorPos)`，只信任最后一次 move 事件算出的 `m_targetPosition` / `m_targetPane`；快速拖动时 move 事件被 Qt 合并，最后一次被处理的 move 可能在光标进入指示器之前，目标还是 `-1`，静默无操作。
- 修复：松手时用 `cursorPos` 重新做一遍命中检测再停靠；结束后重置 `m_targetPosition` / `m_targetPane`。

### BUG-5 指示器显隐顺序错误 + 静态状态泄漏
- 现象：指示器组闪烁、时有时无；离开区域后残留。
- 根因：
  - `floatingPaneMoved` 对每个候选 pane 调用 `updateFloatingPane`，不命中的 pane 走 `else` **无条件**隐藏指示器和预览，把前面刚命中 pane 显示出来的又藏掉。
  - `updateFloatingPane` 内 `static QRect lastHitRect, lastStickerRect` 跨拖动泄漏状态。
- 修复：
  - `updateFloatingPane` 改为返回是否命中该 pane；只有光标不在任何 pane 上才隐藏指示器组；未命中指示器时只隐藏蓝色预览、保留指示器组作为引导。
  - 删除静态变量；显示后补 `raise()` 保证 z 序。

### BUG-6 图标着色失败（Format_Indexed8）
- 现象：对关闭/固定按钮图标重新着色时打印 `QPainter::begin: Cannot paint on an image with the QImage::Format_Indexed8 format`，深色主题下图标仍为黑色、不可见。
- 根因：PNG 资源加载为 `QImage::Format_Indexed8`，不能直接绘制。
- 修复：`DockingToolButton::paintEvent` 中着色前 `convertToFormat(QImage::Format_ARGB32)`。

## 二、主机侧配套修复（不在本库内，但与本库问题直接相关）

| 问题 | 位置 | 说明 |
|---|---|---|
| 应用强制浅色方案 | `src/fw/appfw/src/gui/GuiApplication.cpp` | 原来 `setColorScheme(Qt::ColorScheme::Light)` 强制浅色；删除后跟随系统；Qt<6.5 Windows 下读注册表 `AppsUseLightTheme` + Fusion 深色调色板 |
| `argc` 悬垂引用崩溃 | `src/fw/appfw/src/gui/GuiApplication.cpp` | `new QApplication(c, argv())` 传局部 `int c`，`QCoreApplication` 按引用保存；xcb 下 `QXcbWindow::create()` 计算 WM_CLASS 时读悬空引用 → `strlen` 段错误。改为传成员 `d->argc` |
| WSLg 下拖动无效 | `src/fw/appfw/src/gui/GuiApplication.cpp` | WSLg 下 Qt 默认走 Wayland 插件：窗口 `move()` 不生效、`QCursor::pos()` 返回 (0,0)，拖动机制完全失效；WSL + DISPLAY 存在且未显式指定平台时强制 `QT_QPA_PLATFORM=xcb` |
| `DockPanel::setFloating` 只改状态 | `src/fw/appfw/src/gui/DockPanel.cpp` | 原来只 `setState`，窗格仍留在停靠树：拖动走"已浮动"分支跳过 `closePane`，中央区不回收空间，旧占位区域上不显示指示器。改为真正 `floatPane(QPoint(0,0))` / `dockPane` 停靠回去。注意不能手动 `closePane + floatPane(QRect())`：`closePane` 的 `setParent(NULL)` 不映射屏幕坐标，窗格会落到错误位置 |

## 三、遗留问题（未修复）

1. **Flyout 拖出路径抓取丢失**：`DockingPaneContainer::onMoveDragFlyoutTitle` 中 `beginDrag()` 隐藏 flyout 后抓取被释放，后续移动依赖光标压在浮动窗格标题上；可用 `reacquireGrab()` 同样思路修复（需注意 release 事件路由到 pane 标题而非 flyout 标题的清理逻辑）。
2. **`floatingPaneEndMove` 中 `qApp->setActiveWindow(dockedPane)`**：停靠目标是子控件而非顶层窗口，调用为无效操作（无害）。
3. **Tabbed 子窗格区域重叠**：`updateFloatingPane` 按 `m_dockingPaneList` 顺序命中，"后命中覆盖先命中"；Tab 容器与其子 pane 重叠时停靠目标选择依赖列表顺序。
4. **框架层 `DockFeatures` 缺特性位**：枚举只有 `Closable`，注释中提到的 Floatable / Movable 未定义，特性约束未落地到拖动行为（`src/fw/appfw/sdk/vine/appfw/gui/Gui.hpp`）。
5. **框架层 `DockPanel` 状态方法"只改标志不执行操作"**（与 `setFloating` 同类封装缺陷）：
   - `pin()` / `unpin()`：只 `setState(Pinned/Docked)`，未走管理器的自动隐藏按钮逻辑（`hidePane` 等）
   - `collapse()` / `restore()`：只 `setState(Hidden/Docked)`，未真正隐藏/恢复显示
   - 后续使用这些 API 时会复现"状态与界面不一致"（如：状态是 Pinned 却没有自动隐藏按钮、状态是 Hidden 却仍显示）。修复方向：像 `setFloating` 一样接到 DockingPanes 的实际操作上。

## 四、本次改动涉及的库文件清单

- `inc/DockingPaneTheme.h`（新增）
- `src/DockingPaneContainer.cpp`
- `src/DockingPaneFlyoutWidget.cpp`
- `src/DockingPaneTabbedContainer.cpp`
- `src/DockingPaneTitleWidget.cpp`
- `src/DockAutoHideButton.cpp`
- `src/DockingToolButton.cpp`
- `src/DockingPaneManager.cpp`
- `inc/DockingPaneManager.h`、`inc/DockingPaneContainer.h`、`inc/DockingPaneFlyoutWidget.h`、`inc/DockingPaneTitleWidget.h`、`inc/DockAutoHideButton.h`、`inc/DockingToolButton.h`
