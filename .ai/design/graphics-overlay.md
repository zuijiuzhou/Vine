# Graphics Overlay（HUD 叠加层）设计

状态：Phase 1（核心框架）已落地并有单测；Phase 2/3（后端 vsg 子视口渲染 + 具体坐标轴内容）待做。

## 目标

在 graphics 核心提供**通用、后端无关**的"叠加在画面之上"的能力，覆盖不止坐标轴的一种场景：
左下角坐标轴 / 准星 / 小地图 / 屏幕标注 / 拾取高亮 / 水印 等。
约束：多后端兼容（当前 vsg，未来手写 Vulkan/OpenGL）——**叠加内容仍是普通
Scene/Node/Drawable/Material**，后端不需要为 overlay 写专属代码。

## 核心抽象

一个 Overlay 由 4 个正交属性描述：**内容(Scene) + 相机(镜像/独立) + 区域(子视口) + 合成(清除/深度/顺序)**。
它与现有"RenderPass 渲染一个 Scene"同构，因此落地是"多一层有序 pass"，而非新渲染范式。

- `RenderPass` 新增：可选子视口 `setViewport(x,y,w,h)`、清除开关 `setClearEnabled(bool)`。
  - 主 pass 默认清屏；overlay pass 默认不清（叠画在上层）。
- `RenderBackend` 新增：`setViewport(x,y,w,h)`（默认 no-op = 全屏）。由 `RenderPass::execute` 在画前调用。
- `Overlay`（`src/viz/graphics/sdk/vine/graphics/Overlay.hpp`）：
  - 数据：`content()`(Scene) + `pass()`(RenderPass)；引擎持有其引用。
  - 展示：`visible()` / `zOrder()`（升序绘制）。
  - 相机跟随：`setSourceCamera()` + `setMirrorMode()`：
    - `None`：独立相机（小地图 / 2D HUD）。
    - `Orientation`：只跟随源相机朝向，保持本层取景距离（坐标轴）。
    - `FullView`：完全采用源相机视图（画中画 / VR 眼，预留）。
  - 每帧：`update(double dt)`（默认应用 mirror），引擎在画前调用；surface 变化回调 `onSurfaceResized()`。
- `RenderEngine`：`addOverlay/removeOverlay/clearOverlays`；`frame(dt)` = 主 pass →
  按 zOrder 依次 `update` + `pass()->execute(content, backend)` → end/swap。

```cpp
engine->addOverlay(axis_gizmo);   // 坐标轴
engine->addOverlay(crosshair);    // 以后加准星/小地图/水印… 都是再 addOverlay 一个
```

## 已实现文件

- `sdk/vine/graphics/Overlay.hpp` / `src/Overlay.cpp`
- `sdk/vine/graphics/AxisGizmo.hpp` / `src/AxisGizmo.cpp`（坐标轴内容：三根红X/绿Y/蓝Z 方柱，
  复用 IndexedTriangleMesh 与 SceneBridge 管线；`MirrorMode::Orientation` + 左下角子视口）
- `RenderPass.hpp/.cpp`：viewport + clearEnabled
- `RenderBackend.hpp`：`setViewport` 默认 no-op
- `RenderEngine.hpp/.cpp`：overlay 列表与帧尾绘制
- 测试 `tests/test_graphics/GraphicsTest.cpp` OverlayTest(3) + AxisGizmoTest(3)：
  - 可见 overlay 按 zOrder 绘制、隐藏跳过
  - 子视口转发到 backend、overlay 不额外清屏
  - `Orientation` 镜像：朝向跟随源、取景距离不变
  - AxisGizmo：3 根彩色柱、左下角视口定位、朝向镜像

## 为什么 Camera 不引入 master/slave

`Camera` 保持叶子矩阵对象（eye/target/up + 投影），不加父子关系。
"谁跟随谁"由 Overlay 层 `MirrorMode` + `setSourceCamera()` 运行时装配表达，
避免把视图关系耦合进 Camera（aspect/manipulator/resize 均不受影响）。
真正的"同视图多投影"（VR 双眼 / 环绕多屏）留作高层的 `View` 概念，届时再引入，
`MirrorMode::FullView/Offset` 已为其预留扩展点。

