#include <vine/vsg/VsgRenderer.hpp>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <map>

#include <vine/graphics/Camera.hpp>
#include <vine/graphics/Geometry.hpp>
#include <vine/graphics/Group.hpp>
#include <vine/graphics/Light.hpp>
#include <vine/graphics/Material.hpp>
#include <vine/graphics/Node.hpp>
#include <vine/graphics/RenderCommand.hpp>
#include <vine/graphics/RenderPass.hpp>
#include <vine/graphics/RenderTarget.hpp>
#include <vine/graphics/Scene.hpp>
#include <vine/vsg/CameraBridge.hpp>
#include <vine/vsg/SceneBridge.hpp>
#include <vine/vsg/VsgMaterialManager.hpp>
#include <vsg/app/CommandGraph.h>
#include <vsg/app/RenderGraph.h>
#include <vsg/app/View.h>
#include <vsg/app/Viewer.h>
#include <vsg/commands/BindIndexBuffer.h>
#include <vsg/commands/BindVertexBuffers.h>
#include <vsg/commands/Commands.h>
#include <vsg/commands/Draw.h>
#include <vsg/commands/DrawIndexed.h>
#include <vsg/lighting/AmbientLight.h>
#include <vsg/lighting/DirectionalLight.h>
#include <vsg/lighting/Light.h>
#include <vsg/nodes/StateGroup.h>
#include <vsg/nodes/VertexIndexDraw.h>
#include <vsg/state/ColorBlendState.h>
#include <vsg/state/DepthStencilState.h>
#include <vsg/state/DescriptorImage.h>
#include <vsg/state/ImageInfo.h>
#include <vsg/state/InputAssemblyState.h>
#include <vsg/state/MultisampleState.h>
#include <vsg/state/RasterizationState.h>
#include <vsg/state/Sampler.h>
#include <vsg/state/ShaderStage.h>
#include <vsg/state/ViewportState.h>
#include <vsg/state/ViewDependentState.h>
#include <vsg/state/material.h>
#include <vsg/state/Image.h>
#include <vsg/state/ImageView.h>
#include <vsg/utils/Builder.h>
#include <vsg/utils/GraphicsPipelineConfigurator.h>
#include <vsg/utils/ShaderCompiler.h>
#include <vsg/utils/ShaderSet.h>
#include <vsg/vk/Device.h>
#include <vsg/vk/Framebuffer.h>
#include <vsg/vk/RenderPass.h>

#ifdef _WIN32
#    ifndef NOMINMAX
#        define NOMINMAX
#    endif
#    include <windows.h>
#endif

V_VSG_NS_BEGIN

