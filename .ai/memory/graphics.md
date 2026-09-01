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

## 设计特点

✓ **引用计数**：所有核心类继承 `RefCounted<T>`，用 `intrusive_ptr` 管理
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

