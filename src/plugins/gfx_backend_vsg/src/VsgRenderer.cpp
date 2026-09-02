#include <vine/vsg/VsgRenderer.hpp>

#include <vine/graphics/Camera.hpp>
#include <vine/graphics/Drawable.hpp>
#include <vine/graphics/Material.hpp>
#include <vine/graphics/Node.hpp>
#include <vine/graphics/RenderCommand.hpp>
#include <vine/graphics/RenderPass.hpp>
#include <vine/graphics/Scene.hpp>

#include <vine/vsg/CameraBridge.hpp>
#include <vine/vsg/SceneBridge.hpp>
#include <vine/vsg/VsgMaterialManager.hpp>

#include "VsgUtils.hpp"

#include <vsg/commands/BindIndexBuffer.h>
#include <vsg/commands/BindVertexBuffers.h>
#include <vsg/commands/Commands.h>
#include <vsg/commands/DrawIndexed.h>
#include <vsg/app/CommandGraph.h>
#include <vsg/app/RenderGraph.h>
#include <vsg/app/View.h>
#include <vsg/app/Viewer.h>
#include <vsg/nodes/StateGroup.h>
#include <vsg/nodes/VertexIndexDraw.h>
#include <vsg/state/ColorBlendState.h>
#include <vsg/state/DepthStencilState.h>
#include <vsg/state/InputAssemblyState.h>
#include <vsg/state/MultisampleState.h>
#include <vsg/state/RasterizationState.h>
#include <vsg/state/material.h>
#include <vsg/state/ViewportState.h>
#include <vsg/lighting/AmbientLight.h>
#include <vsg/lighting/Light.h>
#include <vsg/utils/GraphicsPipelineConfigurator.h>
#include <vsg/utils/ShaderSet.h>

#ifdef _WIN32
#    ifndef NOMINMAX
#        define NOMINMAX
#    endif
#    include <windows.h>
#endif

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <map>

V_VSG_NS_BEGIN

