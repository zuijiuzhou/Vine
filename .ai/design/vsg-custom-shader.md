# vsg 后端自定义着色器设计（自写 shading ABI）

> 状态：设计稿 v1（2026-09-03）
> 2026-09-03 已落地：语义着色预置 `ShaderPreset` + 到 vsg 内建 set 的过渡映射，并完成像素验证（见 §8）。
> 关联：`graphics-lighting.md`、`graphics-shadow.md`、`vsg-design.md`、`render-pipeline-builder.md`
>
> **一句话**：材质 / 光照 / 阴影 / 着色语义全部归 `graphics`（我们），vsg 只保留
> "窗口 / 交换链 / 资源 / 命令 / 管线构建 / 录制" 工程层。Shader/UBO/描述符契约是
> **后端无关的 ABI**，未来平移到 Diligent / 手写 Vulkan 后端时原样复用。

## 1. 动机与目标

- vendored vsg 的 phong 是**序列化 blob**（`phong_ShaderSet.cpp` 仅二进制流），
  不可编辑：材质数据被绑死在 `PhongMaterialValue`，光照依赖 `vsg::Light` 节点 +
  `ViewDependentState` 的 `lightData`，阴影依赖 vsg 内建黑盒。
- 实测结论（详见 `graphics-shadow.md` §10）：vsg 内建阴影在**独立 viewer**（vsg_probe）
  可用，但在我们引擎（多 view + 每帧重建光节点 + 自定义宿主）里集成不可靠、且黑盒。
- 目标：
  1. 我们拥有材质/光照/阴影的**完整数据布局与着色逻辑**，跨驱动确定、可扩展多光/PCF；
  2. 摆脱对 vendored blob 与 vsg::Light / PhongMaterialValue 的耦合；
  3. 这套 ABI 后端无关，为未来自研后端（Diligent / Volk+VMA）留好接缝。

## 2. vsg 边界

### 弃用（语义层，回归我们）
- `createPhongShaderSet()` / vendored phong / `PhongMaterialValue`
- `vsg::Light` 节点（`AmbientLight/DirectionalLight/...`）+ VDS 的 `lightData`
  （我们改用每帧 `LightsUBO`，直接由 `Scene.lights()` 打包）
- vsg 内建阴影（`HardShadows`/`ShadowSettings`/shadow 预渲染）——阴影改走我们自己的
  engine depth pass + 自写采样

### 保留（工程层，仍由 vsg 提供）
- `vsg::Window`（表面/交换链/多缓冲/acquire-present）、`vsg::Device`
- `CommandGraph` / `RenderGraph` / `View` / `RecordTraversal`（多视口、per-viewID 管线缓存）
- `RenderPass` / `Framebuffer`（主窗 + 离屏可采样 + depth-only）
- `GraphicsPipelineConfigurator` + `ShaderSet`（装配**我们自己的** stages/bindings）
- `ShaderCompiler`（GLSL→SPIR-V）、Buffer/Image/ImageView/Descriptor 等资源
- 顶点数组映射不变：保留 `vsg_Vertex / vsg_Normal / vsg_Color` 属性名
  → `SceneBridge::assignArray` 无需改动

## 3. 关键机制约束（来自 vendored vsg 源码核对）

- vsg 录制只在 ShaderSet **声明了 view-dependent binding**（名字如 `lightData`、
  `viewportData`、`shadowMaps`，经 `ViewDependentStateBinding`）时，才经
  `BindViewDescriptorSets` 在 set0 自动绑 VDS（`GraphicsPipelineConfigurator::_assignInheritedSets`）。
- 因此**自写 ShaderSet 只要不声明这些名字，VDS 就完全不介入**，set 布局归我们。
  现有 `makeScreenTextureNode` 已实证：不含 VDS 的自定义 ShaderSet 录制/渲染正常。
- 干净做法：把 `view->features` 置 `0`（或仅保留 `INHERIT_VIEWPOINT`），使 VDS 连
  lightData/阴影资源都不分配，避免无谓开销。

## 4. Shader ABI 草案

