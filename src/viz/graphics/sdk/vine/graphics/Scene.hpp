#pragma once
#include "graphics_global.hpp"

#include <vine/Object.hpp>
#include <vine/RefCounted.hpp>
#include <vine/intrusive_ptr.hpp>
#include <vine/String.hpp>
#include <vector>

#include "BoundingBox.hpp"
#include "Node.hpp"

V_GRAPHICS_NS_BEGIN

class Camera;
struct RenderCommand;

/**
 * @brief Scene graph container managing root nodes.
 *
 * Holds the root nodes of the scene hierarchy and provides tree queries:
 * bounding box computation, name-based search, and render command collection.
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

    /** @brief Adds a root-level node.
     *
     * @param node Node to add. Ownership is retained by the caller.
     */
    void addNode(Node* node);

    /** @brief Removes a root-level node.
     *
     * @param node Node to remove.
     */
    void removeNode(Node* node);

    /** @brief Gets all root-level nodes. */
    std::vector<NodePtr> nodes() const;

    /** @brief Finds a node by name (recursive search).
     *
     * @param name Name to search for.
     * @return Found node, or null.
     */
    NodePtr findNode(const String& name) const;

    /** @brief Removes all nodes. */
    void clear();

    /** @brief Computes the bounding box of the whole scene. */
    BoundingBox boundingBox() const;

    /** @brief Collects render commands for the given camera.
     *
     * @param camera Camera used for culling/ordering.
     * @return Collected render commands.
     */
    std::vector<RenderCommand> collectRenderCommands(const Camera* camera) const;

  private:
    String name_;
    bool visible_ = true;
    float opacity_ = 1.0f;
    std::vector<NodePtr> nodes_;
};

using ScenePtr = intrusive_ptr<Scene>;

V_GRAPHICS_NS_END
