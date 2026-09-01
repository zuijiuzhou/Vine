# Graphics 模块设计（现代化架构）

> 状态：设计稿 v2（完整功能集）
>
> **目标**：为 Vine 提供场景图管理和渲染抽象层，支持多相机、多渲染通道、交互操纵、拾取。
> 核心功能：场景树管理、可视对象、相机操纵、射线求交、渲染管道、后端抽象。

## 1. 背景与目标

`Graphics` 模块是 Vine 可视化系统的中间层，负责：
1. **场景管理**：以树形结构组织可渲染对象
2. **相机系统**：支持多相机、视图矩阵、投影矩阵
3. **相机操纵**：轨道、摇移、缩放、第一人称等交互方式
4. **拾取系统**：射线-几何体求交、选择框选
5. **渲染管道**：多 Pass、多 RenderTarget 支持
6. **渲染抽象**：定义后端接口，支持 OpenGL/Vulkan/等

不负责：
- 实际渲染实现（交给后端 RenderBackend）
- UI 布局（属于 AppFramework）
- 物理模拟（属于其他模块）

## 2. 模块结构

```
src/viz/graphics/
  CMakeLists.txt
  include/vine/graphics/
    graphics_global.hpp          # API export 宏 + 命名空间
    
    # 场景与几何体
    Drawable.hpp                 # 可绘制对象基类
    Geometry.hpp                 # 几何体（Mesh/BRep）
    Material.hpp                 # 材质定义
    Scene.hpp                    # 场景容器
    
    # 相机与视图
    Camera.hpp                   # 相机（替代旧 View）
    Viewport.hpp                 # 视口（屏幕区域）
    CameraManipulator.hpp        # 相机操纵器（轨道/摇移/缩放）
    
    # 拾取与求交
    Ray.hpp                      # 射线定义
    RayIntersection.hpp          # 射线-几何体求交
    Pickable.hpp                 # 可拾取对象接口
    
    # 渲染管道
    RenderPass.hpp               # 渲染通道（相机+目标+清除状态）
    RenderTarget.hpp             # 渲染目标（颜色+深度缓冲）
    RenderCommand.hpp            # 渲染命令
    RenderBackend.hpp            # 渲染后端抽象接口
    
    # 可选
    Light.hpp                    # 光源
    
  src/
    Drawable.cpp
    Geometry.cpp
    Material.cpp
    Scene.cpp
    Camera.cpp
    Viewport.cpp
    CameraManipulator.cpp
    Ray.cpp
    RayIntersection.cpp
    RenderPass.cpp
    RenderTarget.cpp
    RenderCommand.cpp
    RenderBackend.cpp
```

## 3. 核心类设计

### 3.1 `Drawable`（可绘制对象基类）

所有可在场景中渲染的对象的基类。

```cpp
class V_GRAPHICS_API Drawable : public Object, public RefCounted<Drawable> {
    V_OBJECT_META_DECL;
    V_DISABLE_COPY(Drawable);

  public:
    Drawable();
    ~Drawable();

  public:
    /** @brief 绘制对象名称。 */
    String name() const;
    void setName(const String& name);

    /** @brief 对象是否可见。*/
    bool isVisible() const;
    void setVisible(bool visible);

    /** @brief 获取模型矩阵（局部坐标系）。 */
    Mat4d localTransform() const;
    void setLocalTransform(const Mat4d& transform);

    /** @brief 获取世界坐标系矩阵（由父节点计算）。 */
    Mat4d worldTransform() const;

    /** @brief 边界框（局部坐标系）。 */
    BoundingBox boundingBox() const;

    /** @brief 材质引用。 */
    intrusive_ptr<Material> material();
    void setMaterial(Material* material);

  protected:
    /** @brief 子类实现：计算局部边界框。 */
    virtual BoundingBox computeBoundingBox() const = 0;

  private:
    struct Data;
    Data* const d;
};

using DrawablePtr = intrusive_ptr<Drawable>;
```

### 3.2 `Geometry`（几何体）

表示网格、BRep 等可绘制的几何对象。

```cpp
class V_GRAPHICS_API Geometry : public Drawable {
    V_OBJECT_META_DECL;

  public:
    enum class Type {
        TriangleMesh,      // 三角网格
        IndexedMesh,       // 有索引网格
        BRep,              // 边界表示（OCCT）
        Primitive,         // 基本体（球、立方体等）
    };

  public:
    Geometry();

    /** @brief 几何体类型。 */
    Type geometryType() const;

    /** @brief 获取关联的 vine::geometry::Shape（可为空）。 */
    const vine::geometry::Shape* shape() const;
    void setShape(vine::geometry::Shape* shape);

    /** @brief 三角形数量。 */
    std::size_t triangleCount() const;

    /** @brief 顶点数量。 */
    std::size_t vertexCount() const;

  protected:
    BoundingBox computeBoundingBox() const override;

  private:
    struct Data;
    Data* const d;
};

using GeometryPtr = intrusive_ptr<Geometry>;
```