### 4.1 管线构建
`buildVineShaderSet(ShaderPreset preset, extent, depth_test)` 取代 `buildShaderSet()`：
- `ShaderPreset`（graphics 语义枚举，见 §8）是**着色轴的选择键**；未来每个 preset 产出自己的
  stages / bindings（Phong / Flat / PBR / Shadowed 变体）。
- 用 `ShaderCompiler` 编译自写 GLSL 450 VS/FS（运行时）；
- 装配与现状相同的 default states（depth/raster/cull-none/blend/inputAssembly/multisample/viewport）；
- 自写 binding 命名避免与 vsg view-dependent 名冲突（`vine_*` 前缀）。

### 4.2 描述符 set 布局
```
set0  每帧
  b0  FrameUBO    { mat4 viewProj; mat4 view; mat4 proj; vec3 cam_pos; ... }
  b1  LightsUBO   { uint count; VineLight lights[N]; }   // 由 Scene.lights() 打包
  b2  sampler2DShadow shadow_map    // 该视图有 castShadow 光时才绑（我们自己的 depth RT）
set1  材质 / 每 drawable
  b0  MaterialUBO { vec4 base_color; vec4 emissive; float shininess; uint flags; ... }
      （或 dynamic 大缓冲 + dynamic offset，见 4.4）
set2  贴图（未来扩展）
```
`VineLight` 结构（先支持方向光）：
```
vec4  dir_and_type;    // xyz=方向, w=类型(0 ambient/1 dir/2 point/3 spot)
vec4  color_and_intensity;
vec4  params;          // spot 内外角 / point 衰减等
mat4  shadow_matrix;   // 仅 castShadow 有效
```
- shadow 采样：receiver 世界坐标 × `shadow_matrix` → [0,1] 深度比较
  （matrix = lightProj × lightView × model⁻¹ 约定，与 engine 光相机一致）。
- 先用 Hard（PCF 后续）；bias 取自 `Light::ShadowSettings.bias` 默认 0.002。

### 4.3 VS/FS
- VS：读 `vsg_Vertex/vsg_Normal/vsg_Color`；输出世界法线/位置（+ 未来 uv）；
- FS：材质 × 累加（ambient + N·L 方向光 + specular），有阴影则乘 shadow 项；
- **interface 变量名必须与顶点/片元精确一致**（glslang 链接要求），沿用
  `makeScreenTextureNode` 的教训。

### 4.4 每 drawable 的 model 矩阵与材质数据
推荐方案：**一块 dynamic UBO（矩阵 + 材质数组），按 drawable dynamic offset 绑定**，
每帧从命令流填充（复用 `SceneBridge` 的逐帧 reconcile 思路），管线数量收敛（少切换）。
备选：每 drawable 独立小 UBO（实现简单、切换多）。P0 可先走独立小 UBO，P2 再收敛为 dynamic。

### 4.5 数据上游（不变 & 新增）
- Frame/Lights：每帧从引擎相机 + 该 pass 内容场景的 `Scene.lights()` 打包
  （引擎 `RenderPass::execute → setLights` 已按 pass 转发内容光，天然可用）。
- Material：`Vine Material` 仍是**唯一真相源**；后端把其参数打包进 `MaterialUBO`
  （替代 `PhongMaterialValue`），缓存键仍是 `Material*`（闭环见资源生命周期管理文档）。
- 阴影 depth pass：使用 `RenderEngine` 已算好的**光相机（正交）**与 engine depth RT
  （1024² D24）——不再需要 vsg 内建 shadow 预渲染。

## 5. Node / 录制层不变
- `SceneBridge` 仍产出"壳"：`MatrixTransform → StateGroup → (Bind*/Draw*)`；只换
  StateGroup 内管线/描述符内容为自定义。
- **Node 是 draw 载体壳，语义全在 shader**——不依赖 vsg 场景图语义（无 Light/LOD/Bin）。

## 6. 分阶段落地

- **P0 垂直切片**：`buildVineShaderSet` 平替 `buildShaderSet`，只用于主场景几何；
  先复刻当前 phong 光照（ambient+方向光）使画面与现有输出一致（主视图/PiP 对照 A/B）。