## 后端（vsg）落地（Phase 2/3，已实现并设备验证通过 2026-09-03）

`VsgRenderer`（gfx_backend_vsg）overlay 落地方式（多轮调试后的最终形态）：
- `render()` 只做"同步 + 编译"，**提交推迟到 `swapBuffers()`**（`submitFrame()`：record+submit+present 一次）→ 主 pass + 各 overlay pass 一帧只 present 一次。
- `setViewport(x,y,w,h)` override：记录 pending 子视口，由下一次 `render()` 消费；无子视口 = 全屏。
- **关键结论 1**：不要为 overlay 单独建 `RenderGraph`（同窗口第二个 render pass）——其 CLEAR 会显示
  （灰底方块出现），但 View 内容**从不光栅化**（全窗口诊断 + 预编译 warm-up 均证伪了编译时机）。
  正确做法 = **主 RenderGraph 里的第二个 `vsg::View`**（官方多视口范式，同一 render pass）：
  overlay 相机 `viewportState` 每帧设为子矩形 → 内容被裁剪/映射到该子区并画在主场景之上。
- **关键结论 2**：overlay 内容用**独立的 `SceneBridge`（overlayBridge）** + 关闭深度测试/写入的
  shaderSet（`buildShaderSet(extent, depth_test=false)`）→ 轴永远在最上层，不被场景几何遮挡。
- **关键结论 3**：overlay 视图只放一个 `AmbientLight`（intensity 1），**不要**用 `createHeadlight()`
  的定向光——定向光方向固定，镜像相机转到对角线（如 (1,1,1)）时朝相机面法线·光为负 → 轴发黑。
  环境光下 phong `ambientColor = diffuse*ambient*ambient.a`，与面朝向无关 → 恒纯色。
- **关键结论 4**：`AxisGizmo` 必须**持有**自己的相机（成员 `camera_`），不能传局部 `intrusive_ptr`
  裸指针给 pass（构造后即析构 → 悬空 → mirror 每帧写已释放内存 → eye=NaN/inf 相机）。
- **关键结论 5**：`RenderEngine::initialize()` 在 `backend->initialize()` 成功后**预执行一次各 overlay
  pass**（warm-up），让 overlay 内容在首帧前于"与主内容一致的可靠上下文"编译（早期发现帧内运行时
  编译几何"proven unreliable"；保留 warm-up 作为保险）。
- 观感：轴默认 framing 距离 3.3（长度 1 的柱约占子框半宽 73%）、半厚 0.09（约 3px）、
  材质 diffuse=颜色 + ambient=白 + specular=黑（平板纯色、无白色高光）。
- 代价：单 pass 多视图无法给 overlay 做独立清屏 → **无灰色不透明底块**（轴直接叠在 3D 场景上），
  用户已接受（不再需要底块）。场景残影/深度穿帮由深度关闭解决。
- App 接线：`app_shell/AppShellUi::addAxisGizmo()` 创建 `AxisGizmo`、
  `setSourceCamera(engine->masterCamera())` 并 `engine->addOverlay(...)`（demo dock 构建时调用）。
- 状态：编译/单测全绿（test_graphics），运行期用户已确认：轴在左下角显示、随视角旋转、
  任意角度保持纯色不黑、不被立方体遮挡。

## 坐标轴内容（AxisGizmo）
轴 = 三根细柱（红X/绿Y/蓝Z），用现有 `IndexedTriangleMesh` 构建、绕局部原点，
配 `MirrorMode::Orientation` + 左下角子视口 + 小透视相机。避免 `glLineWidth`/文字（后端分叉点）。

## 环境备注

- `v_add_library` 用 `file(GLOB_RECURSE)`，新增 .cpp 后需 `cmake.configure` 重新配置才会纳入。
- 若 configure 因残留 `vsg_FOUND:INTERNAL=TRUE` 走错 "installed vsg" 分支：
  删除该行后 `cmake.configure`（强制）即回到 FetchContent vsg（`_deps/vsg-src`）。
