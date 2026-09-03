# Graphics 可编程着色设计（用户写 GLSL / ShaderProgram）

> 状态：设计稿 v1（2026-09-03），评审对象。
> 上游/前身：`vine-shader.md`（后端 vsg 自写内置 shader 的 P0 落地稿；其 §11 为本文前身，以本文为准）。
> 关联：`graphics-scene-graph.md`（program 挂点）、`graphics-state.md`（状态参与变体键）、
> `graphics-render-pipeline.md`（pass 级 program + 命名产出槽）、`vsg-custom-shader.md`。
>
> **一句话**：SDK 第一准则——**用户必须能写 GLSL**；SDK 只给一个**薄着色接口**（源 + 类型化参数
> + 纹理/输入槽），不搞重型"契约"；内置 `ShaderPreset` 与用户 Program 是**同一模型**；vsg（乃至
> 自写 Vulkan/GL）只是把 Program 编译并装配成管线的可替换实现。
>
> ⚠ **编译能力（更新 2026-09-03）**：vsg 已**集成 glslang**——gfx_backend_vsg 在 VINE_USE_FETCHCONTENT
> 分支**源码构建 vsg** 并链系统 glslang-dev（`VSG_SUPPORTS_ShaderCompiler 1`，`ShaderCompiler.cpp` 已
> 编译），**运行期 GLSL→SPIR-V 可用**。因此 SDK 支持两条编译路径：
> - (a) **运行期**：后端 `vsg::ShaderCompiler` 直接把用户 GLSL 编成 SPIR-V（无外部工具依赖）；
> - (b) **离线/作者时**：`glslangValidator` 或 SDK 薄辅助 `compileGlslToSpirv()` 预编译（测试/工具链）。
> `ShaderProgram` 也可直接携带 SPIR-V（字节或 `.spv` 路径）。后端以 `ShaderSet.stages /
> attributeBindings / descriptorBindings / defaultGraphicsPipelineStates` **自描述装配**用户
> Program 并经 `GraphicsPipelineConfigurator` 成管线；`program()==nullptr` → 内置默认
> （ShaderPreset / vendored SPIR-V），零回归。依赖注记：无 glslang 的本地 vsg 安装仅在
> VINE_USE_FETCHCONTENT=OFF 分支使用（无运行期编译）。详见 §6/§7/§10。
>
> 📋 评审（2026-09-03）：P1 SDK 侧已落地——`ShaderProgram/ShaderStage`、`Geometry::setProgram`、
> `StateNode::setProgram`、`effectiveProgram`、`RenderCommand.program`（GraphicsTest 全绿）；
> 后端"编译半环"已验（`vsg::ShaderCompiler`，GlslCompileTest）且"自定义 ShaderSet 装配"经
> `vsg_color_probe custom` 探针在 lavapipe 验证（已入回归脚本）。**未接**：SceneBridge 遇
> `cmd.program` 建 ShaderSet 并喂视图/模型矩阵——卡点：vsg 内置 phong ShaderSet 是**序列化 blob**
> （`shaders/phong_ShaderSet.cpp` = io.read_cast 数据，无文本可镜像），且 vsg 内核/ViewDependentState
> 未暴露"模型矩阵/视图矩阵"的标准注入名；接线前需先逆向出 phong 的 push/descriptor 契约（用
> vsg_shader_dump 打印其 stages/attributeBindings/descriptorBindings/pushConstantRanges），勿盲改。

## 0. 为什么必须（多 pass 的意义）

多 pass 的价值 = 每个 pass 的着色逻辑可编程。若用户不能写 shader，ScreenPass 只能跑引擎写死的
全屏拷贝，`publish/resolve` 退化为"拷来拷去"，deferred/SSAO/泛光/自定义合成全做不了。
因此"用户写 GLSL"是**管线架构需求**，不是附加功能。

## 1. 薄接口（刻意不做成重型契约）

真正不可省只有一句：**shader 要有一个确定的输入接口**（矩阵、顶点属性、参数、pass 纹理怎么进）。
这跟后端无关，任何可编程渲染都有；但**不必**形式化为 location 表 + 布局推导 + 声明式块抽象——
那是"跨任意后端一次编写"才需要的。未来后端只有 vsg 与手写 Vulkan（同为 SPIR-V），用户 GLSL 两边
都能跑，因此接口可以极薄：

```cpp
// SDK（后端无关）：ShaderStage + 参数 + 槽
class ShaderStage { ShaderStageType type; String source; String entry; };  // VS/FS；compute 留缝
class ShaderProgram : public Object, public RefCounted<ShaderProgram> {
  String name();
  void addStage(ShaderStage);                 // 或 setVertexSource/setFragmentSource
  void addParam(String name, ParamType type); // float/vec2..4/mat4/int（布局后端推）
  void setParam(String name, const ParamValue&);
  void addTextureSlot(String name);           // pass 命名产出 或 纹理资产（P1）
  // 需要哪些顶点属性（按 loc）也声明在这里
};
```

**默认好用的关键**：`program()==nullptr` → 走引擎默认（内置程序由 ShaderPreset+材质+几何数据决定），
一行 GLSL 不碰、零回归。

## 2. 挂点与解析链

- `Geometry::setProgram(...)`（null=默认）——per-object 覆盖（graphics-scene-graph.md）；
- `StateNode::setProgram(...)`——给子树统一设（如"整块无光照"），可选；叶子优先；
- **pass 级**：`ScreenPass`/通用 pass 可挂 Program + 声明输入槽（见 §5）。

解析链：`Geometry.program` 非空用它；否则向上找最近 `StateNode.program`；否则引擎默认内置。

## 3. 最小绑定约定（后端保证，用户对着写）