namespace
{

/**
 * @brief Builds the canonical Phong shader set with complete pipeline states.
 *
 * This vsg build's createPhongShaderSet() arrives without default pipeline
 * states, so pipelines built by GraphicsPipelineConfigurator would lack a
 * ViewportState and nothing would rasterize (makeRawDemoNode() must spell out
 * the full state set for the same reason). Declare the canonical states here
 * so every SceneBridge-built geometry pipeline is complete. The baked viewport
 * matches the window size at attach; when the window drives a dynamic viewport
 * it is overridden at record time anyway.
 *
 * @param extent     Window extent for the baked static viewport.
 * @param depth_test When false, depth test/write are disabled so the geometry
 *                   always draws on top of previously rendered content (used
 *                   for HUD overlays such as the axis gizmo).
 * @return Configured shader set.
 */
::vsg::ref_ptr<::vsg::ShaderSet> buildShaderSet(const VkExtent2D& extent, bool depth_test)
{
    auto shaderSet = ::vsg::createPhongShaderSet();
    auto raster_state = ::vsg::RasterizationState::create();
    raster_state->cullMode = VK_CULL_MODE_NONE;  // tolerate either winding order
    auto depth_state = ::vsg::DepthStencilState::create();
    if (!depth_test) {
        depth_state->depthTestEnable = VK_FALSE;
        depth_state->depthWriteEnable = VK_FALSE;
    }
    shaderSet->defaultGraphicsPipelineStates = ::vsg::GraphicsPipelineStates{
        depth_state,
        raster_state,
        ::vsg::ColorBlendState::create(),
        ::vsg::InputAssemblyState::create(),
        ::vsg::MultisampleState::create(),
        ::vsg::ViewportState::create(extent),
    };
    return shaderSet;
}

/**
 * @brief Temporary test escape hatch: when VINE_VSG_OWN_WINDOW is set, the
 * backend creates its own independent vsg window instead of binding to the
 * Qt-hosted surface.
 *
 * Used to verify rendering end-to-end independent of the Qt child-window
 * compositing path (see design notes). Remove once the on-screen path is
 * decided.
 */
bool forceOwnWindow()
{
    const char* value = std::getenv("VINE_VSG_OWN_WINDOW");
    return value != nullptr && value[0] != '\0';
}

/**
 * @brief Builds a raw vsg red triangle, bypassing SceneBridge entirely.
 *
 * TEMP test helper for the independent-window path: builds the geometry and
 * its Phong pipeline directly with the canonical vsg API so rendering can be
 * validated without the Vine scene/node abstraction in between.
 *
 * @return A state group containing the triangle draw.
 */
::vsg::ref_ptr<::vsg::Node> makeRawDemoNode()
{
    auto shaderSet = ::vsg::createPhongShaderSet();
    // The phong shader set ships without default pipeline states in this vsg
    // build, and GraphicsPipelineConfigurator only fills DepthStencil /
    // Rasterization / ColorBlend / etc. — never a ViewportState. Explicitly
    // provide the full set so the pipeline has a viewport and correct state.
    shaderSet->defaultGraphicsPipelineStates = ::vsg::GraphicsPipelineStates{
        ::vsg::DepthStencilState::create(),
        ::vsg::RasterizationState::create(),
        ::vsg::ColorBlendState::create(),
        ::vsg::InputAssemblyState::create(),
        ::vsg::MultisampleState::create(),
        ::vsg::ViewportState::create(VkExtent2D{ 640, 360 }),
    };
    static bool s_dumped = false;
    if (!s_dumped) {
        s_dumped = true;
        FILE* f = std::fopen("raw_layout.txt", "w");
        if (f != nullptr) {
            std::fprintf(f, "[raw] attributeBindings:\n");
            for (const auto& ab : shaderSet->attributeBindings) {
                std::fprintf(f, "[raw]   attr name=%s loc=%u fmt=%d\n",
                             ab.name.c_str(), ab.location, static_cast<int>(ab.format));
            }
            std::fprintf(f, "[raw] descriptorBindings:\n");
            for (const auto& db : shaderSet->descriptorBindings) {
                std::fprintf(f, "[raw]   desc name=%s set=%u binding=%u type=%d count=%u\n",
                             db.name.c_str(), db.set, db.binding, static_cast<int>(db.descriptorType), db.descriptorCount);
            }
            std::fprintf(f, "[raw] defaultPipelineStates=%zu\n", shaderSet->defaultGraphicsPipelineStates.size());
            std::fclose(f);
        }
    }
    auto config = ::vsg::GraphicsPipelineConfigurator::create(shaderSet);

    auto vertices = ::vsg::vec3Array::create(3);
    (*vertices)[0] = ::vsg::vec3(-1.0f, -1.0f, 0.0f);
    (*vertices)[1] = ::vsg::vec3(1.0f, -1.0f, 0.0f);
    (*vertices)[2] = ::vsg::vec3(0.0f, 1.0f, 0.0f);
    auto normals = ::vsg::vec3Array::create(3);
    for (auto& normal : *normals) {
        normal = ::vsg::vec3(0.0f, 0.0f, 1.0f);
    }
    auto colors = ::vsg::vec4Array::create(3);
    for (auto& color : *colors) {
        color = ::vsg::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    auto indices = ::vsg::uintArray::create(3);
    (*indices)[0] = 0;
    (*indices)[1] = 1;
    (*indices)[2] = 2;

    ::vsg::DataList arrays;
    config->assignArray(arrays, "vsg_Vertex", VK_VERTEX_INPUT_RATE_VERTEX, vertices);
    config->assignArray(arrays, "vsg_Normal", VK_VERTEX_INPUT_RATE_VERTEX, normals);
    config->assignArray(arrays, "vsg_Color", VK_VERTEX_INPUT_RATE_VERTEX, colors);

    auto material = ::vsg::PhongMaterialValue::create();
    material->value().ambient = ::vsg::vec4(1.0f, 0.0f, 0.0f, 1.0f);  // red ambient test
    material->value().diffuse = ::vsg::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    material->value().specular = ::vsg::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    material->value().shininess = 32.0f;
    config->assignDescriptor("material", material);

    config->init();
    auto stateGroup = ::vsg::StateGroup::create();
    config->copyTo(stateGroup, {});

    // NOTE: manual geometry must use explicit bind/draw commands, NOT a
    // manually-assembled VertexIndexDraw, or nothing is rasterized (see
    // vsgExamples utils/vsggraphicspipelineconfigurator).
    auto drawCommands = ::vsg::Commands::create();
    drawCommands->addChild(::vsg::BindVertexBuffers::create(config->baseAttributeBinding, arrays));
    drawCommands->addChild(::vsg::BindIndexBuffer::create(indices));
    drawCommands->addChild(::vsg::DrawIndexed::create(3, 1, 0, 0, 0));
    stateGroup->addChild(drawCommands);
    return stateGroup;
}

/**
 * @brief Collects one render command per visible drawable, ignoring frustum culling.
 *
 * The per-frame path (Scene::collectRenderCommands) frustum-culls against the
 * camera, so content outside the frustum is not compiled until it becomes
 * visible. To make startup content visible from the very first frame we need
 * to populate the retained vsg graph before the viewer compiles, so this
 * walker gathers every visible drawable with its world transform, effective
 * opacity and material — mirroring Scene::collectRenderCommands but without
 * the camera test.
 *
 * @param scene  The Vine scene to walk.
 * @return One render command per visible drawable.
 */
std::vector<vine::graphics::RenderCommand> collectSceneCommandsNoCull(vine::graphics::Scene* scene)
{
    std::vector<vine::graphics::RenderCommand> commands;
    if (scene == nullptr || !scene->isVisible()) {
        return commands;
    }

    struct NodeWalker {
        /** @brief Walks one node and its subtree, accumulating render commands. */
        void operator()(const vine::graphics::Node* node, float opacity,
                        std::vector<vine::graphics::RenderCommand>& out) const
        {
            if (node == nullptr || !node->isVisible()) {
                return;
            }
            const float node_opacity = opacity * node->opacity();
            const auto world = node->worldTransform();
            for (const auto& drawable : node->drawables()) {
                if (!drawable->isVisible()) {
                    continue;
                }
                vine::graphics::Material* material = drawable->material();
                const float material_opacity = material != nullptr ? material->opacity() : 1.0f;
                const float effective = std::clamp(node_opacity * drawable->opacity() * material_opacity, 0.0f, 1.0f);
                auto& command = out.emplace_back(drawable.get(), material, world);
                command.opacity = effective;
                command.isTransparent = effective < (1.0f - 1e-6f);
            }
            for (const auto& child : node->children()) {
                (*this)(child.get(), node_opacity, out);
            }
        }
    };

    NodeWalker walk;
    for (const auto& node : scene->nodes()) {
        walk(node.get(), scene->opacity(), commands);
    }
    return commands;
}

/**
 * @brief vsg::Viewer whose pollEvents() does not pump the native message queue.
 *
 * vsg's Win32_Window::pollEvents() drains and dispatches the thread's Windows
 * message queue (PeekMessage/DispatchMessage). That is correct for a
 * standalone vsg application, but when vsg is embedded in a GUI toolkit such
 * as Qt — which owns the message loop — dispatching from inside a frame call
 * re-enters the toolkit: the dispatched message triggers a Qt event, which can
 * request another frame, which pumps again, recursing until the stack
 * overflows. Input is delivered by the host instead, so window polling is
 * disabled; only the buffered vsg events are dropped.
 */
class EmbeddedViewer : public ::vsg::Viewer {
  public:
    /** @brief Discards stale events without polling any attached window. */
    bool pollEvents(bool discardPreviousEvents) override
    {
        if (discardPreviousEvents) {
            this->getEvents().clear();
        }
        return false;
    }
};

}  // namespace

struct VsgRenderer::Data {
    vine::graphics::Scene* scene = nullptr;
    vine::graphics::Camera* camera = nullptr;
    SceneBridge sceneBridge;
    // Second bridge for overlay (HUD) content: its pipelines have depth
    // test/write disabled so overlays always draw on top of the main scene.
    SceneBridge overlayBridge;
    CameraBridge cameraBridge;
    VsgMaterialManager materialManager;
    void* bound_handle = nullptr;
    ::vsg::ref_ptr<::vsg::Window> window;
    ::vsg::ref_ptr<::vsg::Viewer> viewer;
    ::vsg::ref_ptr<::vsg::CommandGraph> command_graph;
    ::vsg::ref_ptr<::vsg::RenderGraph> render_graph;
    ::vsg::ref_ptr<::vsg::Camera> vsg_camera;
    ::vsg::ref_ptr<::vsg::Node> vsg_scene;
    vine::Color clear_color{ 51, 51, 51, 255 };
    bool clear_depth = true;
    bool initialized = false;