### 3.3 `Material`（材质）

定义对象的渲染属性。

```cpp
class V_GRAPHICS_API Material : public Object, public RefCounted<Material> {
    V_OBJECT_META_DECL;

  public:
    Material();

  public:
    /** @brief 材质名称。 */
    String name() const;
    void setName(const String& name);

    /** @brief 漫反射颜色 RGBA。 */
    Color diffuse() const;
    void setDiffuse(const Color& color);

    /** @brief 高光颜色 RGB，A=镜面强度。 */
    Color specular() const;
    void setSpecular(const Color& color);

    /** @brief 环境光颜色 RGB。 */
    Color ambient() const;
    void setAmbient(const Color& color);

    /** @brief 光泽度（Phong 指数）。 */
    float shininess() const;
    void setShininess(float shine);

    /** @brief 透明度 [0, 1]，0=完全透明，1=完全不透明。 */
    float opacity() const;
    void setOpacity(float alpha);

    /** @brief 纹理文件路径（可为空）。 */
    String textureFile() const;
    void setTextureFile(const String& path);

  private:
    struct Data;
    Data* const d;
};

using MaterialPtr = intrusive_ptr<Material>;
```

### 3.4 `Scene`（场景）

场景图容器，管理所有可绘制对象的树形关系。

```cpp
class V_GRAPHICS_API Scene : public Object, public RefCounted<Scene> {
    V_OBJECT_META_DECL;
    V_DISABLE_COPY_MOVE(Scene);

  public:
    Scene();
    ~Scene();

  public:
    /** @brief 场景名称。 */
    String name() const;
    void setName(const String& name);

    /** @brief 添加根级可绘制对象。 */
    void addDrawable(Drawable* drawable);

    /** @brief 移除根级可绘制对象。 */
    void removeDrawable(Drawable* drawable);

    /** @brief 获取所有根级可绘制对象。 */
    std::vector<DrawablePtr> drawables() const;

    /** @brief 根据名称查找对象（递归搜索）。 */
    DrawablePtr findDrawable(const String& name) const;

    /** @brief 删除所有对象。 */
    void clear();

    /** @brief 场景中所有对象的总边界框。 */
    BoundingBox boundingBox() const;

    /** @brief 收集所有需要渲染的命令。 */
    std::vector<RenderCommand> collectRenderCommands(const View* view) const;

  private:
    struct Data;
    Data* const d;
};

using ScenePtr = intrusive_ptr<Scene>;
```

### 3.5 `Camera`（相机）

仅管理相机参数和矩阵计算，不管理渲染目标或视口。

```cpp
class V_GRAPHICS_API Camera : public Drawable {
    V_OBJECT_META_DECL;

  public:
    enum class ProjectionType {
        Perspective,   // 透视投影
        Orthographic,  // 正交投影
    };

  public:
    Camera();
    ~Camera();

  public:
    /** @brief 投影类型。 */
    ProjectionType projectionType() const;
    void setProjectionType(ProjectionType type);

    // 相机参数（全局坐标系）
    /** @brief 相机位置。 */
    Vec3d eye() const;
    void setEye(const Vec3d& position);

    /** @brief 相机看向的目标点。 */
    Vec3d target() const;
    void setTarget(const Vec3d& point);

    /** @brief 向上向量。 */
    Vec3d up() const;
    void setUp(const Vec3d& upVector);

    // 视锥参数
    /** @brief 近裁剪面距离。 */
    double nearPlane() const;
    void setNearPlane(double distance);

    /** @brief 远裁剪面距离。 */
    double farPlane() const;
    void setFarPlane(double distance);

    // 透视投影参数
    /** @brief 视角（FOV，度数）。 */
    double fieldOfView() const;
    void setFieldOfView(double fovDegrees);

    // 正交投影参数
    /** @brief 正交投影的视口高度。 */
    double orthographicHeight() const;
    void setOrthographicHeight(double height);

  public:
    /** @brief 计算视图矩阵。 */
    Mat4d viewMatrix() const;

    /** @brief 屏幕坐标转世界坐标（射线）。 */
    Ray screenToWorldRay(const Vec2d& screenPos, const Viewport* viewport) const;

  private:
    struct Data;
    Data* const d;
};

using CameraPtr = intrusive_ptr<Camera>;
```

### 3.5b `Viewport`（视口）

管理屏幕区域和投影参数。

