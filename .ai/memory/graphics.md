# Graphics 模块核心

> 2026-09-03 更新：渲染后端(vsg)走向"自定义着色器路线"——语义层(材质/光照/阴影/着色)
> 全部归 graphics，vsg 仅保留工程层(窗口/交换链/命令/管线构建/录制)。设计见
> `.ai/design/vsg-custom-shader.md`。同时补齐 vsg 资源生命周期闭环：
> `RenderBackend::releaseOverlay/releaseRenderTarget`(默认空实现) +
> `RenderEngine` 删除点接线(removeOverlay/clearOverlays/removePass/clearPasses/shadow 剪枝) +
> `VsgRenderer` 摘除 overlay View / offscreen graph / PiP slot 并 deviceWaitIdle。
> 遗留：Material 缓存释放未接线、Scene 几何靠 600 帧懒驱逐、Object 销毁钩子(自动兜底)未做。
> 测试：GraphicsTest 82 全绿（17 套件）。
>
> 2026-09-03 二更（Design B）：RenderEngine **不再有 main pass**，也不再自动建 scene/camera/pass。
> 引擎空启动：`scene()` 为可选默认内容(未绑 content 的 pass 用它，可 null)；`masterCamera()` 为
> 可选的交互主相机（manipulator 驱动它，不设则无效）。pipeline 完全显式——所有 pass 进统一
> `passes_` 注册表按 order 升序执行 + overlays 最后；"窗口 pass"= camera==masterCamera 且
> RT==null 的注册 pass（约定 order 0）。旧 `setCamera/camera`→`setMasterCamera/masterCamera`，
> `setMainPass/mainPass` 已删。RenderControl(fw) 作为"普通 viewer 引导层"：ctor 注入默认
> Scene+masterCamera，init() 在 passCount()==0 时注册默认窗口 pass。
> **阴影子系统已整体移出 RenderEngine**（addShadowPass/runShadowPasses/ShadowSlot/auto castShadow
> 调度全删，82 测试）：阴影 depth pass 现在就是普通注册 pass（光相机 + depth-only RT + order<0 +
> content）；Light::castShadow/ShadowSettings 保留为语义标志。
> ⚠ **路线决策**：shadow **排到最后**实现——前置 = 自定义 shader(buildVineShaderSet P0/P1) + 多 pass
> 完全成熟；届时才消费 castShadow/ShadowSettings。此前不跑任何半成品阴影：已剥除 vsg 内建
> HardShadows 探针(buildLightNode) + shadow-state 诊断 + VINE_VSG_SEED_LIGHTS_ONCE，demo sun 不再
> castShadow(VINE_VSG_NO_SHADOW 已删)。App 冒烟 exit=124。
>
> 2026-09-03 三更（着色预置）：graphics 加语义枚举 `ShaderPreset{StandardPhong, FlatShaded, Pbr,
> ShadowedPhong}`（`ShaderPreset.hpp`），由 RenderEngine（渲染配置）持有并在 initialize 前转发
> 后端（`RenderBackend::setShaderPreset` 默认 no-op）。vsg 后端映射：StandardPhong→phong ShaderSet、
> FlatShaded→flat ShaderSet（二者 "material" 描述符都是 PhongMaterialValue，SceneBridge 材质路径
> 通用，已验证）；Pbr/ShadowedPhong **预留**（Pbr 需 PbrMaterialValue，shadow 排最后）→ 暂回落
> Phong。builder 不掺和（preset 是着色轴，非 pass 拓扑轴）。GraphicsTest 84 全绿（+1 转发测试）。

**模块职责**：场景图管理、可视对象、相机视图、渲染抽象层

## 核心类关系

```
Object
  ├─ Drawable (可绘制对象基类)
  │   └─ Geometry (网格/BRep/基本体)
  ├─ Material (材质: 颜色、纹理、光泽)
  ├─ Scene (场景容器: 树形结构)
  └─ View (相机: 投影、变换)
```

## 主要 API

### Scene（场景）
- `addDrawable()` / `removeDrawable()` — 树形管理
- `findDrawable()` — 名称查询
- `boundingBox()` — 递归计算边界
- `collectRenderCommands()` — 收集渲染指令