    /** @brief One retained overlay view (a second View of the main render graph). */
    struct OverlaySlot {
        vine::graphics::Camera* camera = nullptr;
        ::vsg::ref_ptr<::vsg::Camera> vsg_camera;
        ::vsg::ref_ptr<::vsg::Group> root;
        ::vsg::ref_ptr<::vsg::View> view;
        bool ready = false;
    };
    std::map<vine::graphics::Camera*, OverlaySlot> overlay_slots;

    // Sub-viewport queued by setViewport(), consumed by the next render().
    bool has_pending_viewport = false;
    int pending_viewport[4] = { 0, 0, 0, 0 };
    // Set when a frame was drawn; consumed by swapBuffers()/submitFrame().
    bool needs_submit = false;
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
    try {
    // Window. When a host native window is bound, attach to its surface (e.g.
    // a Qt QWindow) instead of creating a separate window.
    auto traits = ::vsg::WindowTraits::create();
    traits->windowTitle = "Vine";
    traits->width = 1280;
    traits->height = 720;
    traits->debugLayer = false;

    void* host_handle = d->bound_handle;
    if (forceOwnWindow()) {
        // Temporary test path: create vsg's own window, ignoring the Qt-hosted
        // surface handle, to verify rendering independent of Qt compositing.
        host_handle = nullptr;
    }
    if (host_handle != nullptr) {
#ifdef _WIN32
        traits->nativeWindow = reinterpret_cast<HWND>(host_handle);
        RECT client_rect{};
        if (::GetClientRect(reinterpret_cast<HWND>(host_handle), &client_rect)
            && client_rect.right > client_rect.left && client_rect.bottom > client_rect.top) {
            traits->width = client_rect.right - client_rect.left;
            traits->height = client_rect.bottom - client_rect.top;
        }
#else
        traits->nativeWindow = host_handle;
#endif
    }
    d->window = ::vsg::Window::create(traits);
    if (d->window == nullptr) {
        std::fprintf(stderr, "[VsgRenderer] Window::create FAILED (nativeWindow=%d, %ux%u)\n",
                     traits->nativeWindow.has_value() ? 1 : 0, traits->width, traits->height);
        shutdown();
        return false;
    }

    // Retained vsg root: SceneBridge fills it each frame from the render
    // command stream, so scene edits (move / recolor / add / remove) show up
    // without re-initializing the backend.
    auto root = ::vsg::Group::create();
    d->vsg_scene = root;

    // Camera bridge.
    d->vsg_camera = d->cameraBridge.create(d->camera);
    if (d->vsg_camera == nullptr) {
        std::fprintf(stderr, "[VsgRenderer] initialize: CameraBridge::create failed\n");
        shutdown();
        return false;
    }
    // The camera must carry a viewport state for the render graph to know the
    // render area; CameraBridge intentionally leaves it null (it has no window).
    d->vsg_camera->viewportState = ::vsg::ViewportState::create(d->window->extent2D());

    // Phong shader set (embedded SPIR-V, no runtime glslang required). Each
    // geometry gets its own pipeline + PhongMaterial descriptor built by
    // SceneBridge, so material properties propagate through to the GPU.
    auto shaderSet = buildShaderSet(d->window->extent2D(), true);
    d->sceneBridge.setShaderSet(shaderSet);
    d->sceneBridge.setMaterialManager(&d->materialManager);
    d->sceneBridge.clearCache();

    // Overlays (HUD) render on top of the main scene: give them a second
    // bridge whose pipelines disable depth test/write so the axis gizmo is
    // never hidden by scene geometry in front of it.
    d->overlayBridge.setShaderSet(buildShaderSet(d->window->extent2D(), false));
    d->overlayBridge.setMaterialManager(&d->materialManager);
    d->overlayBridge.clearCache();

    if (forceOwnWindow()) {
        // TEMP test: render a raw vsg triangle built directly, bypassing the
        // Vine scene/SceneBridge layer, to validate vsg rendering in the
        // independent window.
        root->addChild(makeRawDemoNode());
    } else {
        // Pre-populate the retained graph from the current scene so existing
        // content is compiled once here, before any frame runs. Compiling
        // freshly added geometry at runtime inside render() has proven
        // unreliable, so all content present at startup is built and compiled
        // now; runtime additions are still attempted later via
        // syncRenderCommands.
        const auto initial_commands = collectSceneCommandsNoCull(d->scene);
        std::vector<::vsg::ref_ptr<::vsg::Node>> created_at_init;
        d->sceneBridge.syncRenderCommands(initial_commands, root, &created_at_init);
    }

    // Viewer + render graph. assignHeadlight = true adds a headlight so the
    // Phong fragment shader has a light source to shade against. EmbeddedViewer
    // disables vsg's native message pumping (Qt owns the message loop here).
    d->viewer = ::vsg::ref_ptr<::vsg::Viewer>(new EmbeddedViewer());
    d->viewer->addWindow(d->window);

    auto renderGraph = ::vsg::createRenderGraphForView(d->window, d->vsg_camera, d->vsg_scene,
                                                       VK_SUBPASS_CONTENTS_INLINE, true);
    d->render_graph = renderGraph;
    auto commandGraph = ::vsg::CommandGraph::create(d->window);
    commandGraph->addChild(renderGraph);
    d->command_graph = commandGraph;
    d->viewer->assignRecordAndSubmitTaskAndPresentation(::vsg::CommandGraphs{ commandGraph });

    const auto compileResult = d->viewer->compile();
    if (!compileResult) {
        std::fprintf(stderr, "[VsgRenderer] compile failed: %s\n",
                     compileResult.message.c_str());
        shutdown();
        return false;
    }

    d->initialized = true;
    return true;
    } catch (const std::exception& e) {
        std::fprintf(stderr, "[VsgRenderer] initialize exception: %s\n", e.what());
    } catch (...) {
        std::fprintf(stderr, "[VsgRenderer] initialize unknown exception\n");
    }
    shutdown();
    return false;
}

void VsgRenderer::shutdown()
{
    if (d->viewer != nullptr) {
        d->viewer->deviceWaitIdle();
        // Detach the window from the viewer so its command graphs are dropped
        // before the viewer is released.
        if (d->window != nullptr) {
            d->viewer->removeWindow(d->window);
        }
        d->viewer->close();
        d->viewer = nullptr;
    }
    if (d->window != nullptr) {
        // Release the native handle the platform window wraps. When the
        // reference is dropped below, the Win32_Window destructor would call
        // ::DestroyWindow() (and ::UnregisterClass()) on the HOST's window —
        // here a Qt-owned HWND that Qt is itself tearing down. releaseWindow()
        // nulls the internal HWND so the destructor leaves Qt's window alone.
        d->window->releaseWindow();
        d->window = nullptr;
    }
    // Drop the retained vsg scene, render graph and the per-geometry / material
    // caches. Compiled pipelines and descriptor sets hold a reference to the
    // old vsg::Device; if they survive a surface-recreate re-init, the next
    // Window::create() allocates a second Device and trips vsg's
    // VSG_MAX_DEVICES limit (== 1 in this build) with an uncaught exception
    // (Device.cpp:63). Everything below must be released before a re-init can
    // create a fresh device.
    d->render_graph = nullptr;
    d->vsg_scene = nullptr;
    d->vsg_camera = nullptr;
    d->sceneBridge.clearCache();
    d->materialManager.clear();
    d->bound_handle = nullptr;
    d->initialized = false;
}

void VsgRenderer::beginFrame()
{
    if (d->viewer == nullptr) {
        return;
    }
    d->viewer->advanceToNextFrame();
    d->viewer->handleEvents();
}

void VsgRenderer::endFrame()
{
    if (d->viewer == nullptr) {
        return;
    }
    d->viewer->update();
}

void VsgRenderer::executePass(const vine::graphics::RenderPass* pass,
                              const std::vector<vine::graphics::RenderCommand>& commands)
{
    if (pass == nullptr) {
        return;
    }
    clear(pass->clearColor(), pass->shouldClearDepth());
    render(commands, pass->camera());
}

void VsgRenderer::setRenderTarget(vine::graphics::RenderTarget* target)
{
    // Only the default framebuffer is supported. Off-screen targets are a
    // future extension.
    (void)target;
}

void VsgRenderer::render(const std::vector<vine::graphics::RenderCommand>& commands,
                         const vine::graphics::Camera* camera)
{
    if (!d->initialized || d->viewer == nullptr) {
        return;
    }

    // Consume the sub-viewport queued by setViewport() just before this pass
    // (overlays); the main pass never sets one and renders the full surface.
    const bool has_vp = d->has_pending_viewport;
    const int vp_x = d->pending_viewport[0];
    const int vp_y = d->pending_viewport[1];
    const int vp_w = d->pending_viewport[2];
    const int vp_h = d->pending_viewport[3];
    d->has_pending_viewport = false;

    const bool is_main = (camera == nullptr || camera == d->camera);
    if (is_main) {
        if (camera != nullptr) {
            d->cameraBridge.apply(const_cast<vine::graphics::Camera*>(camera), d->vsg_camera);
        }
        // The command stream is the source of truth: reconcile the retained
        // vsg scene against it (in-place for moves/material edits).
        auto* root = d->vsg_scene.cast<::vsg::Group>().get();
        if (!forceOwnWindow()) {
            std::vector<::vsg::ref_ptr<::vsg::Node>> created;
            d->sceneBridge.syncRenderCommands(commands, root, &created);
            if (!created.empty()) {
                // A full-graph compile keeps newly built/rebuild subtrees
                // correct (see design doc §9 Phase 3 for the incremental plan).
                const auto compileResult = d->viewer->compile();
                if (!compileResult) {
                    std::fprintf(stderr, "[VsgRenderer] compile in render failed: %s\n",
                                 compileResult.message.c_str());
                }
            }
        }
    } else if (camera != nullptr) {
        renderOverlayPass(commands, camera, has_vp ? vp_x : 0, has_vp ? vp_y : 0,
                          has_vp ? vp_w : 0, has_vp ? vp_h : 0);
    }

    // Submission is deferred to swapBuffers() so one frame (main pass + all
    // overlay passes) is recorded and presented exactly once.
    d->needs_submit = true;
}

void VsgRenderer::renderOverlayPass(const std::vector<vine::graphics::RenderCommand>& commands,
                                    const vine::graphics::Camera* camera, int vp_x, int vp_y,
                                    int vp_w, int vp_h)
{
    auto* cam = const_cast<vine::graphics::Camera*>(camera);
    auto& slot = d->overlay_slots[cam];
    if (!slot.ready) {
        slot.camera = cam;
        slot.root = ::vsg::Group::create();
        slot.vsg_camera = d->cameraBridge.create(cam);
        if (slot.vsg_camera == nullptr) {
            d->overlay_slots.erase(cam);
            return;
        }
        // Add the overlay as a SECOND View of the SAME render graph as the
        // main scene (the canonical vsg multi-viewport pattern). An earlier
        // approach used a separate RenderGraph per overlay: its clear showed
        // (the grey backdrop box) but the view content never rasterized, so
        // overlay geometry is now drawn in the same render pass as the main
        // scene; the overlay camera's viewportState clips it to the sub-rect.
        auto view = ::vsg::View::create(slot.vsg_camera);
        // Overlay (HUD) content is lit by a single ambient light: with a
        // directional headlight the axis went dark/black from diagonal views
        // because the light direction is fixed while the overlay camera (which
        // mirrors the source) rotates. Ambient-only lighting makes phong's
        // colour independent of surface orientation, giving flat sticks.
        auto ambient = ::vsg::AmbientLight::create();
        ambient->name = "overlay_ambient";
        ambient->color.set(1.0f, 1.0f, 1.0f);
        ambient->intensity = 1.0f;
        view->addChild(ambient);
        view->addChild(slot.root);
        slot.view = view;
        if (d->render_graph != nullptr) {
            d->render_graph->addChild(view);
        }
        slot.ready = true;
        // A newly added View must be compiled before it is recorded.
        d->viewer->compile();
    }

    // Zero-size viewport means the full window.
    if (vp_w <= 0 || vp_h <= 0) {
        const auto extent = d->window->extent2D();
        vp_w = static_cast<int>(extent.width);
        vp_h = static_cast<int>(extent.height);
    }
    slot.vsg_camera->viewportState =
        ::vsg::ViewportState::create(vp_x, vp_y, static_cast<uint32_t>(vp_w), static_cast<uint32_t>(vp_h));

    d->cameraBridge.apply(cam, slot.vsg_camera);

    std::vector<::vsg::ref_ptr<::vsg::Node>> created;
    d->overlayBridge.syncRenderCommands(commands, slot.root.get(), &created);
    if (!created.empty()) {
        const auto compileResult = d->viewer->compile();
        if (!compileResult) {
            std::fprintf(stderr, "[VsgRenderer] overlay compile failed: %s\n",
                         compileResult.message.c_str());
        }
    }
}

void VsgRenderer::setViewport(int x, int y, int width, int height)
{
    d->pending_viewport[0] = x;
    d->pending_viewport[1] = y;
    d->pending_viewport[2] = width;
    d->pending_viewport[3] = height;
    d->has_pending_viewport = true;
}

void VsgRenderer::submitFrame()
{
    if (!d->initialized || d->viewer == nullptr || !d->needs_submit) {
        return;
    }
    d->needs_submit = false;
    d->viewer->recordAndSubmit();
    d->viewer->present();
}

void VsgRenderer::clear(const vine::Color& backgroundColor, bool clearDepth)
{
    d->clear_color = backgroundColor;
    d->clear_depth = clearDepth;
    if (d->render_graph != nullptr) {
        // The vsg render graph captured the window's clear color when it was
        // created; push the requested color through so the pass clear state
        // actually reaches the GPU clear. The color components are floats in
        // [0,1]; clear_color is stored as 0-255 bytes.
        const VkClearColorValue clear_value{
            { backgroundColor.r / 255.0f, backgroundColor.g / 255.0f, backgroundColor.b / 255.0f, backgroundColor.a / 255.0f }
        };
        d->render_graph->setClearValues(clear_value, VkClearDepthStencilValue{ 0.0f, 0 });
    }
}

void VsgRenderer::swapBuffers()
{
    // One record+submit+present per frame, after all passes were synced.
    submitFrame();
}

vine::graphics::MaterialManager* VsgRenderer::materialManager()
{
    return &d->materialManager;
}

void VsgRenderer::setWindowHandle(void* native_handle)
{
    d->bound_handle = native_handle;
}

void VsgRenderer::resize(int width, int height)
{
    (void)width;
    (void)height;
    if (d->window != nullptr) {
        d->window->resize();
    }
    // The RenderGraph derives its render area from the camera's viewport state
    // each frame; keep it at the live window size or content would keep
    // rendering into the original region after a window resize.
    if (d->vsg_camera != nullptr && d->window != nullptr) {
        d->vsg_camera->viewportState = ::vsg::ViewportState::create(d->window->extent2D());
    }
}

void* VsgRenderer::nativeHandle() const
{
    return d->bound_handle;
}

void VsgRenderer::frame()
{
    if (!d->initialized || d->viewer == nullptr) {
        return;
    }
    // VSG frame order: advance -> handleEvents -> update -> record -> present.
    beginFrame();
    endFrame();
    render({}, d->camera);
    swapBuffers();
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