```cpp
class V_GRAPHICS_API Viewport {
  public:
    Viewport();
    ~Viewport();

  public:
    /** @brief 视口宽度（像素）。 */
    int width() const;
    void setWidth(int w);

    /** @brief 视口高度（像素）。 */
    int height() const;
    void setHeight(int h);

    /** @brief 设置视口尺寸。 */
    void setSize(int w, int h);

    /** @brief 视口宽高比。 */
    double aspectRatio() const { return static_cast<double>(width()) / height(); }

    /** @brief 计算透视投影矩阵。 */
    Mat4d perspectiveMatrix(double fov, double nearPlane, double farPlane) const;

    /** @brief 计算正交投影矩阵。 */
    Mat4d orthographicMatrix(double height, double nearPlane, double farPlane) const;

  private:
    struct Data;
    Data* const d;
};
```

### 3.5c `CameraManipulator`（相机操纵器）

处理用户交互，更新相机参数。

```cpp
class V_GRAPHICS_API CameraManipulator {
  public:
    enum class Mode {
        Orbit,           // 轨道（绕目标点旋转）
        Pan,             // 摇移（平移）
        Zoom,            // 缩放
        FirstPerson,     // 第一人称
    };

  public:
    CameraManipulator(Camera* camera);

  public:
    /** @brief 当前模式。 */
    Mode mode() const;
    void setMode(Mode m);

    // 轨道模式
    /** @brief 轨道旋转（绕目标点）。 */
    void orbit(double deltaYaw, double deltaPitch);

    /** @brief 设置轨道中心（目标点）。 */
    void setOrbitCenter(const Vec3d& center);

    /** @brief 获取轨道半径。 */
    double orbitRadius() const;
    void setOrbitRadius(double radius);

    // 摇移模式
    /** @brief 摇移相机（屏幕坐标）。 */
    void pan(double screenDx, double screenDy, const Viewport* viewport);

    // 缩放模式
    /** @brief 缩放（滚轮）。 */
    void zoom(double factor);

    // 第一人称模式
    /** @brief 第一人称前后移动。 */
    void moveForward(double distance);
    void moveRight(double distance);
    void moveUp(double distance);

    /** @brief 第一人称旋转。 */
    void rotate(double deltaYaw, double deltaPitch);

    // 通用
    /** @brief 应用到相机。 */
    void apply();

  private:
    struct Data;
    Data* const d;
};
```

### 3.6 `RenderCommand`（渲染命令）

表示单个可绘制对象的渲染指令。

```cpp
struct V_GRAPHICS_API RenderCommand {
    /** 可绘制对象。 */
    DrawablePtr drawable;

    /** 材质。 */
    MaterialPtr material;

    /** 世界坐标系矩阵。 */
    Mat4d modelMatrix;

    /** 是否透明（需要后序渲染）。 */
    bool isTransparent = false;

    /** 几何体数据引用。 */
    const vine::geometry::Shape* shape = nullptr;

    /** @brief 默认构造。 */
    RenderCommand() = default;

    /** @brief 构造并初始化。 */
    RenderCommand(Drawable* d, Material* m, const Mat4d& model)
      : drawable(d), material(m), modelMatrix(model)
    {
        if (d) {
            isTransparent = (m && m->opacity() < 1.0f);
        }
    }

    /** @brief 计算渲染优先级（用于排序）。 */
    float depth(const Vec3d& cameraEye) const
    {
        if (!drawable) return 0.0f;
        Vec3d center = modelMatrix * Vec3d::Zero();
        return static_cast<float>((center - cameraEye).norm());
    }
};
```

### 3.7 `RenderBackend`（渲染后端抽象）

定义不同渲染实现的接口。

```cpp
class V_GRAPHICS_API RenderBackend {
  public:
    virtual ~RenderBackend() = default;

    /** @brief 初始化后端。 */
    virtual bool initialize() = 0;

    /** @brief 清理后端资源。 */
    virtual void shutdown() = 0;

    /** @brief 开始一帧渲染。 */
    virtual void beginFrame() = 0;

    /** @brief 结束一帧渲染。 */
    virtual void endFrame() = 0;

    /** @brief 执行渲染命令列表。 */
    virtual void render(const std::vector<RenderCommand>& commands, const View* view) = 0;

    /** @brief 清空颜色和深度缓冲。 */
    virtual void clear(const Color& backgroundColor) = 0;

    /** @brief 设置视口大小。 */
    virtual void setViewport(int x, int y, int width, int height) = 0;

    /** @brief 交换缓冲区（用于 double buffer）。 */
    virtual void swapBuffers() = 0;

  protected:
    RenderBackend() = default;
};

using RenderBackendPtr = std::unique_ptr<RenderBackend>;
```

## 4. 设计模式与约定

### 4.1 引用计数
- `Scene`、`View`、`Drawable`、`Material` 都继承 `RefCounted<T>`
- 使用 `intrusive_ptr<T>` 管理所有权
- 公开 API 接受原始指针（调用者管理生命周期）或返回 `intrusive_ptr`

