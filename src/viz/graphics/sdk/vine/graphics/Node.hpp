#pragma once
#include "graphics_global.hpp"

#include <vine/intrusive_ptr.hpp>
#include <vine/Object.hpp>
#include <vine/RefCounted.hpp>
#include <vine/String.hpp>
#include <vine/math/Matrix4x4.hpp>
#include <vine/math/Rect3.hpp>
#include <vector>

#include "Drawable.hpp"

V_GRAPHICS_NS_BEGIN

using vine::math::Mat4d;
using vine::math::Aabbd;

/**
 * @brief Scene graph node carrying a transform, children, and an optional drawable.
 *
 * Node is the building block of the scene hierarchy, similar to osg::Node or
 * OgreNext's SceneNode. It holds a local transform, parent/child links, and an
 * optional Drawable (the pure renderable). World transforms cascade through the
 * parent chain.
 */
class V_GRAPHICS_API Node : public Object, public RefCounted<Node> {
    V_OBJECT_META_DECL;

  public:
    Node();
    ~Node();

  public:
    /** @brief Gets the node name. */
    String name() const;

    /** @brief Sets the node name. */
    void setName(const String& name);

    /** @brief Returns whether this node (and its subtree) is visible. */
    bool isVisible() const;

    /** @brief Sets whether this node (and its subtree) is visible. */
    void setVisible(bool visible);

    /** @brief Gets the node opacity multiplier in [0, 1], applied to the
     * whole subtree. */
    float opacity() const;

    /** @brief Sets the node opacity multiplier in [0, 1], applied to the
     * whole subtree. */
    void setOpacity(float opacity);

    /** @brief Gets the local transform. */
    Mat4d localTransform() const;

    /** @brief Sets the local transform.
     *
     * @param transform Local-space transform.
     */
    void setLocalTransform(const Mat4d& transform);

    /** @brief Gets the world transform (parent chain applied). */
    Mat4d worldTransform() const;

    /** @brief Gets the parent node (null for root nodes). */
    Node* parent() const;

    /** @brief Adds a child node.
     *
     * @param child Child to add. Ownership is retained by this node.
     */
    void addChild(Node* child);

    /** @brief Removes a child node.
     *
     * @param child Child to remove.
     */
    void removeChild(Node* child);

    /** @brief Gets all child nodes. */
    std::vector<intrusive_ptr<Node>> children() const;

    /** @brief Adds a drawable to this node.
     *
     * A node can hold multiple drawables, like osg::Geode.
     *
     * @param drawable Drawable to attach.
     */
    void addDrawable(Drawable* drawable);

    /** @brief Removes a drawable from this node.
     *
     * @param drawable Drawable to detach.
     */
    void removeDrawable(Drawable* drawable);

    /** @brief Gets all drawables attached to this node. */
    std::vector<DrawablePtr> drawables() const;

    /** @brief Computes the world-space bounding box of this subtree. */
    Aabbd boundingBox() const;

  private:
    String name_;
    bool visible_ = true;
    float opacity_ = 1.0f;
    Mat4d local_;
    Node* parent_ = nullptr;
    std::vector<intrusive_ptr<Node>> children_;
    std::vector<DrawablePtr> drawables_;
};

using NodePtr = intrusive_ptr<Node>;

V_GRAPHICS_NS_END
