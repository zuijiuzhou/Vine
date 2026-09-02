#include <vine/graphics/Node.hpp>

#include <vine/graphics/RenderCommand.hpp>
#include <vine/math/Transform3.hpp>

#include <algorithm>

V_GRAPHICS_NS_BEGIN

using vine::math::Vec3d;

V_OBJECT_META_IMPL(Node, vine::Object);

namespace
{

/**
 * @brief Transforms a bounding box by a matrix, returning the world AABB.
 *
 * @param box   Local-space box.
 * @param world World transform.
 * @return World-space AABB.
 */
Aabbd transformBox(const Aabbd& box, const Mat4d& world)
{
    Aabbd result = Aabbd::empty();
    const auto mn = box.min();
    const auto mx = box.max();
    const vine::math::Point3d corners[8] = {
        mn,
        vine::math::Point3d(mx.x, mn.y, mn.z),
        vine::math::Point3d(mn.x, mx.y, mn.z),
        vine::math::Point3d(mx.x, mx.y, mn.z),
        vine::math::Point3d(mn.x, mn.y, mx.z),
        vine::math::Point3d(mx.x, mn.y, mx.z),
        vine::math::Point3d(mn.x, mx.y, mx.z),
        mx,
    };
    for (const auto& c : corners) {
        const auto p = world * c;
        result.expandBy(Vec3d(p.x, p.y, p.z));
    }
    return result;
}

}  // namespace

Node::Node() = default;

Node::~Node() = default;

String Node::name() const
{
    return name_;
}

void Node::setName(const String& name)
{
    name_ = name;
}

bool Node::isVisible() const
{
    return visible_;
}

void Node::setVisible(bool visible)
{
    visible_ = visible;
}

float Node::opacity() const
{
    return opacity_;
}

void Node::setOpacity(float opacity)
{
    opacity_ = opacity;
}

Mat4d Node::localTransform() const
{
    return local_;
}

void Node::setLocalTransform(const Mat4d& transform)
{
    local_ = transform;
}

Mat4d Node::worldTransform() const
{
    if (parent_ != nullptr) {
        return parent_->worldTransform() * local_;
    }
    return local_;
}

Node* Node::parent() const
{
    return parent_;
}

void Node::addChild(Node* child)
{
    if (child == nullptr || child == this) {
        return;
    }
    // Detach from an existing parent.
    if (child->parent_ != nullptr) {
        child->parent_->removeChild(child);
    }
    child->parent_ = this;
    children_.emplace_back(child);
}

void Node::removeChild(Node* child)
{
    if (child == nullptr) {
        return;
    }
    auto it = std::find_if(children_.begin(), children_.end(),
                           [child](const NodePtr& ptr) { return ptr.get() == child; });
    if (it != children_.end()) {
        child->parent_ = nullptr;
        children_.erase(it);
    }
}

std::vector<NodePtr> Node::children() const
{
    return children_;
}

void Node::addDrawable(Drawable* drawable)
{
    if (drawable == nullptr) {
        return;
    }
    drawables_.emplace_back(drawable);
}

void Node::removeDrawable(Drawable* drawable)
{
    if (drawable == nullptr) {
        return;
    }
    auto it = std::find_if(drawables_.begin(), drawables_.end(),
                           [drawable](const DrawablePtr& ptr) { return ptr.get() == drawable; });
    if (it != drawables_.end()) {
        drawables_.erase(it);
    }
}

std::vector<DrawablePtr> Node::drawables() const
{
    return drawables_;
}

Aabbd Node::boundingBox() const
{
    Aabbd box = Aabbd::empty();
    const Mat4d world = worldTransform();
    for (const auto& drawable : drawables_) {
        box.expandBy(transformBox(drawable->boundingBox(), world));
    }
    for (const auto& child : children_) {
        const Aabbd child_box = child->boundingBox();
        if (child_box.isValid()) {
            box.expandBy(child_box);
        }
    }
    return box;
}

V_GRAPHICS_NS_END