### View（相机）
- 参数：eye, target, up, near/far, FOV, 宽高比
- `viewMatrix()` / `projectionMatrix()` — 矩阵计算
- `screenToWorldRay()` — 拾取射线

### Drawable（可绘制对象）
- `localTransform()` / `worldTransform()` — 层级变换
- `isVisible()` — 可见性
- `material()` — 材质绑定
- `boundingBox()` — 局部 AABB

### Geometry（几何体）
- `setShape()` — 关联 vine::geometry::Shape
- `geometryType()` — 类型：Mesh/BRep/Primitive

### Material（材质）
- RGB 颜色：diffuse, specular, ambient
- 参数：shininess, opacity (透明度)
- 可选：纹理文件路径

## 边界框类型（Aabbd）

- `Scene/Node/Drawable/Geometry::boundingBox()` / `computeBoundingBox()` 返回
  `vine::math::Aabbd`（`Rect3<double>` 别名，见 `vine/math/Rect3.hpp`）。
  原来的 `vine::graphics::BoundingBox` 已移除。
- **语义**：`Rect3` 默认构造为零点在原点的合法零盒；累积式构建必须用
  `Aabbd::empty()`（反转哨兵）起步，再用 `expandBy(Point3/Vector3/Rect3)`。
  空盒 `isEmpty()==true`、`isValid()==false`。
- **注意**：`Rect3.hpp` 只前向声明 `Point3/Vector3`，`min()/max()/center()/size()`
  的调用方需自行 include `vine/math/Point3.hpp`/`Vector3.hpp`；
  `vine::math::Point3d` 别名仅定义于 `Point3.hpp`。

## 设计特点

✓ **引用计数**：所有核心类（含 `CameraManipulator`）继承 `RefCounted<T>`，用
  `intrusive_ptr` 管理
✓ **借用/所有权分界**：getter 返回与“不 retain”的借用入参用 `vine::raw_ptr<T>`；
  会 retain（存入 owning 字段/容器）的 setter/add 入参用 `intrusive_ptr<T>`
  （by value + std::move，如 `setMaterial`、`Node::addChild`、`setScene`）；
  引擎持有操纵器经 `setCameraManipulator(intrusive_ptr<...>)`，`cameraManipulator()` 返 `raw_ptr`
✓ **Pimpl**：数据隐藏，二进制兼容性
✓ **无环依赖**：只依赖 Core、Global、Geometry；不反向依赖
✓ **后端抽象**：`RenderBackend` 接口支持多个实现（OpenGL/Vulkan）
✓ **命名规范**：无 `get` 前缀，`is`/`has` 布尔前缀，`set` setter

## RenderEngine 有序场景通道管线（2026-09 落地）

- 每帧执行：`pre passes(order<0)` → `main pass(order 0)` → `post passes(order>0)`
  → `overlays(升序 zOrder)`。
- `addPass(intrusive_ptr<RenderPass>, int order)` / `removePass(raw_ptr)` /
  `clearPasses()` / `passCount()`；同 order 稳定按插入序。
- **内容关联由 Engine 管理**：`addPass(pass, content, order)` 绑定显式场景，
  `bindPassContent` 重绑、`contentOf` 查询；执行按 `slot.content ?: engine.scene_`
  解析 → `setScene()` 对未绑定 pass 是单点更新。`RenderPass` 不携带 Scene。
- `RenderPass`：view(Camera)=借用(raw_ptr)；**输出 RenderTarget=持有(intrusive_ptr)**
  （析构在 .cpp 出外联）；null target=backbuffer；clear/viewport 在 pass 上。
- Overlay（HUD）始终最后；不 addPass 时行为与旧版一致。
- 未来 light：光源挂 Scene；shadow map = 以光源相机渲染的 order<0 通道。
- 衔接/数据传递（设计）：pass 输出=RenderTarget 纹理；将来后处理用“命名产出槽”
  (publish/resolve) 由 Engine 连接；当前阶段只需顺序 + 各自输出 target。
