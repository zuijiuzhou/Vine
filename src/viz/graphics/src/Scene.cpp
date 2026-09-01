#include <vine/graphics/Scene.hpp>

#include <vine/graphics/Drawable.hpp>
#include <vine/graphics/RenderCommand.hpp>

V_GRAPHICS_NS_BEGIN

V_OBJECT_META_IMPL(Scene, vine::Object);

struct Scene::Data {
    String name;
    std::vector<NodePtr> nodes;
};

namespace
{

/**
 * @brief Recursively finds a node by name.
 *
 * @param node Root node to search.
 * @param name Name to find.
 * @return Matching node, or null.
 */
NodePtr findNodeRecursive(const Node* node, const String& name)
{
    if (node == nullptr) {
        return NodePtr();
    }
    if (node->name() == name) {
        return NodePtr(const_cast<Node*>(node));
    }
    for (const auto& child : node->children()) {
        NodePtr found = findNodeRecursive(child.get(), name);
        if (found != nullptr) {
            return found;
        }
    }
    return NodePtr();
}

/**
 * @brief Recursively collects render commands from a node subtree.
 *
 * @param node Root node to traverse.
 * @param out  Output command list.
 */
void collectNodeCommands(const Node* node, std::vector<RenderCommand>& out)
{
    if (node == nullptr || !node->isVisible()) {
        return;
    }
    for (const auto& drawable : node->drawables()) {
        out.emplace_back(drawable.get(), drawable->material(), node->worldTransform());
    }
    for (const auto& child : node->children()) {
        collectNodeCommands(child.get(), out);
    }
}

}  // namespace

Scene::Scene()
  : d(new Data())
{}

Scene::~Scene()
{
    delete d;
}

String Scene::name() const
{
    return d->name;
}

void Scene::setName(const String& name)
{
    d->name = name;
}

void Scene::addNode(Node* node)
{
    if (node == nullptr) {
        return;
    }
    d->nodes.emplace_back(node);
}

void Scene::removeNode(Node* node)
{
    if (node == nullptr) {
        return;
    }
    auto it = std::find_if(d->nodes.begin(), d->nodes.end(),
                           [node](const NodePtr& ptr) { return ptr.get() == node; });
    if (it != d->nodes.end()) {
        d->nodes.erase(it);
    }
}

std::vector<NodePtr> Scene::nodes() const
{
    return d->nodes;
}

NodePtr Scene::findNode(const String& name) const
{
    for (const auto& node : d->nodes) {
        NodePtr found = findNodeRecursive(node.get(), name);
        if (found != nullptr) {
            return found;
        }
    }
    return NodePtr();
}

void Scene::clear()
{
    d->nodes.clear();
}

BoundingBox Scene::boundingBox() const
{
    BoundingBox box;
    for (const auto& node : d->nodes) {
        if (!node->isVisible()) {
            continue;
        }
        const BoundingBox node_box = node->boundingBox();
        if (node_box.isValid()) {
            box.expand(node_box);
        }
    }
    return box;
}

std::vector<RenderCommand> Scene::collectRenderCommands(const Camera* camera) const
{
    std::vector<RenderCommand> commands;
    for (const auto& node : d->nodes) {
        collectNodeCommands(node.get(), commands);
    }
    return commands;
}

V_GRAPHICS_NS_END