namespace
{

/**
 * @brief Builds the shader set for the given shading preset with complete
 * pipeline states.
 *
 * vsg's built-in shader sets (createPhongShaderSet / createFlatShadedShaderSet)
 * arrive without default pipeline states, so pipelines built by
 * GraphicsPipelineConfigurator would lack a ViewportState and nothing would
 * rasterize. Declare the canonical states here so every SceneBridge-built
 * geometry pipeline is complete. The baked viewport matches the window size at
 * attach; when the window drives a dynamic viewport it is overridden at record
 * time anyway. Both Phong and flat presets bind a "material" descriptor of
 * type vsg::PhongMaterialValue, so the shared Vine material path (SceneBridge
 * assigns that value) works unchanged for either.
 *
 * @param preset     Shading-model preset to build for.
 * @param extent     Window extent for the baked static viewport.
 * @param depth_test When false, depth test/write are disabled so the geometry
 *                   always draws on top of previously rendered content (used
 *                   for HUD overlays such as the axis gizmo).
 * @return Configured shader set.
 */
::vsg::ref_ptr<::vsg::ShaderSet> buildShaderSet(vine::graphics::ShaderPreset preset,
                                                const VkExtent2D& extent, bool depth_test)
{
    // Pbr / ShadowedPhong are reserved presets without a backend mapping yet
    // (Pbr needs its own PbrMaterialValue; shadow comes last in the roadmap),
    // so they fall back to the Phong shader set for now.
    ::vsg::ref_ptr<::vsg::ShaderSet> shaderSet =
        (preset == vine::graphics::ShaderPreset::FlatShaded) ? ::vsg::createFlatShadedShaderSet()
                                                             : ::vsg::createPhongShaderSet();
    auto raster_state      = ::vsg::RasterizationState::create();
    raster_state->cullMode = VK_CULL_MODE_NONE; // tolerate either winding order
    auto depth_state       = ::vsg::DepthStencilState::create();
    if (!depth_test) {
        depth_state->depthTestEnable  = VK_FALSE;
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
    auto shaderSet                           = ::vsg::createPhongShaderSet();
    // The phong shader set ships without default pipeline states in this vsg
    // build, and GraphicsPipelineConfigurator only fills DepthStencil /
    // Rasterization / ColorBlend / etc. — never a ViewportState. Explicitly
    // provide the full set so the pipeline has a viewport and correct state.
    shaderSet->defaultGraphicsPipelineStates = ::vsg::GraphicsPipelineStates{
        ::vsg::DepthStencilState::create(),  ::vsg::RasterizationState::create(), ::vsg::ColorBlendState::create(),
        ::vsg::InputAssemblyState::create(), ::vsg::MultisampleState::create(),   ::vsg::ViewportState::create(VkExtent2D{ 640, 360 }),
    };
    static bool s_dumped = false;
    if (!s_dumped) {
        s_dumped = true;
        FILE* f  = std::fopen("raw_layout.txt", "w");
        if (f != nullptr) {
            std::fprintf(f, "[raw] attributeBindings:\n");
            for (const auto& ab : shaderSet->attributeBindings) {
                std::fprintf(f, "[raw]   attr name=%s loc=%u fmt=%d\n", ab.name.c_str(), ab.location, static_cast<int>(ab.format));
            }
            std::fprintf(f, "[raw] descriptorBindings:\n");
            for (const auto& db : shaderSet->descriptorBindings) {
                std::fprintf(f,
                             "[raw]   desc name=%s set=%u binding=%u type=%d count=%u\n",
                             db.name.c_str(),
                             db.set,
                             db.binding,
                             static_cast<int>(db.descriptorType),
                             db.descriptorCount);
            }
            std::fprintf(f, "[raw] defaultPipelineStates=%zu\n", shaderSet->defaultGraphicsPipelineStates.size());
            std::fclose(f);
        }
    }
    auto config = ::vsg::GraphicsPipelineConfigurator::create(shaderSet);

    auto vertices  = ::vsg::vec3Array::create(3);
    (*vertices)[0] = ::vsg::vec3(-1.0f, -1.0f, 0.0f);
    (*vertices)[1] = ::vsg::vec3(1.0f, -1.0f, 0.0f);
    (*vertices)[2] = ::vsg::vec3(0.0f, 1.0f, 0.0f);
    auto normals   = ::vsg::vec3Array::create(3);
    for (auto& normal : *normals) {
        normal = ::vsg::vec3(0.0f, 0.0f, 1.0f);
    }
    auto colors = ::vsg::vec4Array::create(3);
    for (auto& color : *colors) {
        color = ::vsg::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    auto indices  = ::vsg::uintArray::create(3);
    (*indices)[0] = 0;
    (*indices)[1] = 1;
    (*indices)[2] = 2;

    ::vsg::DataList arrays;
    config->assignArray(arrays, "vsg_Vertex", VK_VERTEX_INPUT_RATE_VERTEX, vertices);
    config->assignArray(arrays, "vsg_Normal", VK_VERTEX_INPUT_RATE_VERTEX, normals);
    config->assignArray(arrays, "vsg_Color", VK_VERTEX_INPUT_RATE_VERTEX, colors);

    auto material               = ::vsg::PhongMaterialValue::create();
    material->value().ambient   = ::vsg::vec4(1.0f, 0.0f, 0.0f, 1.0f); // red ambient test
    material->value().diffuse   = ::vsg::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    material->value().specular  = ::vsg::vec4(0.2f, 0.2f, 0.2f, 1.0f);
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
        void operator()(const vine::graphics::Node* node, float opacity, std::vector<vine::graphics::RenderCommand>& out) const
        {
            if (node == nullptr || !node->isVisible()) {
                return;
            }
            const float node_opacity = opacity * node->opacity();
            if (const auto* geometry = dynamic_cast<const vine::graphics::Geometry*>(node)) {
                vine::graphics::Material* material = geometry->material();
                const float effective = std::clamp(node_opacity, 0.0f, 1.0f);
                auto& command = out.emplace_back(
                    vine::intrusive_ptr<vine::graphics::Geometry>(const_cast<vine::graphics::Geometry*>(geometry)),
                    vine::intrusive_ptr<vine::graphics::Material>(material), node->worldMatrix());
                command.opacity       = effective;
                command.isTransparent = effective < (1.0f - 1e-6f);
                return;
            }
            if (const auto* group = dynamic_cast<const vine::graphics::Group*>(node)) {
                for (const auto& child : group->children()) {
                    (*this)(child.get(), node_opacity, out);
                }
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

} // namespace

namespace
{

VkFormat toColorFormat(vine::graphics::RenderTarget::ColorFormat f)
{
    switch (f) {
        case vine::graphics::RenderTarget::ColorFormat::RGBA8: return VK_FORMAT_R8G8B8A8_UNORM;
        case vine::graphics::RenderTarget::ColorFormat::RGBA16F: return VK_FORMAT_R16G16B16A16_SFLOAT;
        case vine::graphics::RenderTarget::ColorFormat::RGBA32F: return VK_FORMAT_R32G32B32A32_SFLOAT;
    }
    return VK_FORMAT_R8G8B8A8_UNORM;
}

VkFormat toDepthFormat(vine::graphics::RenderTarget::DepthFormat f)
{
    switch (f) {
        case vine::graphics::RenderTarget::DepthFormat::D16: return VK_FORMAT_D16_UNORM;
        case vine::graphics::RenderTarget::DepthFormat::D24: return VK_FORMAT_D24_UNORM_S8_UINT;
        case vine::graphics::RenderTarget::DepthFormat::D32:
        case vine::graphics::RenderTarget::DepthFormat::D32F: return VK_FORMAT_D32_SFLOAT;
    }
    return VK_FORMAT_D32_SFLOAT;
}

/**
 * @brief Builds an off-screen render pass whose colour attachment ends in
 * SHADER_READ_ONLY_OPTIMAL so a later pass can sample it as a texture.
 *
 * vsg::createRenderPass() leaves the colour attachment in PRESENT_SRC_KHR
 * (correct for swapchain output, wrong for a texture sampled by a later
 * pass). The dependency on subpass-external fragment-shader reads makes the
 * colour writes visible to the sampling pass without an extra barrier.
 *
 * @param device      Device the render pass is created on.
 * @param color_format Colour attachment format.
 * @param depth_format Depth attachment format, or VK_FORMAT_UNDEFINED for a
 *                     colour-only pass.
 * @return The configured render pass.
 */
::vsg::ref_ptr<::vsg::RenderPass> makeSampleableRenderPass(::vsg::Device* device, VkFormat color_format, VkFormat depth_format)
{
    const bool has_depth = depth_format != VK_FORMAT_UNDEFINED;

    ::vsg::AttachmentDescription color = {};
    color.format          = color_format;
    color.samples         = VK_SAMPLE_COUNT_1_BIT;
    color.loadOp          = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color.storeOp         = VK_ATTACHMENT_STORE_OP_STORE;
    color.stencilLoadOp   = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color.stencilStoreOp  = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color.initialLayout   = VK_IMAGE_LAYOUT_UNDEFINED;
    color.finalLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    ::vsg::RenderPass::Attachments attachments{ color };

    ::vsg::AttachmentReference color_ref = {};
    color_ref.attachment = 0;
    color_ref.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    ::vsg::SubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachments.emplace_back(color_ref);

    if (has_depth) {
        ::vsg::AttachmentDescription depth = {};
        depth.format          = depth_format;
        depth.samples         = VK_SAMPLE_COUNT_1_BIT;
        depth.loadOp          = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth.storeOp         = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth.stencilLoadOp   = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depth.stencilStoreOp  = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth.initialLayout   = VK_IMAGE_LAYOUT_UNDEFINED;
        depth.finalLayout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        attachments.push_back(depth);

        ::vsg::AttachmentReference depth_ref = {};
        depth_ref.attachment = 1;
        depth_ref.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        subpass.depthStencilAttachments.emplace_back(depth_ref);
    }

    ::vsg::RenderPass::Dependencies dependencies;

    // Initial (UNDEFINED) -> COLOR_ATTACHMENT_OPTIMAL before the first subpass.
    ::vsg::SubpassDependency ext_to_sub = {};
    ext_to_sub.srcSubpass = VK_SUBPASS_EXTERNAL;
    ext_to_sub.dstSubpass = 0;
    ext_to_sub.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    ext_to_sub.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    ext_to_sub.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies.push_back(ext_to_sub);

    // After the subpass, transition to SHADER_READ_ONLY and make the colour
    // writes visible to a later pass that samples the attachment.
    ::vsg::SubpassDependency sub_to_ext = {};
    sub_to_ext.srcSubpass = 0;
    sub_to_ext.dstSubpass = VK_SUBPASS_EXTERNAL;
    sub_to_ext.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    sub_to_ext.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    sub_to_ext.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    sub_to_ext.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dependencies.push_back(sub_to_ext);

    return ::vsg::RenderPass::create(device, attachments, ::vsg::RenderPass::Subpasses{ subpass }, dependencies);
}

/**
 * @brief Builds a depth-only off-screen render pass (shadow maps).
 *
 * The depth attachment is stored and left in SHADER_READ_ONLY_OPTIMAL so a
 * later pass can sample it as a shadow map. The subpass-external fragment-read
 * dependency makes the depth writes visible to the sampling pass.
 *
 * @param device      Device the render pass is created on.
 * @param depth_format Depth attachment format.
 * @return The configured render pass.
 */
::vsg::ref_ptr<::vsg::RenderPass> makeDepthOnlyRenderPass(::vsg::Device* device, VkFormat depth_format)
{
    ::vsg::AttachmentDescription depth = {};
    depth.format           = depth_format;
    depth.samples          = VK_SAMPLE_COUNT_1_BIT;
    depth.loadOp           = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth.storeOp          = VK_ATTACHMENT_STORE_OP_STORE;  // sampled later as a shadow map
    depth.stencilLoadOp    = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth.stencilStoreOp   = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth.initialLayout    = VK_IMAGE_LAYOUT_UNDEFINED;
    depth.finalLayout      = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    ::vsg::RenderPass::Attachments attachments{ depth };

    ::vsg::AttachmentReference depth_ref = {};
    depth_ref.attachment = 0;
    depth_ref.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    ::vsg::SubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.depthStencilAttachments.emplace_back(depth_ref);

    ::vsg::RenderPass::Dependencies dependencies;
    // UNDEFINED -> DEPTH_STENCIL_ATTACHMENT before the subpass.
    ::vsg::SubpassDependency ext_to_sub = {};
    ext_to_sub.srcSubpass = VK_SUBPASS_EXTERNAL;
    ext_to_sub.dstSubpass = 0;
    ext_to_sub.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    ext_to_sub.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    ext_to_sub.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependencies.push_back(ext_to_sub);

    // After the subpass, transition to SHADER_READ_ONLY and make the depth
    // writes visible to a later pass that samples the shadow map.
    ::vsg::SubpassDependency sub_to_ext = {};
    sub_to_ext.srcSubpass = 0;
    sub_to_ext.dstSubpass = VK_SUBPASS_EXTERNAL;
    sub_to_ext.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    sub_to_ext.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    sub_to_ext.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    sub_to_ext.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dependencies.push_back(sub_to_ext);

    return ::vsg::RenderPass::create(device, attachments, ::vsg::RenderPass::Subpasses{ subpass }, dependencies);
}

/**
 * @brief Builds a state-group that draws a full-screen textured triangle
 * sampling @p image_view into the current target.
 *
 * The vertex shader generates the full-screen triangle from gl_VertexIndex
 * (no vertex buffers / camera matrices involved); the fragment shader samples
 * the passed image. Depth test/write are disabled so the textured triangle
 * composites over previously rendered content (used by the PiP screen pass).
 *
 * @param image_view Image to sample (the off-screen target's colour view).
 * @param extent     Surface extent for the baked static viewport.
 * @return The drawable state-group, or null when shader compilation failed.
 */
::vsg::ref_ptr<::vsg::Node> makeScreenTextureNode(::vsg::ref_ptr<::vsg::ImageView> image_view, const VkExtent2D& extent)
{
    const std::string vertex_source = R"(#version 450
layout(location = 0) out vec2 v_uv;
void main()
{
    v_uv = vec2(float((gl_VertexIndex << 1) & 2), float(gl_VertexIndex & 2));
    gl_Position = vec4(v_uv * 2.0 - 1.0, 0.0, 1.0);
}
)";
    const std::string fragment_source = R"(#version 450
layout(location = 0) in vec2 v_uv;
layout(location = 0) out vec4 out_color;
layout(binding = 0) uniform sampler2D screen_tex;
void main()
{
    out_color = texture(screen_tex, vec2(v_uv.x, 1.0 - v_uv.y));
}
)";

    auto vs = ::vsg::ShaderStage::create(VK_SHADER_STAGE_VERTEX_BIT, "main", vertex_source);
    auto fs = ::vsg::ShaderStage::create(VK_SHADER_STAGE_FRAGMENT_BIT, "main", fragment_source);

    auto compiler = ::vsg::ShaderCompiler::create();
    if (compiler == nullptr || !compiler->supported()) {
        std::fprintf(stderr, "[VsgRenderer] screen pass: shader compiler unavailable\n");
        return ::vsg::ref_ptr<::vsg::Node>();
    }
    if (!compiler->compile(vs) || !compiler->compile(fs)) {
        std::fprintf(stderr, "[VsgRenderer] screen pass: GLSL compilation failed\n");
        return ::vsg::ref_ptr<::vsg::Node>();
    }

    auto shaderSet = ::vsg::ShaderSet::create();
    shaderSet->stages = ::vsg::ShaderStages{ vs, fs };
    shaderSet->addDescriptorBinding(
        "screen_tex", "", 0, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, {});

    auto raster = ::vsg::RasterizationState::create();
    raster->cullMode = VK_CULL_MODE_NONE;
    auto depth_state = ::vsg::DepthStencilState::create();
    depth_state->depthTestEnable  = VK_FALSE;
    depth_state->depthWriteEnable = VK_FALSE;
    shaderSet->defaultGraphicsPipelineStates = ::vsg::GraphicsPipelineStates{
        depth_state,
        raster,
        ::vsg::ColorBlendState::create(),
        ::vsg::InputAssemblyState::create(),
        ::vsg::MultisampleState::create(),
        ::vsg::ViewportState::create(extent),
    };

    auto config = ::vsg::GraphicsPipelineConfigurator::create(shaderSet);
    auto sampler = ::vsg::Sampler::create();
    auto image_info = ::vsg::ImageInfo::create(sampler, image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    config->assignTexture("screen_tex", ::vsg::ImageInfoList{ image_info });
    config->init();

    auto stateGroup = ::vsg::StateGroup::create();
    config->copyTo(stateGroup, ::vsg::ref_ptr<::vsg::SharedObjects>());
    auto draw = ::vsg::Commands::create();
    draw->addChild(::vsg::Draw::create(3, 1, 0, 0));
    stateGroup->addChild(draw);
    return stateGroup;
}

/**
 * @brief Builds a vsg light node from a Vine light.
 *
 * vsg lights are scene nodes collected per view into the phong "lightData"
 * uniform; colour comes in float [0,1] (vine::Colorf) and intensity is a
 * multiplier. Disabled lights produce no node.
 *
 * @param light Vine light to translate.
 * @return The vsg light node, or null for a disabled / unsupported light.
 */
::vsg::ref_ptr<::vsg::Node> buildLightNode(const vine::graphics::Light* light)
{
    if (light == nullptr || !light->isEnabled()) {
        return ::vsg::ref_ptr<::vsg::Node>();
    }
    const auto c = light->color();
    switch (light->type()) {
        case vine::graphics::LightType::Ambient: {
            auto ambient = ::vsg::AmbientLight::create();
            ambient->color.set(c.r, c.g, c.b);
            ambient->intensity = light->intensity();
            return ambient;
        }
        case vine::graphics::LightType::Directional: {
            auto dir = ::vsg::DirectionalLight::create();
            dir->color.set(c.r, c.g, c.b);
            dir->intensity = light->intensity();
            const auto v = light->direction();
            dir->direction.set(v.x, v.y, v.z);
            // Shadow mapping is deferred until the custom-shader / multi-pass
            // slice is mature: a directional Vine light maps to a plain vsg
            // directional light for now. Light::castShadow() stays a reserved
            // semantic flag for that future slice and is not consumed here.
            return dir;
        }
        default:
            // Point/Spot are not implemented yet.
            return ::vsg::ref_ptr<::vsg::Node>();
    }
}

/**
 * @brief Replaces a view's light-group children with the given Vine lights.
 *
 * Light nodes carry no GPU resources (they are collected into the per-view
 * lightData uniform at record time), so replacing them each frame is cheap
 * and needs no recompile. An empty list leaves the group as-is so the view
 * keeps its default light(s).
 *
 * @param group  The view's light group.
 * @param lights Vine lights to attach.
 */
void setGroupLights(::vsg::Group* group, const std::vector<const vine::graphics::Light*>& lights)
{
    if (group == nullptr || lights.empty()) {
        return;
    }
    group->children.clear();
    for (const auto* light : lights) {
        auto node = buildLightNode(light);
        if (node != nullptr) {
            group->addChild(node);
        }
    }
}

} // namespace

struct VsgRenderer::Data {
    vine::graphics::Scene*  scene  = nullptr;
    vine::graphics::Camera* camera = nullptr;
    SceneBridge             sceneBridge;
    // Second bridge for overlay (HUD) content: its pipelines have depth
    // test/write disabled so overlays always draw on top of the main scene.
    SceneBridge                         overlayBridge;
    CameraBridge                        cameraBridge;
    VsgMaterialManager                  materialManager;
    vine::graphics::ShaderPreset        shader_preset{ vine::graphics::ShaderPreset::StandardPhong };
    void*                               bound_handle = nullptr;
    ::vsg::ref_ptr<::vsg::Window>       window;
    ::vsg::ref_ptr<::vsg::Viewer>       viewer;
    ::vsg::ref_ptr<::vsg::CommandGraph> command_graph;
    ::vsg::ref_ptr<::vsg::RenderGraph>  render_graph;
    ::vsg::ref_ptr<::vsg::Camera>       vsg_camera;
    ::vsg::ref_ptr<::vsg::Node>         vsg_scene;
    ::vsg::ref_ptr<::vsg::View>         main_view;        // main scene view (owned here)
    ::vsg::ref_ptr<::vsg::Group>        main_light_group; // lights under the main view
    vine::Color                         clear_color{ 51, 51, 51, 255 };
    bool                                clear_depth = true;
    bool                                initialized = false;

    /** @brief One retained overlay view (a second View of the main render graph). */
    struct OverlaySlot {
        vine::graphics::Camera*       camera = nullptr;
        ::vsg::ref_ptr<::vsg::Camera> vsg_camera;
        ::vsg::ref_ptr<::vsg::Group>  root;
        ::vsg::ref_ptr<::vsg::View>   view;
        bool                          ready = false;
    };

    std::map<vine::graphics::Camera*, OverlaySlot> overlay_slots;

    // Sub-viewport queued by setViewport(), consumed by the next render().
    bool has_pending_viewport = false;
    int  pending_viewport[4]  = { 0, 0, 0, 0 };
    // Lights queued by setLights(), consumed by the next render().
    std::vector<const vine::graphics::Light*> pending_lights;
    // Set when a frame was drawn; consumed by swapBuffers()/submitFrame().
    bool needs_submit         = false;

    // ---- Off-screen render-to-texture support (EXPERIMENTAL) ----

    /// Target queued by setRenderTarget(), consumed by the next render().
    vine::graphics::RenderTarget* active_target = nullptr;

    /** @brief One off-screen target's GPU attachments and render graph. */
    struct OffscreenTarget {
        ::vsg::ref_ptr<::vsg::Image>       color_image;
        ::vsg::ref_ptr<::vsg::ImageView>   color_view;
        ::vsg::ref_ptr<::vsg::Image>       depth_image;
        ::vsg::ref_ptr<::vsg::ImageView>   depth_view;
        ::vsg::ref_ptr<::vsg::RenderPass>  render_pass;
        ::vsg::ref_ptr<::vsg::Framebuffer> framebuffer;
        ::vsg::ref_ptr<::vsg::RenderGraph> graph;
        ::vsg::ref_ptr<::vsg::Camera>      vsg_camera;
        ::vsg::ref_ptr<::vsg::View>        view;        // the off-screen view (owned here)
        ::vsg::ref_ptr<::vsg::Group>       light_group; // lights under the off-screen view
        ::vsg::ref_ptr<::vsg::Group>       root;
        // Dedicated retained-scene bridge (own nodes/pipelines). Sharing the
        // main scene bridge would attach its already-compiled (main-view
        // viewID) pipelines to this target's view -> GraphicsPipeline::vk()
        // crash, since vsg compiles pipelines per viewID.
        SceneBridge                         bridge;
        int                                width = 0;
        int                                height = 0;
    };

    std::map<vine::graphics::RenderTarget*, OffscreenTarget> offscreen;

    // ---- Screen (fullscreen sample) pass support (EXPERIMENTAL) ----

    /** @brief One picture-in-picture view sampling an off-screen target. */
    struct ScreenSlot {
        ::vsg::ref_ptr<::vsg::Camera>      camera;      // carries the sub-rect viewport
        ::vsg::ref_ptr<::vsg::View>        view;        // second View of the main render graph
        ::vsg::ref_ptr<::vsg::ImageView>   source_view; // keeps the sampled attachment alive
        int                                source_w = 0;
        int                                source_h = 0;
        bool                               ready = false;
    };

    std::map<vine::graphics::RenderTarget*, ScreenSlot> screen_slots;
};

VsgRenderer::VsgRenderer(vine::raw_ptr<vine::graphics::Scene> scene, vine::raw_ptr<vine::graphics::Camera> camera)
  : d(new Data())
{
    d->scene  = scene;
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
    // try {
        // Window. When a host native window is bound, attach to its surface (e.g.
        // a Qt QWindow) instead of creating a separate window.
        auto traits         = ::vsg::WindowTraits::create();
        traits->windowTitle = "Vine";
        traits->width       = 1280;
        traits->height      = 720;
        traits->debugLayer  = false;

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
            if (::GetClientRect(reinterpret_cast<HWND>(host_handle), &client_rect) && client_rect.right > client_rect.left &&
                client_rect.bottom > client_rect.top)
            {
                traits->width  = client_rect.right - client_rect.left;
                traits->height = client_rect.bottom - client_rect.top;
            }
#else
            // vsg's Xcb backend reads the native window as an xcb_window_t
            // (uint32_t). The host handle carries QWindow::winId() bits, so
            // narrow it to exactly that type; std::any only matches on the
            // exact type, and storing a void*/64-bit handle makes vsg throw
            // bad_any_cast when it casts back to xcb_window_t.
            traits->nativeWindow = static_cast<unsigned int>(
                reinterpret_cast<std::uintptr_t>(host_handle));
#endif
        }
        d->window = ::vsg::Window::create(traits);
        if (d->window == nullptr) {
            std::fprintf(stderr,
                         "[VsgRenderer] Window::create FAILED (nativeWindow=%d, %ux%u)\n",
                         traits->nativeWindow.has_value() ? 1 : 0,
                         traits->width,
                         traits->height);
            shutdown();
            return false;
        }

        // Retained vsg root: SceneBridge fills it each frame from the render
        // command stream, so scene edits (move / recolor / add / remove) show up
        // without re-initializing the backend.
        auto root    = ::vsg::Group::create();
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
        auto shaderSet = buildShaderSet(d->shader_preset, d->window->extent2D(), true);
        d->sceneBridge.setShaderSet(shaderSet);
        d->sceneBridge.setMaterialManager(&d->materialManager);
        d->sceneBridge.clearCache();

        // Overlays (HUD) render on top of the main scene: give them a second
        // bridge whose pipelines disable depth test/write so the axis gizmo is
        // never hidden by scene geometry in front of it.
        d->overlayBridge.setShaderSet(buildShaderSet(d->shader_preset, d->window->extent2D(), false));
        d->overlayBridge.setMaterialManager(&d->materialManager);
        d->overlayBridge.clearCache();

        if (forceOwnWindow()) {
            // TEMP test: render a raw vsg triangle built directly, bypassing the
            // Vine scene/SceneBridge layer, to validate vsg rendering in the
            // independent window.
            root->addChild(makeRawDemoNode());
        }
        else {
            // Pre-populate the retained graph from the current scene so existing
            // content is compiled once here, before any frame runs. Compiling
            // freshly added geometry at runtime inside render() has proven
            // unreliable, so all content present at startup is built and compiled
            // now; runtime additions are still attempted later via
            // syncRenderCommands.
            const auto                               initial_commands = collectSceneCommandsNoCull(d->scene);
            std::vector<::vsg::ref_ptr<::vsg::Node>> created_at_init;
            d->sceneBridge.syncRenderCommands(initial_commands, root, &created_at_init);
        }

        // Viewer + render graph. The main view is built by hand (mirroring
        // vsg::createRenderGraphForView) so the backend owns the View and can
        // swap its light sources at runtime (see setLights()): the view holds a
        // light group (default = the vsg headlight) followed by the scene root.
        // EmbeddedViewer disables vsg's native message pumping (Qt owns the
        // message loop here).
        d->viewer = ::vsg::ref_ptr<::vsg::Viewer>(new EmbeddedViewer());
        d->viewer->addWindow(d->window);

        d->main_view = ::vsg::View::create(d->vsg_camera);
        d->main_light_group = ::vsg::Group::create();
        if (d->scene != nullptr && !d->scene->lights().empty()) {
            // Seed the view's lights from the scene BEFORE the first compile so
            // vsg selects the shadow-capable phong variant when a light casts.
            for (const auto& light_ptr : d->scene->lights()) {
                if (auto node = buildLightNode(light_ptr.get())) {
                    d->main_light_group->addChild(node);
                }
            }
        } else {
            d->main_light_group->addChild(::vsg::createHeadlight());
        }
        d->main_view->addChild(d->main_light_group);
        d->main_view->addChild(d->vsg_scene);

        // TEMP shadow-isolation probe: draw one magenta vsg::Builder box as a
        // sibling of the scene under the main view (syncRenderCommands rewrites
        // only the scene root's children each frame, so this stays). If it
        // casts a visible shadow while the SceneBridge boxes do not, SceneBridge
        // geometry is not reaching vsg's internal shadow map.
        if (std::getenv("VINE_VSG_PROBE_BUILDER_BOX") != nullptr) {
            auto options = ::vsg::Options::create();
            options->sharedObjects = ::vsg::SharedObjects::create();
            auto builder = ::vsg::Builder::create();
            builder->options = options;
            ::vsg::GeometryInfo geom;
            geom.position = ::vsg::vec3(2.0f, 0.35f, 1.6f);
            geom.dx       = ::vsg::vec3(0.7f, 0.0f, 0.0f);
            geom.dy       = ::vsg::vec3(0.0f, 0.7f, 0.0f);
            geom.dz       = ::vsg::vec3(0.0f, 0.0f, 0.7f);
            geom.color    = ::vsg::vec4(1.0f, 0.4f, 0.9f, 1.0f);
            ::vsg::StateInfo state;
            d->main_view->addChild(builder->createBox(geom, state));
        }

        auto renderGraph = ::vsg::RenderGraph::create(d->window, d->main_view);
        renderGraph->contents = VK_SUBPASS_CONTENTS_INLINE;
        d->render_graph       = renderGraph;
        auto commandGraph     = ::vsg::CommandGraph::create(d->window);
        commandGraph->addChild(renderGraph);
        d->command_graph = commandGraph;
        d->viewer->assignRecordAndSubmitTaskAndPresentation(::vsg::CommandGraphs{ commandGraph });

        const auto compileResult = d->viewer->compile();
        if (!compileResult) {
            std::fprintf(stderr, "[VsgRenderer] compile failed: %s\n", compileResult.message.c_str());
            shutdown();
            return false;
        }

        d->initialized = true;
        return true;
    // }
    // catch (const std::exception& e) {
    //     std::fprintf(stderr, "[VsgRenderer] initialize exception: %s\n", e.what());
    // }
    // catch (...) {
    //     std::fprintf(stderr, "[VsgRenderer] initialize unknown exception\n");
    // }
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
    d->main_view    = nullptr;
    d->main_light_group = nullptr;
    d->vsg_scene    = nullptr;
    d->vsg_camera   = nullptr;
    d->sceneBridge.clearCache();
    d->materialManager.clear();
    d->bound_handle = nullptr;
    d->initialized  = false;
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

void VsgRenderer::executePass(vine::raw_ptr<const vine::graphics::RenderPass> pass, const std::vector<vine::graphics::RenderCommand>& commands)
{
    if (pass == nullptr) {
        return;
    }
    clear(pass->clearColor(), pass->shouldClearDepth());
    render(commands, pass->camera());
}

void VsgRenderer::setRenderTarget(vine::raw_ptr<vine::graphics::RenderTarget> target)
{
    // Queue the target for the next render() call (mirrors setViewport()).
    d->active_target = target;
}

void VsgRenderer::setLights(const std::vector<vine::raw_ptr<const vine::graphics::Light>>& lights)
{
    // Queue the lights for the next render() call (mirrors setViewport()): the
    // light nodes are built when the matching view is reconciled in render().
    d->pending_lights.clear();
    d->pending_lights.reserve(lights.size());
    for (const auto* light : lights) {
        d->pending_lights.push_back(light);
    }
}

bool VsgRenderer::supportsRenderTargets()
{
    return true;
}

void VsgRenderer::render(const std::vector<vine::graphics::RenderCommand>& commands, vine::raw_ptr<const vine::graphics::Camera> camera)
{
    if (!d->initialized || d->viewer == nullptr) {
        return;
    }

    // Consume the sub-viewport queued by setViewport() just before this pass
    // (overlays); the main pass never sets one and renders the full surface.
    const bool has_vp       = d->has_pending_viewport;
    const int  vp_x         = d->pending_viewport[0];
    const int  vp_y         = d->pending_viewport[1];
    const int  vp_w         = d->pending_viewport[2];
    const int  vp_h         = d->pending_viewport[3];
    d->has_pending_viewport = false;

    // Consume the lights queued by setLights() just before this pass (from the
    // content scene). Empty keeps each view's default light(s).
    std::vector<const vine::graphics::Light*> lights = std::move(d->pending_lights);
    d->pending_lights.clear();

    // Off-screen render-to-texture pass: the engine queued a non-null target.
    if (d->active_target != nullptr) {
        vine::graphics::RenderTarget* target = d->active_target;
        d->active_target                   = nullptr;
        renderOffscreenTarget(target, commands, camera, lights);
        d->needs_submit = true;
        return;
    }

    const bool is_main = (camera == nullptr || camera == d->camera);
    if (is_main) {
        if (camera != nullptr) {
            d->cameraBridge.apply(const_cast<vine::graphics::Camera*>(camera), d->vsg_camera);
        }
        // Content lights replace the main view's default headlight.
        setGroupLights(d->main_light_group.get(), lights);
        // The command stream is the source of truth: reconcile the retained
        // vsg scene against it (in-place for moves/material edits).
        auto* root = d->vsg_scene.cast<::vsg::Group>().get();
        if (!forceOwnWindow()) {
            std::vector<::vsg::ref_ptr<::vsg::Node>> created;
            const bool rebuilt = d->sceneBridge.syncRenderCommands(commands, root, &created);
            // TEMP diag: report for the first few frames whether scene->vsg
            // rebuilds geometry nodes every frame or reuses them.
            static int s_sync_diag = 0;
            if (s_sync_diag < 5) {
                ++s_sync_diag;
                std::fprintf(stderr,
                             "[VsgRenderer][diag] main sync: created=%zu rootChildren=%zu changed=%d\n",
                             created.size(), root->children.size(), rebuilt ? 1 : 0);
            }
            if (!created.empty()) {
                // A full-graph compile keeps newly built/rebuild subtrees
                // correct (see design doc §9 Phase 3 for the incremental plan).
                const auto compileResult = d->viewer->compile();
                if (!compileResult) {
                    std::fprintf(stderr, "[VsgRenderer] compile in render failed: %s\n", compileResult.message.c_str());
                }
            }
        }
    }
    else if (camera != nullptr) {
        renderOverlayPass(commands, camera, has_vp ? vp_x : 0, has_vp ? vp_y : 0, has_vp ? vp_w : 0, has_vp ? vp_h : 0);
    }

    // Submission is deferred to swapBuffers() so one frame (main pass + all
    // overlay passes) is recorded and presented exactly once.
    d->needs_submit = true;
}

void VsgRenderer::renderOffscreenTarget(vine::graphics::RenderTarget* target,
                                        const std::vector<vine::graphics::RenderCommand>& commands,
                                        vine::raw_ptr<const vine::graphics::Camera> camera,
                                        const std::vector<const vine::graphics::Light*>& lights)
{
    // EXPERIMENTAL: off-screen color (+ optional depth) or depth-only
    // (shadow map) render-to-texture. Must be validated on a real Vulkan
    // device before production use.
    if (target == nullptr || !target->valid() || (!target->hasColor() && !target->hasDepth()) || camera == nullptr) {
        return;
    }
    auto* cam = const_cast<vine::graphics::Camera*>(camera);
    auto& off = d->offscreen[target];

    // Rebuild when the target's logical size changed (render-to-texture must
    // track the off-screen buffer size). The map entry and its bridge are kept
    // (bridge cache is dropped) so the creation block below rebuilds the GPU
    // attachments + render graph against the new size.
    if (off.graph && (off.width != target->width() || off.height != target->height())) {
        if (d->command_graph != nullptr) {
            auto& children = d->command_graph->children;
            children.erase(
                std::remove_if(children.begin(), children.end(),
                               [&off](const ::vsg::ref_ptr<::vsg::Node>& child) {
                                   return child.get() == off.graph.get();
                               }),
                children.end());
        }
        // Wait for any in-flight command buffer that may still reference the
        // old framebuffer/images before their Vk handles are destroyed.
        if (d->viewer != nullptr) {
            d->viewer->deviceWaitIdle();
        }
        off.bridge.clearCache();
        off.color_image = {};
        off.color_view  = {};
        off.depth_image = {};
        off.depth_view  = {};
        off.render_pass = {};
        off.framebuffer = {};
        off.graph       = {};
        off.vsg_camera  = {};
        off.view        = {};
        off.light_group = {};
        off.root        = {};
        off.width       = 0;
        off.height      = 0;
    }

    if (!off.graph) {
        if (d->window == nullptr) {
            return;
        }
        const uint32_t w = static_cast<uint32_t>(target->width());
        const uint32_t h = static_cast<uint32_t>(target->height());
        if (w == 0 || h == 0) {
            return;
        }
        off.width  = static_cast<int>(w);
        off.height = static_cast<int>(h);
        auto device = d->window->getOrCreateDevice();
        const bool has_color = target->hasColor();
        const bool has_depth = target->hasDepth();

        ::vsg::ImageViews attachments;
        attachments.reserve((has_color ? 1u : 0u) + (has_depth ? 1u : 0u));
        if (has_color) {
            // Color attachment: written here, later usable as a sampled texture
            // (VK_IMAGE_USAGE_SAMPLED_BIT) or as a blit source for compositing.
            auto color = ::vsg::Image::create();
            color->imageType     = VK_IMAGE_TYPE_2D;
            color->format        = toColorFormat(target->colorFormat());
            color->extent        = VkExtent3D{ w, h, 1 };
            color->mipLevels     = 1;
            color->arrayLayers   = 1;
            color->tiling        = VK_IMAGE_TILING_OPTIMAL;
            color->usage         = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
            color->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            off.color_image      = color;
            // createImageView compiles the Image (creates VkImage + allocates/binds
            // device memory) and creates+compiles the ImageView (VkImageView).
            // WITHOUT this the VkImage/VkImageView stay VK_NULL_HANDLE and the
            // Framebuffer holds a corrupt handle -> vkCmdBeginRenderPass crashes.
            off.color_view = ::vsg::createImageView(device.get(), color, VK_IMAGE_ASPECT_COLOR_BIT);
            attachments.push_back(off.color_view);
        }
        if (has_depth) {
            auto depth = ::vsg::Image::create();
            depth->imageType     = VK_IMAGE_TYPE_2D;
            depth->format        = toDepthFormat(target->depthFormat());
            depth->extent        = VkExtent3D{ w, h, 1 };
            depth->mipLevels     = 1;
            depth->arrayLayers   = 1;
            depth->tiling        = VK_IMAGE_TILING_OPTIMAL;
            depth->usage         = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
            depth->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            off.depth_image      = depth;
            off.depth_view       = ::vsg::createImageView(device.get(), depth, VK_IMAGE_ASPECT_DEPTH_BIT);
            attachments.push_back(off.depth_view);
        }

        off.render_pass = has_color
            ? makeSampleableRenderPass(device.get(),
                                       toColorFormat(target->colorFormat()),
                                       has_depth ? toDepthFormat(target->depthFormat()) : VK_FORMAT_UNDEFINED)
            : makeDepthOnlyRenderPass(device.get(), toDepthFormat(target->depthFormat()));
        off.framebuffer = ::vsg::Framebuffer::create(off.render_pass, attachments, w, h, 1);

        off.graph              = ::vsg::RenderGraph::create();
        off.graph->framebuffer = off.framebuffer;
        off.graph->renderArea  = VkRect2D{ { 0, 0 }, { w, h } };
        off.graph->contents    = VK_SUBPASS_CONTENTS_INLINE;
        off.graph->viewportState = ::vsg::ViewportState::create(VkExtent2D{ w, h });
        // Clear values match the attachment order (colour then depth). Depth is
        // cleared to the far plane for depth-only (shadow) targets and to the
        // previously proven value for colour+RT targets.
        off.graph->clearValues.clear();
        if (has_color) {
            VkClearValue color_clear = {};
            color_clear.color        = VkClearColorValue{ { 0.2f, 0.2f, 0.2f, 1.0f } };
            off.graph->clearValues.push_back(color_clear);
        }
        if (has_depth) {
            VkClearValue depth_clear = {};
            depth_clear.depthStencil = VkClearDepthStencilValue{ has_color ? 0.0f : 1.0f, 0 };
            off.graph->clearValues.push_back(depth_clear);
        }

        // A View with its own camera + retained scene root. Its default light
        // is a dim ambient fill; content lights (setLights) replace it later.
        off.root = ::vsg::Group::create();
        // Configure this target's dedicated bridge (main-scene depth-testing
        // policy); its pipelines compile against the off-screen render pass
        // under this view's own viewID.
        off.bridge.setShaderSet(buildShaderSet(d->shader_preset, VkExtent2D{ w, h }, true));
        off.bridge.setMaterialManager(&d->materialManager);
        off.bridge.clearCache();
        off.vsg_camera = d->cameraBridge.create(cam);
        if (off.vsg_camera == nullptr) {
            d->offscreen.erase(target);
            return;
        }
        off.light_group = ::vsg::Group::create();
        if (!lights.empty()) {
            // Seed the off-screen view's lights BEFORE its first compile.
            for (const auto* light : lights) {
                if (auto node = buildLightNode(light)) {
                    off.light_group->addChild(node);
                }
            }
        } else {
            auto ambient = ::vsg::AmbientLight::create();
            ambient->name = "offscreen_ambient";
            ambient->color.set(1.0f, 1.0f, 1.0f);
            ambient->intensity = 1.0f;
            off.light_group->addChild(ambient);
        }
        off.view = ::vsg::View::create(off.vsg_camera);
        off.view->addChild(off.light_group);
        off.view->addChild(off.root);
        off.graph->addChild(off.view);

        if (d->command_graph != nullptr) {
            // Record off-screen graphs BEFORE the main window graph so the
            // colour texture they produce is current when the PiP screen pass
            // (a second view of the main graph) samples it in the same frame.
            d->command_graph->children.insert(d->command_graph->children.begin(), off.graph);
        }
        // Compile the new graph (its pipelines are built against the off-screen
        // render pass). EXPERIMENTAL: must be validated on-device.
        const auto compileResult = d->viewer->compile();
        if (!compileResult) {
            std::fprintf(stderr, "[VsgRenderer] offscreen compile failed: %s\n", compileResult.message.c_str());
            return;
        }
        std::fprintf(stderr, "[VsgRenderer] EXPERIMENTAL off-screen target %ux%u attached\n", w, h);
    }

    // Content lights replace the off-screen view's default ambient fill.
    setGroupLights(off.light_group.get(), lights);

    // Update the off-screen view from the pass camera, then sync the retained
    // geometry (this target's dedicated SceneBridge) under the target's root.
    d->cameraBridge.apply(cam, off.vsg_camera);
    std::vector<::vsg::ref_ptr<::vsg::Node>> created;
    auto* root = off.root.cast<::vsg::Group>().get();
    off.bridge.syncRenderCommands(commands, root, &created);
    if (!created.empty()) {
        const auto compileResult = d->viewer->compile();
        if (!compileResult) {
            std::fprintf(stderr, "[VsgRenderer] offscreen sync compile failed: %s\n", compileResult.message.c_str());
        }
    }
}

void VsgRenderer::drawScreenTexture(vine::graphics::RenderTarget* source)
{
    if (!d->initialized || d->viewer == nullptr || d->window == nullptr || source == nullptr) {
        return;
    }

    // Consume the sub-viewport queued by setViewport() (the ScreenPass's PiP
    // rectangle); mirrors how render() consumes one for overlays.
    const bool has_vp = d->has_pending_viewport;
    const int  vp_x   = d->pending_viewport[0];
    const int  vp_y   = d->pending_viewport[1];
    const int  vp_w   = d->pending_viewport[2];
    const int  vp_h   = d->pending_viewport[3];
    d->has_pending_viewport = false;

    auto src_it = d->offscreen.find(source);
    if (src_it == d->offscreen.end() || src_it->second.color_view == nullptr) {
        std::fprintf(stderr, "[VsgRenderer] drawScreenTexture: source target has no colour attachment\n");
        return;
    }
    const auto& src = src_it->second;
    if (src.width <= 0 || src.height <= 0) {
        return;
    }

    // Drop a stale slot when the sampled target was resized (its colour view
    // was rebuilt, so the old descriptor would sample a destroyed image).
    {
        const auto old = d->screen_slots.find(source);
        if (old != d->screen_slots.end() && old->second.ready &&
            (old->second.source_w != src.width || old->second.source_h != src.height)) {
            if (d->render_graph != nullptr) {
                auto& children = d->render_graph->children;
                children.erase(
                    std::remove_if(children.begin(), children.end(),
                                   [&old](const ::vsg::ref_ptr<::vsg::Node>& child) {
                                       return child.get() == old->second.view.get();
                                   }),
                    children.end());
            }
            d->screen_slots.erase(old);
        }
    }

    auto& slot = d->screen_slots[source];

    // Destination rectangle: the pass's sub-viewport, else the full surface.
    const auto surface = d->window->extent2D();
    const int  surf_w  = static_cast<int>(surface.width);
    const int  surf_h  = static_cast<int>(surface.height);

    int req_x = 0, req_y = 0, req_w = surf_w, req_h = surf_h;
    if (has_vp && vp_w > 0 && vp_h > 0) {
        req_x = vp_x;
        req_y = vp_y;
        req_w = vp_w;
        req_h = vp_h;
    }

    int rect_x = req_x, rect_y = req_y, rect_w = req_w, rect_h = req_h;
    if (rect_x < 0 || rect_y < 0 || rect_w > surf_w || rect_h > surf_h || rect_x + rect_w > surf_w || rect_y + rect_h > surf_h) {
        // The requested rect does not fit the surface (e.g. an anchor computed
        // before the surface size was known): auto-anchor bottom-right inside
        // the surface, keeping the (16:9) size within half of it.
        int w = req_w;
        int h = req_h;
        if (w > surf_w / 2) {
            w = surf_w / 2;
            h = static_cast<int>(w * 9 / 16);
        }
        if (h > surf_h / 2) {
            h = surf_h / 2;
            w = static_cast<int>(h * 16 / 9);
        }
        const int margin = 8;
        rect_x = surf_w - w - margin;
        rect_y = surf_h - h - margin;
        rect_w = w;
        rect_h = h;
    }

    if (!slot.ready) {
        slot.source_w   = src.width;
        slot.source_h   = src.height;
        slot.source_view = src.color_view;

        // Full-screen textured triangle sampling the off-screen colour, drawn
        // as a second View of the main window render graph (like overlays) so
        // the sub-viewport clips the picture-in-picture rectangle.
        auto content = makeScreenTextureNode(src.color_view, surface);
        if (content == nullptr) {
            d->screen_slots.erase(source);
            return;
        }
        auto camera = ::vsg::Camera::create();
        camera->viewportState = ::vsg::ViewportState::create(rect_x, rect_y, static_cast<uint32_t>(rect_w), static_cast<uint32_t>(rect_h));
        slot.camera = camera;
        auto view = ::vsg::View::create(camera);
        auto group = ::vsg::Group::create();
        group->addChild(content);
        view->addChild(group);
        slot.view = view;
        if (d->render_graph != nullptr) {
            d->render_graph->addChild(view);
        }
        // Compile the new view (its pipeline is built against the window
        // render pass) before it is first recorded.
        const auto compileResult = d->viewer->compile();
        if (!compileResult) {
            std::fprintf(stderr, "[VsgRenderer] screen pass compile failed: %s\n", compileResult.message.c_str());
            // Drop the half-compiled view so it is never recorded.
            if (d->render_graph != nullptr) {
                auto& children = d->render_graph->children;
                children.erase(
                    std::remove_if(children.begin(), children.end(),
                                   [&view](const ::vsg::ref_ptr<::vsg::Node>& child) {
                                       return child.get() == view.get();
                                   }),
                    children.end());
            }
            d->screen_slots.erase(source);
            return;
        }
        slot.ready = true;
        d->needs_submit = true;
        std::fprintf(stderr, "[VsgRenderer] EXPERIMENTAL screen PiP %dx%d -> %d,%d %dx%d attached\n",
                     src.width, src.height, rect_x, rect_y, rect_w, rect_h);
    }

    // Follow the requested sub-viewport each frame (dynamic viewport + scissor).
    slot.camera->viewportState = ::vsg::ViewportState::create(rect_x, rect_y, static_cast<uint32_t>(rect_w), static_cast<uint32_t>(rect_h));
    d->needs_submit = true;
}

void VsgRenderer::releaseOverlay(vine::raw_ptr<const vine::graphics::Camera> overlay_camera)
{
    if (overlay_camera == nullptr || !d->initialized) {
        return;
    }
    auto it = d->overlay_slots.find(const_cast<vine::graphics::Camera*>(overlay_camera));
    if (it == d->overlay_slots.end()) {
        return;
    }
    // Detach the overlay's second View from the main render graph so it is no
    // longer recorded each frame, then drop it (releases its compiled
    // pipelines). Overlay removal is rare, so a device wait before the drop
    // keeps the release safe against an in-flight frame.
    if (d->render_graph != nullptr && it->second.view != nullptr) {
        auto& children = d->render_graph->children;
        children.erase(std::remove_if(children.begin(), children.end(),
                                      [&it](const ::vsg::ref_ptr<::vsg::Node>& child) {
                                          return child.get() == it->second.view.get();
                                      }),
                       children.end());
    }
    if (d->viewer != nullptr) {
        d->viewer->deviceWaitIdle();
    }
    d->overlay_slots.erase(it);
}

void VsgRenderer::releaseRenderTarget(vine::graphics::RenderTarget* target)
{
    if (target == nullptr || !d->initialized) {
        return;
    }
    bool released = false;
    auto ot = d->offscreen.find(target);
    if (ot != d->offscreen.end()) {
        // Remove the target's off-screen graph from the command graph before
        // dropping its images / views / render pass / framebuffer / bridge.
        auto& off = ot->second;
        if (d->command_graph != nullptr && off.graph != nullptr) {
            auto& children = d->command_graph->children;
            children.erase(std::remove_if(children.begin(), children.end(),
                                          [&off](const ::vsg::ref_ptr<::vsg::Node>& child) {
                                              return child.get() == off.graph.get();
                                          }),
                           children.end());
        }
        if (d->viewer != nullptr) {
            d->viewer->deviceWaitIdle();
        }
        off.bridge.clearCache();
        d->offscreen.erase(ot);
        released = true;
    }
    auto st = d->screen_slots.find(target);
    if (st != d->screen_slots.end()) {
        // Drop any picture-in-picture view sampling this target's colour.
        if (d->render_graph != nullptr && st->second.view != nullptr) {
            auto& children = d->render_graph->children;
            children.erase(std::remove_if(children.begin(), children.end(),
                                          [&st](const ::vsg::ref_ptr<::vsg::Node>& child) {
                                              return child.get() == st->second.view.get();
                                          }),
                           children.end());
        }
        if (d->viewer != nullptr) {
            d->viewer->deviceWaitIdle();
        }
        d->screen_slots.erase(st);
        released = true;
    }
    if (released) {
        std::fprintf(stderr, "[VsgRenderer] released GPU resources for removed render target\n");
    }
}

void VsgRenderer::renderOverlayPass(const std::vector<vine::graphics::RenderCommand>& commands,
                                    vine::raw_ptr<const vine::graphics::Camera> camera,
                                    int vp_x,
                                    int vp_y,
                                    int vp_w,
                                    int vp_h)
{
    auto* cam  = const_cast<vine::graphics::Camera*>(camera);
    auto& slot = d->overlay_slots[cam];
    if (!slot.ready) {
        slot.camera     = cam;
        slot.root       = ::vsg::Group::create();
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
        auto view     = ::vsg::View::create(slot.vsg_camera);
        // Overlay (HUD) content is lit by a single ambient light: with a
        // directional headlight the axis went dark/black from diagonal views
        // because the light direction is fixed while the overlay camera (which
        // mirrors the source) rotates. Ambient-only lighting makes phong's
        // colour independent of surface orientation, giving flat sticks.
        auto ambient  = ::vsg::AmbientLight::create();
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
        vp_w              = static_cast<int>(extent.width);
        vp_h              = static_cast<int>(extent.height);
    }
    slot.vsg_camera->viewportState = ::vsg::ViewportState::create(vp_x, vp_y, static_cast<uint32_t>(vp_w), static_cast<uint32_t>(vp_h));

    d->cameraBridge.apply(cam, slot.vsg_camera);

    std::vector<::vsg::ref_ptr<::vsg::Node>> created;
    d->overlayBridge.syncRenderCommands(commands, slot.root.get(), &created);
    if (!created.empty()) {
        const auto compileResult = d->viewer->compile();
        if (!compileResult) {
            std::fprintf(stderr, "[VsgRenderer] overlay compile failed: %s\n", compileResult.message.c_str());
        }
    }
}

void VsgRenderer::setViewport(int x, int y, int width, int height)
{
    d->pending_viewport[0]  = x;
    d->pending_viewport[1]  = y;
    d->pending_viewport[2]  = width;
    d->pending_viewport[3]  = height;
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

vine::raw_ptr<vine::graphics::MaterialManager> VsgRenderer::materialManager()
{
    return &d->materialManager;
}

void VsgRenderer::setShaderPreset(vine::graphics::ShaderPreset preset)
{
    d->shader_preset = preset;
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