- **顶点属性**：`loc 0 = position`（唯一定死）；其余 loc 由用户按需声明消费（默认内置程序按
  约定 loc0/1/2 = pos/normal/color，自定义程序读自己绑的 loc）。
- **每视图/每 draw 数据**（对外契约形态，避免 vsg 专属 push 泄漏）：
  - per-view `VineFrame{ view; inv_view; proj; view_proj; cam_pos; frame(time/viewport) }`
  - per-draw `VineDraw{ model; …用户参数… }`
  用户据此写 `gl_Position = view_proj * model * pos`。push constant 只允许作**后端内部优化**
  （须与上述契约可证等价），不是用户要写的接口。
- 这些约定由后端用 `ShaderSet`（attributeBindings/descriptorBindings）**自描述**实现，
  SDK 只传"源 + 槽声明"，不复制一份契约文档。

## 4. 参数机制

- 类型化参数表 → 后端推导 std140 布局 → per-drawable UBO（set1）；
  每帧仅当参数变时原地打包（复用 vsg Data/DYNAMIC 机制，MaterialUBO 是它的固定特例）。
- 不提供"按名字运行时 poking"（Vulkan 无此模型，且贵）。

## 5. pass 级 Program + 命名产出槽（多 pass 意义落点）

- 复用现有命名产出 `publish/resolve`：Program 声明消费的槽名（`SceneColor/Depth/ShadowMap/…`），
  engine resolve → 后端绑成 sampler → 输出再 publish。
- `ScreenPass::execute` 由固定全屏拷贝改为"跑用户 Program 的全屏三角形"（v1 先只支持全屏 +
  少量输入槽，不做透视变换）。

## 6. 编译 / 排错 / 热更（能力更新 2026-09-03，见顶部 ⚠）

- **交付形态**：`ShaderProgram` 作者用 **GLSL 源**；后端可**运行期编译**（vsg ShaderCompiler，glslang
  已集成）或**载入预编译 SPIR-V**。
- **编译路径**：
  - (a) 运行期：后端 `vsg::ShaderCompiler` 把 GLSL 直接编 SPIR-V（`VSG_SUPPORTS_ShaderCompiler 1`）；
  - (b) 离线：SDK 薄辅助 `compileGlslToSpirv(source, stage, entry)`——调 `glslangValidator` 子进程
    （工具缺失报清晰错误）；或直接给 SPIR-V（字节/`.spv`）→ `ShaderStage::read` / SPIRV 构造。
- **错误分层**：运行期编译错由 ShaderCompiler 上报；离线工具错由 stderr/exit；装配期由后端上报。
- 热更 = 重编译 / 重载 SPIR-V（P1 后可选）。

## 7. 与内置预设同构

- `ShaderPreset{StandardPhong, FlatShaded, Pbr, ShadowedPhong}` 退化为**内置 Program 的别名**
  （默认快路径，走同一 ShaderProgram 模型）；
- 不搞"内置一套机制、用户一套机制"。

## 8. 后端映射

| SDK | vsg | GL(假想) |
|---|---|---|
| ShaderProgram | ShaderSet(stages/attributeBindings/descriptorBindings) + GraphicsPipelineConfigurator | program + glVertexAttribPointer(uniform 由参数表下发) |
| 参数表 | `vsg::Value<…>`(DYNAMIC) → DescriptorBuffer(set1) | glUniform*（编译期布局） |
| 输入槽 | 采样命名产出的 ImageView → sampler | 纹理单元 |

## 9. 分期

- **P0（后端，vine-shader.md）**：自写内置 StandardPhong/FlatShaded 替换 vendored blob——先用
  vsg 内建/近端路径跑通，**内置 shader 从第一版就按 ShaderProgram 形状组织**，为同构铺路。
- **P1**：SDK `ShaderStage/ShaderProgram/Param` 类型（GLSL 源 + 可选 SPIR-V）+ `compileGlslToSpirv`
  离线辅助 + Geometry/StateNode 挂点（null=默认）；先把 pass 级全屏 shader 打通（SPIR-V 交付）。
- **P2（可选）**：per-drawable 自定义顶点数据 shader（点云等）深化、热更、GL 后端验证。

## 10. 决策记录（2026-09-03）

1. SDK 第一准则：用户必须能写 GLSL；着色契约先于后端（vsg 可弃）。
2. **薄接口**：不做重型契约/布局推导；接口最小化到"源 + 类型化参数 + 槽"。
3. loc0=position 为唯一固定约定；per-view/per-draw 用 `VineFrame/VineDraw` 声明式块，push 仅内部。
4. 内置 ShaderPreset 与用户 Program 同一模型；默认 null=内置，用户可覆盖（Geometry/StateNode/pass）。
5. 多 pass 意义 = pass 级 Program + 命名产出槽（复用现有 publish/resolve）。

6. **编译能力（2026-09-03，初稿→更新）**：初判"无 glslang → 仅离线"；随后**集成 glslang**
   （vsg 源码构建 + 系统 glslang-dev → 运行期 `ShaderCompiler` 可用，`VSG_SUPPORTS_ShaderCompiler 1`）。
   最终：**运行期编译可用（默认）**，离线 `glslangValidator` / 直接 SPIR-V 为兜底；作者形态 = GLSL 源。
7. **绑定契约自描述**：用户 Program 声明所需顶点属性（loc0=position 固定）与 uniform/纹理槽；
   后端按 `ShaderSet.attributeBindings/descriptorBindings` 自描述绑定数组与参数，SDK 不复制契约文档。
8. **默认不变**：`program()==nullptr` → 内置（ShaderPreset / vendored SPIR-V），零回归。
