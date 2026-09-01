#include <vine/graphics/Node.hpp>

#include <vine/graphics/RenderCommand.hpp>
#include <vine/math/Transform3.hpp>

#include <algorithm>

V_GRAPHICS_NS_BEGIN

using vine::math::Vec3d;

V_OBJECT_META_IMPL(Node, vine::Object);

struct Node::Data {
    String name;
    bool visible = true;
    Mat4d local;
    Node* parent = nullptr;
    std::vector<NodePtr> children;
    std::vector<DrawablePtr> drawables;
};

namespace
{

/**
 * @brief Transforms a bounding box by a matrix, returning the world AABB.
 *
 * @param box   Local-space box.
 * @param world World transform.
 * @return World-space AABB.
 */
BoundingBox transformBox(const BoundingBox& box, const Mat4d& world)
{
    BoundingBox result;
    const Vec3d corners[8] = {
        box.min,
        { box.max.x, box.min.y, box.min.z },
        { box.min.x, box.max.y, box.min.z },
        { box.max.x, box.max.y, box.min.z },
        { box.min.x, box.min.y, box.max.z },
        { box.max.x, box.min.y, box.max.z },
        { box.min.x, box.max.y, box.max.z },
        box.max,
    };
    for (const auto& c : corners) {
        const auto p = world * vine::math::Point3d(c.x, c.y, c.z);
        result.expand(Vec3d(p.x, p.y, p.z));
    }
    return result;
}

}  // namespace

Node::Node()
  : d(new Data())
{}

Node::~Node()
{
    delete d;
}

String Node::name() const
{
    return d->name;
}

void Node::setName(const String& name)
{
    d->name = name;
}

bool Node::isVisible() const
{
    return d->visible;
}

void Node::setVisible(bool visible)
{
    d->visible = visible;
}

Mat4d Node::localTransform() const
{
    return d->local;
}

void Node::setLocalTransform(const Mat4d& transform)
{
    d->local = transform;
}

Mat4d Node::worldTransform() const
{
    if (d->parent != nullptr) {
        return d->parent->worldTransform() * d->local;
    }
    return d->local;
}

Node* Node::parent() const
{
    return d->parent;
}

void Node::addChild(Node* child)
{
    if (child == nullptr || child == this) {
        return;
    }
    // Detach from an existing parent.
    if (child->d->parent != nullptr) {
        child->d->parent->removeChild(child);
    }
    child->d->parent = this;
    d->children.emplace_back(child);
}

void Node::removeChild(Node* child)
{
    if (child == nullptr) {
        return;
    }
    auto it = std::find_if(d->children.begin(), d->children.end(),
                           [child](const NodePtr& ptr) { return ptr.get() == child; });
    if (it != d->children.end()) {
        child->d->parent = nullptr;
        d->children.erase(it);
    }
}

std::vector<NodePtr> Node::children() const
{
    return d->children;
}

void Node::addDrawable(Drawable* drawable)
{
    if (drawable == nullptr) {
        return;
    }
    d->drawables.emplace_back(drawable);
}

void Node::removeDrawable(Drawable* drawable)
{
    if (drawable == nullptr) {
        return;
    }
    auto it = std::find_if(d->drawables.begin(), d->drawables.end(),
                           [drawable](const DrawablePtr& ptr) { return ptr.get() == drawable; });
    if (it != d->drawables.end()) {
        d->drawables.erase(it);
    }
}

std::vector<DrawablePtr> Node::drawables() const
{
    return d->drawables;
}

BoundingBox Node::boundingBox() const
{
    BoundingBox box;
    const Mat4d world = worldTransform();
    for (const auto& drawable : d->drawables) {
        box.expand(transformBox(drawable->boundingBox(), world));
    }
    for (const auto& child : d->children) {
        const BoundingBox child_box = child->boundingBox();
        if (child_box.isValid()) {
            box.expand(child_box);
        }
    }
    return box;
}

V_GRAPHICS_NS_END