- v2a 平台层(2026-09-03)：RenderTarget 增 hasColor/hasDepth/colorFormat/depthFormat/valid()；
  RenderBackend 增 supportsRenderTargets()+离屏契约。vsg 离屏 scaffold 已实现（color±depth +
  createRenderPass/Framebuffer/RenderGraph + SceneBridge 同步，编译通过，GPU 未验证；默认路径不变），
  采样/合成属 v3。
- v2b(2026-09-03)：FrameContext 骨架(dt/尺寸, engine.frame/Resize 填充, frameContext() 暴露)；
  vsg 离屏 resize 时 deviceWaitIdle+从 CommandGraph.children 摘旧图重建；app_shell 提供
  VINE_VSG_OFFSCREEN=1 离屏验证入口（默认关）。
- v3(2026-09-03, lavapipe 实测)：命名产出槽 publish/resolve 落地 —— RenderPass
  setOutputName/addInputName + resolveInputTextures + execute 改 virtual；新 ScreenPass(默认不清屏)；
  RenderBackend.drawScreenTexture(source)；Engine 每帧帧首清 outputs_ 注册表、逐 pass
  执行前 resolve 输入/执行后 publish 输出（public publish/resolve/unpublish）。vsg：离屏
  renderpass 用 color finalLayout=SHADER_READ_ONLY+external 读依赖（makeSampleableRenderPass），
  离屏图插 command_graph 队首先录；drawScreenTexture 内嵌 GLSL(ShaderCompiler) 全屏纹理三角作主
  render_graph 第二 View 画 PiP（超面自动缩锚右下）。教训：ShaderCompiler 链接要求 VS/FS 接口变量
  同名；新增 View compile 失败须先从 render_graph 摘下再弃。验证：VINE_VSG_OFFSCREEN=1 右下 PiP
  正确显示离屏四色方块(偏暗=仅环境光)；GraphicsTest 68 全过(新增 5 用例)。
- v4a(2026-09-03, lavapipe 实测)：光源归属定案 **Scene 级**(不与 pass/node 绑定；node 级留 v5 升级,
  scene->lights() 换实现即可)。新增 Light(Ambient/Directional, Colorf+intensity+castShadow 预留)/
  Scene 光槽；RenderBackend.setLights(no-op 默认) 由 RenderPass::execute 在 render() 前从内容 scene
  下发(空=保留后端默认 headlight/ambient, 有光=替换)；vsg 主视图手工化(RenderGraph::create(window,
  main_view)+main_light_group(默认 createHeadlight)+vsg_scene)以拿 View 句柄挂/换灯, setGroupLights 每帧
  reconcile 各视图灯(灯节点无 GPU 资源, record 时收进 lightData, 无需重编译)。demo(env 门控) 给 engine
  scene 配 ambient0.25+sun 方向光 → 主/离屏同源、PiP 颜色与主一致(不再偏暗)。教训：vine String 不能赋给
  vsg std::string name。GraphicsTest 73 全过(新增 Light/Scene/RenderPass 传光 5 用例)。
- v4b-1(2026-09-03, CPU+lavapipe)：阴影=Scene 级光的属性+按需。Light::ShadowSettings(res/bias/filter)。
  Engine runShadowPasses()(帧首扫 castShadow 方向光→光正交相机(AABB 取景)+仅深度 RT+内部 pass, 先于主
  管线; 空 AABB/无消费跳过; 帧末释放) + 手动 addShadowPass(light,content)(显式注册, 自动让位, 不必
  castShadow)。vsg renderOffscreenTarget 支持仅深度目标(makeDepthOnlyRenderPass: storeOp=STORE+
  finalLayout SHADER_READ_ONLY+external→fragment 读依赖); clearValues 逐附件手填(勿用 setClearValues,
  其按 finalLayout==DEPTH_STENCIL 判型会误判 SHADER_READ depth)。实测日志见 1024x1024 shadow target
  先于颜色离屏, 无崩溃; GraphicsTest 77 全过(+4)。v4b-2 采样(自建 shadowed Phong)待做。

## 实现计划

1. **框架** → 头文件定义 + 空实现
2. **场景管理** → 树形结构 + 变换层级
3. **视图管理** → 相机矩阵 + 投影
4. **集成几何体** → Geometry 包装 Shape
5. **渲染后端** → OpenGL/Vulkan 实现