- **P1 自写阴影**：绑定我们自己的 depth RT + `shadow_map` 采样，替换 vsg 内建。
- **P2**：材质 dynamic 化 / 多光 / overlay 迁移（overlay 可暂留 vsg phong 作内部特例）。
- 每阶段 GraphicsTest 不依赖后端，保持不变；用 lavapipe 截图 A/B + 真机 validation 验证。

## 7. 决策记录

- (2026-09-03) 选"自定义着色器"路线替代 vsg vendored phong；弃用
  `vsg::Light` / `PhongMaterialValue` / vsg 内建阴影；vsg 保留为工程层。
- 依据：vsg 内建阴影在独立 viewer 可用但引擎内集成不可靠（`vsg_probe`，见
  `graphics-shadow.md` §10）；blob phong 不可改。
- ABI 后端无关，作为未来 Diligent / 自研 Vulkan 后端的接缝。

## 8. ShaderPreset 语义与过渡映射（2026-09-03 已落地 + 像素验证）

### 8.1 语义枚举（graphics SDK，后端无关）
`sdk/vine/graphics/ShaderPreset.hpp`：

```cpp
enum class ShaderPreset { StandardPhong, FlatShaded, Pbr, ShadowedPhong };
```

- **StandardPhong**：lit Phong（diffuse/specular/ambient）——当前默认。
- **FlatShaded**：unlit 平面着色（常量色）。
- **Pbr** / **ShadowedPhong**：**预留**，暂无后端实现（见下）。
- 归属：**渲染配置**（`RenderEngine` 持有，`setShaderPreset/shaderPreset`），初始化前转发后端
  （`RenderBackend::setShaderPreset` 默认 no-op）。**不放进 RenderPipelineBuilder**——preset 是
  "几何怎么着色"的着色轴，与 pass 拓扑（builder）正交；builder 仍是纯配方层。

### 8.2 过渡映射（vsg 内建 set，待 P0 自写替换）

- `StandardPhong` → `vsg::createPhongShaderSet()`
- `FlatShaded`   → `vsg::createFlatShadedShaderSet()`
- `Pbr` / `ShadowedPhong` → **预留**：vsg 1.1.16 虽有 `createPhysicsBasedRenderingShaderSet()`，
  但其 `material` 描述符是 `PbrMaterialValue`（非 PhongMaterialValue），SceneBridge/VsgMaterialManager
  的材质路径不兼容 → 需自定义材质路径，随 P0/P1；ShadowedPhong 随 shadow 路线（最后）。
- **兼容性事实（vsg_shader_dump 核对）**：flat 与 phong 的 `material` 描述符**都是**
  `vsg::PhongMaterialValue`（set1/binding10）→ 共享的 SceneBridge（`assignDescriptor("material",
  phong)`）+ VsgMaterialManager 材质管线**无需改动**即可切 FlatShaded。
- 工具：`vsg_shader_dump` 现 dump flat/phong/pbr 三套 attribute + descriptor + data 类型。

### 8.3 验证记录（lavapipe/Weston，设备像素 378x234）

- **preset 确实生效**：同相机同场景实拍 phong vs flat 全帧像素差 mad≈11.6；最亮孤立黄 cube
  区域 mad≈36.5（flat 无高光/渐变 → 纯色）。两 preset 均 exit=124 稳定、无管线/校验错误。
- **offscreen + PiP 两 preset 均正常**：`VINE_VSG_OFFSCREEN=1` → offscreen 640x360 挂载，
  PiP 189x106@(181,120) 右下挂载；PiP 区放大 = 整帧缩小版 mini-frame（灰菱形地面 + 彩块堆栈），
  证明 离屏渲染 → 发布 SceneColor → ScreenPass 采样上屏链路在跑。
- 环境开关：`VINE_SHADER_PRESET`（存在→FlatShaded）；`VINE_VSG_OFFSCREEN` 开离屏验证。
- 验证入口：`./build/bin/Vine`（非 install 副本），xwd 抓子窗口 + 自写 XWD→PNG 解码。

### 8.4 与 §4/§6 的关系
- P0 自写 `buildVineShaderSet(ShaderPreset, ...)` 时，StandardPhong 须先复刻当前 phong 输出
  （§6 P0 A/B 对照）；届时 FlatShaded/Pbr/ShadowedPhong 各自产出自写 stages/bindings，
  过渡映射（8.2）退役。
