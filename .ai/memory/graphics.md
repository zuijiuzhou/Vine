# Graphics 模块核心

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

## 实现计划

1. **框架** → 头文件定义 + 空实现
2. **场景管理** → 树形结构 + 变换层级
3. **视图管理** → 相机矩阵 + 投影
4. **集成几何体** → Geometry 包装 Shape
5. **渲染后端** → OpenGL/Vulkan 实现