### 4.2 不可复制/移动
- `Scene` / `View` 使用 `V_DISABLE_COPY_MOVE` 防止意外复制
- `Drawable` 允许移动（子类可选择禁用）

### 4.3 数据隐藏（Pimpl）
- 所有公开类都使用 `Data* const d` 指针隐藏实现
- 便于二进制兼容性和后续扩展

### 4.4 命名规范
遵循 Copilot 编码指引：
- 实例方法：camelCase，无 `get` 前缀（如 `name()`）
- 布尔 getter：`is`/`has` 前缀（如 `isVisible()`）
- setter：`set` 前缀（如 `setName()`）
- 类型别名：`Ptr` 后缀（如 `ScenePtr`）

## 5. 实现阶段规划

### 阶段 1：框架（当前）
- [ ] 定义所有公开头文件和 API
- [ ] 实现基础类框架（空 Data 结构）
- [ ] 编写单元测试框架

### 阶段 2：场景管理
- [ ] 实现 `Scene` 树形结构
- [ ] 实现 `Drawable` 的层级变换
- [ ] 递归边界框计算
- [ ] 场景查询接口

### 阶段 3：视图管理
- [ ] 实现 `View` 相机参数
- [ ] 视图矩阵和投影矩阵计算
- [ ] 屏幕-世界坐标转换
- [ ] 视锥剔除（Frustum Culling）

### 阶段 4：几何与材质
- [ ] 集成 `vine::geometry::Shape`
- [ ] `Geometry` 包装层
- [ ] `Material` 属性管理
- [ ] 纹理管理（可选）

### 阶段 5：渲染后端
- [ ] 定义 `RenderBackend` 接口
- [ ] OpenGL 后端实现（可选）
- [ ] 命令缓冲系统
- [ ] 排序与批处理

## 6. 依赖关系

```
Graphics
  ├── vi::Core              (Object, RefCounted, intrusive_ptr)
  ├── vi::Global            (Math: Vec3d, Mat4d, Color, etc.)
  ├── vi::Geometry          (Shape, BoundingBox)
  └── [可选] vi::RenderBackend
```

不反向依赖其他模块。

## 7. API 示例

```cpp
using namespace vine::graphics;

// 创建场景
auto scene = make_intrusive<Scene>();
scene->setName(u8"Main Scene");

// 创建材质
auto material = make_intrusive<Material>();
material->setName(u8"RedPlastic");
material->setDiffuse(Color(1.0f, 0.0f, 0.0f, 1.0f));
material->setOpacity(0.8f);

// 从 Shape 创建几何体
auto geom = make_intrusive<Geometry>();
geom->setName(u8"Sphere");
geom->setShape(sphere_shape);
geom->setMaterial(material.get());
geom->setLocalTransform(transform);

// 添加到场景
scene->addDrawable(geom.get());

// 创建视图
auto view = make_intrusive<View>();
view->setName(u8"MainCamera");
view->setScene(scene.get());
view->setEye(Vec3d(0, 10, 15));
view->setTarget(Vec3d(0, 0, 0));
view->setUp(Vec3d(0, 1, 0));
view->setFieldOfView(60.0);
view->setAspectRatio(16.0 / 9.0);

// 收集渲染命令
auto commands = scene->collectRenderCommands(view.get());

// 渲染
backend->clear(Color(0.2f, 0.2f, 0.2f, 1.0f));
backend->render(commands, view.get());
backend->swapBuffers();
```

## 8. 文件清单

### 头文件（include/vine/graphics/）
- `graphics_global.hpp` — API 导出和命名空间宏
- `Drawable.hpp` — 可绘制对象基类
- `Geometry.hpp` — 几何体类
- `Material.hpp` — 材质定义
- `Scene.hpp` — 场景容器
- `View.hpp` — 视图/相机
- `RenderCommand.hpp` — 渲染命令结构
- `RenderBackend.hpp` — 后端抽象接口

### 实现文件（src/）
- `Drawable.cpp`
- `Geometry.cpp`
- `Material.cpp`
- `Scene.cpp`
- `View.cpp`
- `RenderCommand.cpp`

### 测试文件（tests/test_graphics/）
- `GraphicsTest.cpp` — 基本功能测试
- `SceneTest.cpp` — 场景树操作测试
- `ViewTest.cpp` — 相机变换测试

## 9. 未来扩展点

1. **光源系统**：环境光、定向光、点光源、聚光灯
2. **阴影**：阴影贴图、实时阴影
3. **粒子系统**：粒子发射、动画
4. **动画系统**：骨骼动画、关键帧插值
5. **优化**：八叉树加速、脏标记、缓存策略
6. **多渲染后端**：OpenGL、Vulkan、软光栅化
7. **UI 集成**：ImGui 或自定义 UI 系统
8. **拾取**：射线检测、拾取缓冲

