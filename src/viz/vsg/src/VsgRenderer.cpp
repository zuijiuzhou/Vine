#include <vine/vsg/VsgRenderer.hpp>

#include <vine/graphics/Camera.hpp>
#include <vine/graphics/Scene.hpp>

#include <vine/vsg/CameraBridge.hpp>
#include <vine/vsg/SceneBridge.hpp>

#include "Shaders.hpp"
#include "VsgUtils.hpp"

#include <vsg/app/CommandGraph.h>
#include <functional>
#include <vsg/nodes/Geometry.h>
#include <vsg/nodes/VertexIndexDraw.h>
#include <vsg/app/RenderGraph.h>
#include <vsg/nodes/StateGroup.h>
#include <vsg/state/ArrayState.h>
#include <vsg/state/ColorBlendState.h>
#include <vsg/state/DepthStencilState.h>
#include <vsg/state/GraphicsPipeline.h>
#include <vsg/utils/GraphicsPipelineConfigurator.h>
#include <vsg/utils/ShaderSet.h>
#include <vsg/state/InputAssemblyState.h>
#include <vsg/state/MultisampleState.h>
#include <vsg/state/PipelineLayout.h>
#include <vsg/state/RasterizationState.h>
#include <cstdio>
#include <vsg/state/ShaderStage.h>
#include <vsg/state/VertexInputState.h>
#include <vsg/state/ViewportState.h>

V_VSG_NS_BEGIN

namespace
{

/**
 * @brief Converts embedded SPIR-V words to a ShaderModule::SPIRV vector.
 *
 * @tparam N Array size.
 * @param words Embedded words.
 * @return SPIR-V words vector.
 */
template <std::size_t N>
::vsg::ShaderModule::SPIRV toSpirv(const uint32_t (&words)[N])
{
    return ::vsg::ShaderModule::SPIRV(std::begin(words), std::end(words));
}

/**
 * @brief Recursively copies pipeline state into every StateGroup.
 *
 * @param node  Root node to traverse.
 * @param config Pipeline configurator to copy into each StateGroup.
 */
void applyPipelineToStateGroups(::vsg::Node* node,
                                ::vsg::ref_ptr<::vsg::GraphicsPipelineConfigurator> config)
{
    if (node == nullptr) {
        return;
    }
    if (auto* stateGroup = node->cast<::vsg::StateGroup>()) {
        config->copyTo(::vsg::ref_ptr<::vsg::StateGroup>(stateGroup));
    }
    if (auto* group = node->cast<::vsg::Group>()) {
        for (const auto& child : group->children) {
            applyPipelineToStateGroups(child.get(), config);
        }
    }
}

}  // namespace

struct VsgRenderer::Data {
    vine::graphics::Scene* scene = nullptr;
    vine::graphics::Camera* camera = nullptr;
    SceneBridge sceneBridge;
    CameraBridge cameraBridge;
    ::vsg::ref_ptr<::vsg::Window> window;
    ::vsg::ref_ptr<::vsg::Viewer> viewer;
    ::vsg::ref_ptr<::vsg::Camera> vsg_camera;
    ::vsg::ref_ptr<::vsg::Node> vsg_scene;
};

VsgRenderer::VsgRenderer(vine::graphics::Scene* scene, vine::graphics::Camera* camera)
  : d(new Data())
{
    d->scene = scene;
    d->camera = camera;
}

VsgRenderer::~VsgRenderer()
{
    shutdown();
    delete d;
}

