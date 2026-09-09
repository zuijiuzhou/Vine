#pragma once
#include "graphics_global.hpp"

#include <vector>

#include <vine/Object.hpp>
#include <vine/RefCounted.hpp>
#include <vine/intrusive_ptr.hpp>
#include <vine/raw_ptr.hpp>
#include <vine/String.hpp>

#include "Light.hpp"
#include "Node.hpp"

V_GRAPHICS_NS_BEGIN

using vine::math::Aabbd;

class Camera;
struct RenderCommand;

/**
 * @brief Scene container owning one root subtree plus lights.
 *
 * A scene holds exactly one root node of the scene graph (an empty scene has
 * no root and renders nothing), the scene's light sources, and provides tree
 * queries: bounding box computation, name-based search, and render command
 * collection. Content is composed under the root through Group; the root is
 * typically a Group holding the world's top-level subtrees.
 */
class V_GRAPHICS_API Scene : public Object, public RefCounted<Scene> {
    V_OBJECT_META_DECL;
    V_DISABLE_COPY_MOVE(Scene);

  public:
    Scene();
    ~Scene();

  public:
    /** @brief Gets the scene name. */
    String name() const;

    /** @brief Sets the scene name. */
    void setName(const String& name);

    /** @brief Returns whether the whole scene is rendered. */
    bool isVisible() const;

    /** @brief Sets whether the whole scene is rendered. */
    void setVisible(bool visible);

    /** @brief Gets the scene-level opacity multiplier in [0, 1]. */
    float opacity() const;

    /** @brief Sets the scene-level opacity multiplier in [0, 1]. */
    void setOpacity(float opacity);

    /** @brief Gets the root node of the scene.
     *
     * A scene renders exactly one root subtree; content is composed under it
     * through Group. Returns null for an empty scene (nothing is rendered).
     *
     * @return The scene root node, or null.
     */
    NodePtr root() const;

    /** @brief Sets the root node of the scene.
     *
     * Replaces any previous root. Content is composed by attaching subtrees
     * under @p root (typically a Group) before rendering.
     *
     * @param root Root node to render; null empties the scene.
     */
    void setRoot(intrusive_ptr<Node> root);

    /** @brief Finds a node by name (recursive search from the root).
     *
     * @param name Name to search for.
     * @return Found node, or null.
     */
    NodePtr findNode(const String& name) const;

    /** @brief Removes the root node, leaving an empty scene.
     *
     * Lights are unaffected; use clearLights() to drop them.
     */
    void clear();

    /** @brief Adds a light source to the scene.
     *
     * The scene keeps a reference to the light. Passes rendering this scene
     * light their content with the scene's lights (RenderBackend::setLights).
     *
     * @param light Light to add.
     */
    void addLight(intrusive_ptr<Light> light);

    /** @brief Removes a light source from the scene.
     *
     * @param light Light to remove (by pointer).
     */
    void removeLight(raw_ptr<Light> light);

    /** @brief Removes all light sources. */
    void clearLights();

    /** @brief Gets the scene's light sources. */
    const std::vector<LightPtr>& lights() const;

    /** @brief Returns whether the scene has at least one light source. */
    bool hasLights() const;

    /** @brief Computes the bounding box of the whole scene. */
    Aabbd boundingBox() const;

    /** @brief Collects render commands for the given camera.
     *
     * @param camera Camera used for culling/ordering.
     * @return Collected render commands.
     */
    std::vector<RenderCommand> collectRenderCommands(raw_ptr<const Camera> camera) const;

  private:
    String name_;
    bool visible_ = true;
    float opacity_ = 1.0f;
    NodePtr root_;
    std::vector<LightPtr> lights_;
};

using ScenePtr = intrusive_ptr<Scene>;

V_GRAPHICS_NS_END