bool VsgRenderer::initialize()
{
    if (d->scene == nullptr || d->camera == nullptr) {
        return false;
    }

    // Window.
    auto traits = ::vsg::WindowTraits::create();
    traits->windowTitle = "Vine";
    traits->width = 1280;
    traits->height = 720;
    traits->debugLayer = true;
    d->window = ::vsg::Window::create(traits);
    if (d->window == nullptr) {
        return false;
    }

    // Scene and camera bridges.
    d->vsg_scene = d->sceneBridge.build(d->scene);
    d->vsg_camera = d->cameraBridge.create(d->camera);
    if (d->vsg_scene == nullptr || d->vsg_camera == nullptr) {
        return false;
    }

    // Flat-shader graphics pipeline via ShaderSet + GraphicsPipelineConfigurator.
    auto vertexModule = ::vsg::ShaderModule::create(toSpirv(s_flat_vert_spv));
    auto fragmentModule = ::vsg::ShaderModule::create(toSpirv(s_flat_frag_spv));
    std::fprintf(stderr, "[VsgRenderer] vertex shader words=%zu frag words=%zu\n",
                 vertexModule->code.size(), fragmentModule->code.size());
    auto vertexShader = ::vsg::ShaderStage::create(VK_SHADER_STAGE_VERTEX_BIT, "main", vertexModule);
    auto fragmentShader = ::vsg::ShaderStage::create(VK_SHADER_STAGE_FRAGMENT_BIT, "main", fragmentModule);

    auto shaderSet = ::vsg::ShaderSet::create();
    shaderSet->stages = ::vsg::ShaderStages{ vertexShader, fragmentShader };
    shaderSet->attributeBindings.push_back(::vsg::AttributeBinding{
        "vsg_Vertex", "", 0, VK_FORMAT_R32G32B32_SFLOAT,
        ::vsg::CoordinateSpace::NO_PREFERENCE, {} });
    shaderSet->pushConstantRanges.push_back(::vsg::PushConstantRange{
        "PushConstants", "", VkPushConstantRange{ VK_SHADER_STAGE_VERTEX_BIT, 0, 128 } });
    // Provide an ArrayState so VertexIndexDraw can bind its vertex arrays.
    auto arrayState = ::vsg::ArrayState::create();
    arrayState->vertex_attribute_location = 0;
    arrayState->vertexAttribute.binding = 0;
    arrayState->vertexAttribute.offset = 0;
    arrayState->vertexAttribute.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    shaderSet->definesArrayStates.push_back({ {}, arrayState });

    shaderSet->defaultGraphicsPipelineStates = ::vsg::GraphicsPipelineStates{
        ::vsg::DepthStencilState::create(),
        ::vsg::RasterizationState::create(),
        ::vsg::ColorBlendState::create(),
        ::vsg::InputAssemblyState::create(),
        ::vsg::MultisampleState::create(),
        ::vsg::ViewportState::create(d->window->extent2D()),
    };

    auto pipelineConfig = ::vsg::GraphicsPipelineConfigurator::create(shaderSet);
    // Match the module hints to the configurator's hints so the precompiled
    // SPIR-V modules are reused directly (otherwise getShaderStages would try
    // to recompile from an empty source).
    for (const auto& stage : shaderSet->stages) {
        if (stage->module != nullptr) {
            stage->module->hints = pipelineConfig->shaderHints;
        }
    }
    pipelineConfig->enableArray("vsg_Vertex", VK_VERTEX_INPUT_RATE_VERTEX, 12,
                                VK_FORMAT_R32G32B32_SFLOAT);
    pipelineConfig->init();

    // Copy the pipeline state into every StateGroup in the scene.
    applyPipelineToStateGroups(d->vsg_scene, pipelineConfig);

    // Viewer + render graph.
    d->viewer = ::vsg::Viewer::create();
    d->viewer->addWindow(d->window);

    auto renderGraph = ::vsg::createRenderGraphForView(d->window, d->vsg_camera, d->vsg_scene,
                                                       VK_SUBPASS_CONTENTS_INLINE, false);
    auto commandGraph = ::vsg::CommandGraph::create(d->window);
    commandGraph->addChild(renderGraph);
    d->viewer->assignRecordAndSubmitTaskAndPresentation(::vsg::CommandGraphs{ commandGraph });

    // Debug: dump the full command graph tree.
    std::function<void(::vsg::Node*, int)> dumpCG = [&](::vsg::Node* n, int depth) {
        if (n == nullptr) return;
        std::fprintf(stderr, "[VsgRenderer] %*sCG:%s\n", depth * 2, "", n->className());
        if (auto* g = n->cast<::vsg::Group>()) { for (auto& c : g->children) dumpCG(c.get(), depth + 1); }
    };
    dumpCG(commandGraph, 0);

    auto compileResult = d->viewer->compile();
    std::fprintf(stderr, "[VsgRenderer] compile result=%d msg=%s\n",
                 static_cast<int>(compileResult.result), compileResult.message.c_str());

    // Debug: manually recurse the scene tree counting VertexIndexDraw.
    struct VidCount { std::size_t count = 0; std::size_t arrays = 0; std::size_t indices = 0; };
    VidCount vidCount;
    std::function<void(::vsg::Node*)> recurse = [&](::vsg::Node* n) {
        if (n == nullptr) return;
        if (auto* geom = n->cast<::vsg::Geometry>()) {
            for (const auto& cmd : geom->commands) {
                if (auto* vid = cmd->cast<::vsg::VertexIndexDraw>()) {
                    ++vidCount.count; vidCount.arrays += vid->arrays.size(); if (vid->indices) ++vidCount.indices;
                    if (!vid->arrays.empty()) {
                        std::fprintf(stderr, "[VsgRenderer] vid array[0] buffer=%s\n",
                                     vid->arrays[0]->buffer ? "SET" : "null");
                    }
                }
            }
            return;
        }
        if (auto* g = n->cast<::vsg::Group>()) { for (auto& c : g->children) recurse(c.get()); }
    };
    recurse(d->vsg_scene);
    std::fprintf(stderr, "[VsgRenderer] vid count=%zu arrays=%zu indices=%zu\n",
                 vidCount.count, vidCount.arrays, vidCount.indices);
    std::fprintf(stderr, "[VsgRenderer] vine scene nodes=%zu vsg root=%s\n",
                 d->scene->nodes().size(), d->vsg_scene ? d->vsg_scene->className() : "null");
    std::function<void(::vsg::Node*, int)> dumpTree = [&](::vsg::Node* n, int depth) {
        if (n == nullptr) return;
        std::fprintf(stderr, "[VsgRenderer] %*s%s\n", depth * 2, "", n->className());
        if (auto* g = n->cast<::vsg::Group>()) { for (auto& c : g->children) dumpTree(c.get(), depth + 1); }
    };
    dumpTree(d->vsg_scene, 0);

    return true;
}

void VsgRenderer::update()
{
    if (d->scene == nullptr || d->camera == nullptr) {
        return;
    }
    // Re-translate scene and camera into the vsg structures.
    d->vsg_scene = d->sceneBridge.build(d->scene);
    d->cameraBridge.apply(d->camera, d->vsg_camera);
}

void VsgRenderer::frame()
{
    if (d->viewer == nullptr) {
        return;
    }
    if (d->viewer->advanceToNextFrame()) {
        d->viewer->handleEvents();
        d->viewer->update();
        d->viewer->recordAndSubmit();
        d->viewer->present();
    }
}

void VsgRenderer::shutdown()
{
    if (d->viewer != nullptr) {
        d->viewer->deviceWaitIdle();
        d->viewer->close();
        d->viewer = nullptr;
    }
    d->window = nullptr;
}

::vsg::ref_ptr<::vsg::Viewer> VsgRenderer::viewer() const
{
    return d->viewer;
}

::vsg::ref_ptr<::vsg::Camera> VsgRenderer::vsgCamera() const
{
    return d->vsg_camera;
}

::vsg::ref_ptr<::vsg::Node> VsgRenderer::vsgScene() const
{
    return d->vsg_scene;
}

V_VSG_NS_END
