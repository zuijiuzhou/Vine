#include <vine/vsg/VsgRenderer.hpp>

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
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
#include <vine/graphics/ShaderProgram.hpp>
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
#include <vsg/core/Array.h>
#include <vsg/state/ColorBlendState.h>
#include <vsg/state/DepthStencilState.h>
#include <vsg/state/DescriptorImage.h>
#include <vsg/state/Image.h>
#include <vsg/state/ImageInfo.h>
#include <vsg/state/ImageView.h>
#include <vsg/state/InputAssemblyState.h>
#include <vsg/state/MultisampleState.h>
#include <vsg/state/PushConstants.h>
#include <vsg/state/RasterizationState.h>
#include <vsg/state/Sampler.h>
#include <vsg/state/ShaderStage.h>
#include <vsg/state/ViewDependentState.h>
#include <vsg/state/ViewportState.h>
#include <vsg/state/material.h>
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
 * @param preset      Shading-model preset to build for.
 * @param extent      Window extent for the baked static viewport.
 * @param depth_test  When false, depth test/write are disabled so the geometry
 *                    always draws on top of previously rendered content (used
 *                    for HUD overlays such as the axis gizmo).
 * @param color_count Colour attachment count of the target this set renders
 *                    into. Vulkan requires the pipeline's color-blend
 *                    attachment count to equal the subpass's colour count, so
 *                    MRT targets (color_count > 1) get a matching default
 *                    ColorBlendState; single-colour targets keep one (the
 *                    default).
 * @return Configured shader set.
 */
::vsg::ref_ptr<::vsg::ShaderSet> buildShaderSet(vine::graphics::ShaderPreset preset, const VkExtent2D& extent, bool depth_test, int color_count = 1)
{
    // Pbr / ShadowedPhong are reserved presets without a backend mapping yet
    // (Pbr needs its own PbrMaterialValue; shadow comes last in the roadmap),
    // so they fall back to the Phong shader set for now.
    ::vsg::ref_ptr<::vsg::ShaderSet> shaderSet =
        (preset == vine::graphics::ShaderPreset::FlatShaded) ? ::vsg::createFlatShadedShaderSet() : ::vsg::createPhongShaderSet();
    auto raster_state      = ::vsg::RasterizationState::create();
    raster_state->cullMode = VK_CULL_MODE_NONE; // tolerate either winding order
    auto depth_state       = ::vsg::DepthStencilState::create();
    if (!depth_test) {
        depth_state->depthTestEnable  = VK_FALSE;
        depth_state->depthWriteEnable = VK_FALSE;
    }
    ::vsg::ref_ptr<::vsg::ColorBlendState> blend_state;
    if (color_count > 1) {
        // MRT: one color-blend attachment per colour attachment, blending off
        // and writing every channel (mirrors the single-attachment default).
        ::vsg::ColorBlendState::ColorBlendAttachments attachments;
        attachments.reserve(static_cast<std::size_t>(color_count));
        for (int i = 0; i < color_count; ++i) {
            VkPipelineColorBlendAttachmentState attachment = {};
            attachment.blendEnable         = VK_FALSE;
            attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
            attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
            attachment.colorBlendOp        = VK_BLEND_OP_ADD;
            attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
            attachment.alphaBlendOp        = VK_BLEND_OP_ADD;
            attachment.colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            attachments.push_back(attachment);
        }
        blend_state = ::vsg::ColorBlendState::create(attachments);
    }
    else {
        blend_state = ::vsg::ColorBlendState::create();
    }
    shaderSet->defaultGraphicsPipelineStates = ::vsg::GraphicsPipelineStates{
        depth_state,
        raster_state,
        blend_state,
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
class EmbeddedViewer : public ::vsg::Inherit<::vsg::Viewer, EmbeddedViewer> {
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
 * @brief Builds an off-screen render pass whose colour attachments end in
 * SHADER_READ_ONLY_OPTIMAL so a later pass can sample them as textures.
 *
 * vsg::createRenderPass() leaves the colour attachment in PRESENT_SRC_KHR
 * (correct for swapchain output, wrong for a texture sampled by a later
 * pass). The dependency on subpass-external fragment-shader reads makes the
 * colour writes visible to the sampling pass without an extra barrier. A
 * target with several colour attachments (MRT / G-buffer) gets one attachment
 * per entry, all sampleable on their own; fragment output @p i writes
 * attachment @p i.
 *
 * @param device       Device the render pass is created on.
 * @param color_formats Colour attachment formats, one per attachment (in
 *                      attachment order).
 * @param depth_format Depth attachment format, or VK_FORMAT_UNDEFINED for a
 *                     colour-only pass.
 * @return The configured render pass.
 */
::vsg::ref_ptr<::vsg::RenderPass> makeSampleableRenderPass(
    ::vsg::Device*                    device,
    const std::vector<VkFormat>&      color_formats,
    VkFormat                          depth_format)
{
    const bool has_depth = depth_format != VK_FORMAT_UNDEFINED;

    ::vsg::RenderPass::Attachments attachments;
    attachments.reserve(color_formats.size() + (has_depth ? 1u : 0u));

    ::vsg::SubpassDescription subpass = {};
    subpass.pipelineBindPoint         = VK_PIPELINE_BIND_POINT_GRAPHICS;

    uint32_t attachment_index = 0;
    for (const VkFormat color_format : color_formats) {
        ::vsg::AttachmentDescription color = {};
        color.format                       = color_format;
        color.samples                      = VK_SAMPLE_COUNT_1_BIT;
        color.loadOp                       = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color.storeOp                      = VK_ATTACHMENT_STORE_OP_STORE;
        color.stencilLoadOp                = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color.stencilStoreOp               = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color.initialLayout                = VK_IMAGE_LAYOUT_UNDEFINED;
        color.finalLayout                  = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        attachments.push_back(color);

        ::vsg::AttachmentReference color_ref = {};
        color_ref.attachment                 = attachment_index++;
        color_ref.layout                     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        subpass.colorAttachments.emplace_back(color_ref);
    }

    if (has_depth) {
        ::vsg::AttachmentDescription depth = {};
        depth.format                       = depth_format;
        depth.samples                      = VK_SAMPLE_COUNT_1_BIT;
        depth.loadOp                       = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth.storeOp                      = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth.stencilLoadOp                = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depth.stencilStoreOp               = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth.initialLayout                = VK_IMAGE_LAYOUT_UNDEFINED;
        depth.finalLayout                  = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        attachments.push_back(depth);

        ::vsg::AttachmentReference depth_ref = {};
        depth_ref.attachment                 = attachment_index;
        depth_ref.layout                     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        subpass.depthStencilAttachments.emplace_back(depth_ref);
    }

    ::vsg::RenderPass::Dependencies dependencies;

    // Initial (UNDEFINED) -> COLOR_ATTACHMENT_OPTIMAL before the first subpass.
    ::vsg::SubpassDependency ext_to_sub = {};
    ext_to_sub.srcSubpass               = VK_SUBPASS_EXTERNAL;
    ext_to_sub.dstSubpass               = 0;
    ext_to_sub.srcStageMask             = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    ext_to_sub.dstStageMask             = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    ext_to_sub.dstAccessMask            = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies.push_back(ext_to_sub);

    // After the subpass, transition to SHADER_READ_ONLY and make the colour
    // writes visible to a later pass that samples the attachment.
    ::vsg::SubpassDependency sub_to_ext = {};
    sub_to_ext.srcSubpass               = 0;
    sub_to_ext.dstSubpass               = VK_SUBPASS_EXTERNAL;
    sub_to_ext.srcStageMask             = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    sub_to_ext.dstStageMask             = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    sub_to_ext.srcAccessMask            = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    sub_to_ext.dstAccessMask            = VK_ACCESS_SHADER_READ_BIT;
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
    depth.format                       = depth_format;
    depth.samples                      = VK_SAMPLE_COUNT_1_BIT;
    depth.loadOp                       = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth.storeOp                      = VK_ATTACHMENT_STORE_OP_STORE; // sampled later as a shadow map
    depth.stencilLoadOp                = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth.stencilStoreOp               = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth.initialLayout                = VK_IMAGE_LAYOUT_UNDEFINED;
    depth.finalLayout                  = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    ::vsg::RenderPass::Attachments attachments{ depth };

    ::vsg::AttachmentReference depth_ref = {};
    depth_ref.attachment                 = 0;
    depth_ref.layout                     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    ::vsg::SubpassDescription subpass = {};
    subpass.pipelineBindPoint         = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.depthStencilAttachments.emplace_back(depth_ref);

    ::vsg::RenderPass::Dependencies dependencies;
    // UNDEFINED -> DEPTH_STENCIL_ATTACHMENT before the subpass.
    ::vsg::SubpassDependency ext_to_sub = {};
    ext_to_sub.srcSubpass               = VK_SUBPASS_EXTERNAL;
    ext_to_sub.dstSubpass               = 0;
    ext_to_sub.srcStageMask             = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    ext_to_sub.dstStageMask             = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    ext_to_sub.dstAccessMask            = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependencies.push_back(ext_to_sub);

    // After the subpass, transition to SHADER_READ_ONLY and make the depth
    // writes visible to a later pass that samples the shadow map.
    ::vsg::SubpassDependency sub_to_ext = {};
    sub_to_ext.srcSubpass               = 0;
    sub_to_ext.dstSubpass               = VK_SUBPASS_EXTERNAL;
    sub_to_ext.srcStageMask             = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    sub_to_ext.dstStageMask             = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    sub_to_ext.srcAccessMask            = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    sub_to_ext.dstAccessMask            = VK_ACCESS_SHADER_READ_BIT;
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
    const std::string vertex_source   = R"(#version 450
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

    auto shaderSet    = ::vsg::ShaderSet::create();
    shaderSet->stages = ::vsg::ShaderStages{ vs, fs };
    shaderSet->addDescriptorBinding("screen_tex", "", 0, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, {});

    auto raster                              = ::vsg::RasterizationState::create();
    raster->cullMode                         = VK_CULL_MODE_NONE;
    auto depth_state                         = ::vsg::DepthStencilState::create();
    depth_state->depthTestEnable             = VK_FALSE;
    depth_state->depthWriteEnable            = VK_FALSE;
    shaderSet->defaultGraphicsPipelineStates = ::vsg::GraphicsPipelineStates{
        depth_state,
        raster,
        ::vsg::ColorBlendState::create(),
        ::vsg::InputAssemblyState::create(),
        ::vsg::MultisampleState::create(),
        ::vsg::ViewportState::create(extent),
    };

    auto config     = ::vsg::GraphicsPipelineConfigurator::create(shaderSet);
    auto sampler    = ::vsg::Sampler::create();
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
 * @brief CPU mirror of the deferred-lighting push-constant block.
 *
 * Only vec4 members (std140/std430 offsets coincide), 6 x 16 = 96 bytes <= the
 * declared 128-byte push range. Layout matches the fragment shader ABI the
 * fullscreen program pass expects (see makeFullscreenProgramNode).
 */
struct alignas(16) LightPushBlock
{
    float ambient[4]; // ambient rgb + intensity
    float dir0[4];    // directional light 0: view-space direction
    float col0[4];    // directional light 0: rgb + intensity
    float dir1[4];    // directional light 1: view-space direction
    float col1[4];    // directional light 1: rgb + intensity
    float misc[4];    // x = specular shininess (fixed approximation for now)
};

/**
 * @brief Builds a full-screen textured node running a user fragment program.
 *
 * The vertex shader generates the full-screen triangle from gl_VertexIndex
 * (no vertex buffers / camera matrices); the fragment shader is @p program's
 * fragment stage, sampling @p image_views (one per colour attachment of the
 * source MRT target, descriptor binding i = attachment i) and reading a
 * per-frame @p push_data block (see LightPushBlock). Depth test/write are
 * disabled and blending is off: the draw overwrites the sub-viewport it owns.
 * The retained node is drawn as its own View (viewport = the PiP rectangle),
 * so it composites over previously rendered content.
 *
 * Fragment-shader ABI the user program must follow:
 *   layout(location = 0) in vec2 v_uv;
 *   layout(binding = i) uniform sampler2D <any>;   // i-th source attachment
 *   layout(push_constant) uniform PushConstants { vec4 ... } pc;  // LightPushBlock
 *   layout(location = 0) out vec4 out_color;
 *
 * @param program    User program supplying the fragment stage (any vertex
 *                   stage is ignored; the backend provides the fullscreen VS).
 * @param image_views Source MRT colour-attachment views to sample.
 * @param extent     Surface extent for the baked static viewport.
 * @param push_data  Per-frame push-constant bytes (mutated before each record).
 * @return The drawable state-group, or null when shader compilation failed.
 */
::vsg::ref_ptr<::vsg::Node> makeFullscreenProgramNode(
    vine::raw_ptr<const vine::graphics::ShaderProgram> program,
    const ::vsg::ImageViews&                          image_views,
    const VkExtent2D&                                 extent,
    ::vsg::ref_ptr<::vsg::Data>                       push_data)
{
    if (program == nullptr || image_views.empty() || push_data == nullptr) {
        return ::vsg::ref_ptr<::vsg::Node>();
    }
    // Locate the user's fragment stage.
    const vine::graphics::ShaderStage* fs_spec = nullptr;
    for (const auto& stage : program->stages()) {
        if (stage.type == vine::graphics::ShaderStageType::Fragment) {
            fs_spec = &stage;
            break;
        }
    }
    if (fs_spec == nullptr) {
        std::fprintf(stderr, "[VsgRenderer] fullscreen program: no fragment stage\n");
        return ::vsg::ref_ptr<::vsg::Node>();
    }

    const std::string vertex_source = R"(#version 450
layout(location = 0) out vec2 v_uv;
void main()
{
    v_uv = vec2(float((gl_VertexIndex << 1) & 2), float(gl_VertexIndex & 2));
    gl_Position = vec4(v_uv * 2.0 - 1.0, 0.0, 1.0);
}
)";

    auto vs = ::vsg::ShaderStage::create(VK_SHADER_STAGE_VERTEX_BIT, "main", vertex_source);
    auto fs = ::vsg::ShaderStage::create(VK_SHADER_STAGE_FRAGMENT_BIT, fs_spec->entryPoint.stdstr(), fs_spec->source.stdstr());

    auto compiler = ::vsg::ShaderCompiler::create();
    if (compiler == nullptr || !compiler->supported() || !compiler->compile(vs) || !compiler->compile(fs)) {
        std::fprintf(stderr, "[VsgRenderer] fullscreen program: GLSL compilation failed\n");
        return ::vsg::ref_ptr<::vsg::Node>();
    }

    auto shaderSet    = ::vsg::ShaderSet::create();
    shaderSet->stages = ::vsg::ShaderStages{ vs, fs };
    for (std::size_t i = 0; i < image_views.size(); ++i) {
        shaderSet->addDescriptorBinding("gbuffer" + std::to_string(i), "", 0, static_cast<uint32_t>(i),
                                        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT,
                                        ::vsg::ref_ptr<::vsg::Data>());
    }
    // Per-frame light/view parameters (LightPushBlock, 96 bytes of the range).
    shaderSet->addPushConstantRange("pc_light", "", VK_SHADER_STAGE_FRAGMENT_BIT, 0, 128);

    auto raster                              = ::vsg::RasterizationState::create();
    raster->cullMode                         = VK_CULL_MODE_NONE;
    auto depth_state                         = ::vsg::DepthStencilState::create();
    depth_state->depthTestEnable             = VK_FALSE;
    depth_state->depthWriteEnable            = VK_FALSE;
    shaderSet->defaultGraphicsPipelineStates = ::vsg::GraphicsPipelineStates{
        depth_state,
        raster,
        ::vsg::ColorBlendState::create(),
        ::vsg::InputAssemblyState::create(),
        ::vsg::MultisampleState::create(),
        ::vsg::ViewportState::create(extent),
    };

    auto config  = ::vsg::GraphicsPipelineConfigurator::create(shaderSet);
    auto sampler = ::vsg::Sampler::create();
    for (std::size_t i = 0; i < image_views.size(); ++i) {
        auto image_info = ::vsg::ImageInfo::create(sampler, image_views[i], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        config->assignTexture("gbuffer" + std::to_string(i), ::vsg::ImageInfoList{ image_info });
    }
    config->init();

    auto stateGroup = ::vsg::StateGroup::create();
    config->copyTo(stateGroup, ::vsg::ref_ptr<::vsg::SharedObjects>());
    auto drawCommands = ::vsg::Commands::create();
    // Push the per-frame block first (recorded from push_data's current bytes),
    // then draw the full-screen triangle within the same pipeline layout.
    drawCommands->addChild(::vsg::PushConstants::create(VK_SHADER_STAGE_FRAGMENT_BIT, 0, push_data.get()));
    drawCommands->addChild(::vsg::Draw::create(3, 1, 0, 0));
    stateGroup->addChild(drawCommands);
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
    case vine::graphics::LightType::Ambient:
    {
        auto ambient = ::vsg::AmbientLight::create();
        ambient->color.set(c.r, c.g, c.b);
        ambient->intensity = light->intensity();
        return ambient;
    }
    case vine::graphics::LightType::Directional:
    {
        auto dir = ::vsg::DirectionalLight::create();
        dir->color.set(c.r, c.g, c.b);
        dir->intensity = light->intensity();
        const auto v   = light->direction();
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

struct VsgRenderer::Impl {
    CameraBridge                        cameraBridge;
    VsgMaterialManager                  materialManager;
    vine::graphics::ShaderPreset        shader_preset{ vine::graphics::ShaderPreset::StandardPhong };
    void*                               bound_handle = nullptr;
    ::vsg::ref_ptr<::vsg::Window>       window;
    ::vsg::ref_ptr<::vsg::Viewer>       viewer;
    ::vsg::ref_ptr<::vsg::CommandGraph> command_graph;
    // Window-target shader sets shared by its content slots' bridges: the
    // depth-on set keeps depth test/write on; the depth-off set disables them
    // so the slot's content always draws on top of earlier content (HUD).
    // Per-geometry pipelines are compiled per view (vsg compiles per viewID),
    // so every content slot carries its own SceneBridge; off-screen targets
    // bake their own per-size sets (see Target).
    ::vsg::ref_ptr<::vsg::ShaderSet>    depth_on_shader_set;
    ::vsg::ref_ptr<::vsg::ShaderSet>    depth_off_shader_set;
    // Set by clear() (a pass clears, clearEnabled) and consumed by the next
    // render(): a render that did NOT follow a clear() is styled on-top
    // (depth-off + ambient, HUD); one that did is the main (depth-on) content.
    // This only decides the slot's depth STYLE — its stacking position follows
    // the pass order (pending_pass_order), not this flag.
    bool                                main_pending = false;
    bool                                initialized = false;

    /** @brief Identifies one content slot: (camera, explicit pass order). */
    struct ContentKey {
        vine::graphics::Camera* camera = nullptr;
        int                     order  = 0;
        bool operator<(const ContentKey& o) const
        {
            if (camera != o.camera) return camera < o.camera;
            return order < o.order;
        }
    };

    /** @brief One retained content slot: a View (camera + content) under a
     * target's render graph.
     *
     * Slots are keyed by (camera, explicit pass order): each (camera, order)
     * pair is its own retained View + SceneBridge, so several passes sharing
     * one camera keep separate content under the same target (one slot per
     * distinct order). Within a target the slot Views are stacked in ascending
     * @ref order — the pass's explicit pipeline order (the engine runs passes
     * in it and announces it via RenderBackend::setPassOrder) — so a slot's
     * stacking position is its key. @ref on_top is only a depth STYLE: an
     * on-top slot disables depth test/write and uses an ambient light (HUD /
     * overlay), while a main slot fills the whole target with depth testing on
     * and the content lights.
     */
    struct ContentSlot {
        int                           order  = 0;   // slot key (= the pass's explicit pipeline order) + stacking
        bool                          on_top = false; // depth-off HUD style (independent of stacking order)
        ::vsg::ref_ptr<::vsg::Camera> vsg_camera;
        ::vsg::ref_ptr<::vsg::Group>  root;        // retained content root
        ::vsg::ref_ptr<::vsg::Group>  light_group; // lights under this slot's view
        ::vsg::ref_ptr<::vsg::View>   view;
        SceneBridge                   bridge;      // per-view pipelines (vsg compiles per viewID)
        bool                          ready = false;
    };

    // Sub-viewport queued by setViewport(), consumed by the next render().
    bool has_pending_viewport = false;
    int  pending_viewport[4]  = { 0, 0, 0, 0 };
    // Lights queued by setLights(), consumed by the next render().
    std::vector<const vine::graphics::Light*> pending_lights;
    // Pass order queued by setPassOrder(), consumed by the next render(). It
    // is this camera's content-slot key and stacking order (setupContentSlot).
    int pending_pass_order = 0;
    // Set when a frame was drawn; consumed by swapBuffers()/submitFrame().
    bool needs_submit = false;

    // ---- Output targets: the window (nullptr key) + off-screen (RT* key) ----

    /// Target queued by setRenderTarget(), consumed by the next render().
    vine::graphics::RenderTarget* active_target = nullptr;

    /** @brief Identifies one PiP screen slot: (sampled target, attachment). */
    struct ScreenKey {
        vine::graphics::RenderTarget* target     = nullptr;
        int                           attachment = 0;
        bool operator<(const ScreenKey& o) const
        {
            if (target != o.target) return target < o.target;
            return attachment < o.attachment;
        }
    };

    /** @brief One picture-in-picture view sampling another target's colour
     * attachment. */
    struct ScreenSlot {
        ::vsg::ref_ptr<::vsg::Camera>    camera;      // carries the sub-rect viewport
        ::vsg::ref_ptr<::vsg::View>      view;        // extra View of this target's render graph
        ::vsg::ref_ptr<::vsg::ImageView> source_view; // keeps the sampled attachment alive
        int                              source_w = 0;
        int                              source_h = 0;
        bool                             ready    = false;
    };

    /** @brief One retained fullscreen-program view sampling another target's
     * colour attachments through a user fragment program (deferred lighting).
     *
     * Built lazily on first drawScreenProgram(), keyed by the sampled source
     * target; rebuilt when the source is resized or the program changes. The
     * per-frame push block (view-space lights, see LightPushBlock) is written
     * into @p push_data before each record.
     */
    struct ProgramSlot {
        ::vsg::ref_ptr<::vsg::Camera>    camera;     // carries the sub-rect viewport
        ::vsg::ref_ptr<::vsg::View>      view;       // extra View of this target's render graph
        ::vsg::ref_ptr<::vsg::Node>      node;       // the fullscreen program drawable
        ::vsg::ref_ptr<::vsg::Data>      push_data;  // per-frame push-constant bytes
        vine::graphics::ShaderProgram*   program = nullptr; // program the node was built with
        int                              source_w = 0;
        int                              source_h = 0;
        bool                             ready    = false;
    };

    /** @brief One output target (window = nullptr key, off-screen = RT* key).
     *
     * Unified (C6.4 / C6.5): window and off-screen targets are the SAME
     * shape — a RenderGraph whose children are content-slot Views (per
     * (camera, pass order)) plus optional PiP views (screen_slots). The
     * window target's graph is the shared swapchain graph created in
     * initialize(); each off-screen target owns its
     * own graph + attachments (images / views / render pass / framebuffer)
     * and lazily builds per-size shader sets, so one RT can bake several
     * content slots (different program / content / depth policy) the same
     * way the window does.
     */
    struct Target {
        // ---- off-screen GPU attachments (window target: unused) ----
        // One image + view per colour attachment (MRT / G-buffer targets carry
        // several sampleable textures; single-colour targets keep one entry).
        std::vector<::vsg::ref_ptr<::vsg::Image>>     color_images;
        std::vector<::vsg::ref_ptr<::vsg::ImageView>> color_views;
        ::vsg::ref_ptr<::vsg::Image>       depth_image;
        ::vsg::ref_ptr<::vsg::ImageView>   depth_view;
        ::vsg::ref_ptr<::vsg::RenderPass>  render_pass;
        ::vsg::ref_ptr<::vsg::Framebuffer> framebuffer;
        ::vsg::ref_ptr<::vsg::RenderGraph> graph; // off-screen: owned here; window: the shared swapchain graph
        // Per-size shader sets for off-screen slots (window slots share
        // impl->depth_on_shader_set / depth_off_shader_set). Built lazily.
        ::vsg::ref_ptr<::vsg::ShaderSet> depth_on_shader_set;
        ::vsg::ref_ptr<::vsg::ShaderSet> depth_off_shader_set;
        int width  = 0; // off-screen logical size
        int height = 0;
        // ---- content slots (retained Views under graph), keyed (camera, order) ----
        std::map<ContentKey, ContentSlot> content_slots;
        // ---- PiP views sampling other targets (drawn under this graph) ----
        std::map<ScreenKey, ScreenSlot> screen_slots;
        // ---- fullscreen-program views (deferred lighting), keyed by source ----
        std::map<vine::graphics::RenderTarget*, ProgramSlot> program_slots;
    };
    std::map<vine::graphics::RenderTarget*, Target> targets; // nullptr key == window
};

VsgRenderer::VsgRenderer()
  : impl(new Impl())
{
}

VsgRenderer::~VsgRenderer()
{
    shutdown();
}

bool VsgRenderer::initialize()
{
    // The unified output-target table keys the window (backbuffer) by a null
    // RenderTarget* — the same identity the graphics engine uses for the
    // on-screen target. The window entry is created below when its shared
    // swapchain render graph is assigned.
    // try {
    // Window. When a host native window is bound, attach to its surface (e.g.
    // a Qt QWindow) instead of creating a separate window.
    auto traits         = ::vsg::WindowTraits::create();
    traits->windowTitle = "Vine";
    traits->width       = 1280;
    traits->height      = 720;
    traits->debugLayer  = false;

    void* host_handle = impl->bound_handle;
    if (forceOwnWindow()) {
        // Temporary test path: create vsg's own window, ignoring the Qt-hosted
        // surface handle, to verify rendering independent of Qt compositing.
        host_handle = nullptr;
    }
    if (host_handle != nullptr) {
#ifdef _WIN32
        traits->nativeWindow = reinterpret_cast<HWND>(host_handle);
        RECT client_rect{};
        if (::GetClientRect(reinterpret_cast<HWND>(host_handle), &client_rect) && client_rect.right > client_rect.left && client_rect.bottom > client_rect.top)
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
        traits->nativeWindow = static_cast<unsigned int>(reinterpret_cast<std::uintptr_t>(host_handle));
#endif
    }
    impl->window = ::vsg::Window::create(traits);
    if (impl->window == nullptr) {
        std::fprintf(stderr,
                     "[VsgRenderer] Window::create FAILED (nativeWindow=%d, %ux%u)\n",
                     traits->nativeWindow.has_value() ? 1 : 0,
                     traits->width,
                     traits->height);
        shutdown();
        return false;
    }

    // Window-target shader sets shared by its content slots (embedded SPIR-V,
    // no runtime glslang): the depth-on set keeps depth test/write on; the
    // depth-off set disables it so the slot's content always draws on top of
    // earlier content (HUD). Off-screen targets bake their own per-size sets
    // lazily.
    impl->depth_on_shader_set  = buildShaderSet(impl->shader_preset, impl->window->extent2D(), true);
    impl->depth_off_shader_set = buildShaderSet(impl->shader_preset, impl->window->extent2D(), false);

    // The primary window layer is created lazily on the first window render
    // (the first pass that clears and draws the scene into the backbuffer).
    // The engine owns the pipeline and drives content per pass, so the
    // renderer binds neither a Vine scene nor a camera and pre-creates
    // nothing here.

    // Viewer. EmbeddedViewer disables vsg's native message pumping (Qt owns
    // the message loop here). Content slots are appended to the render graph
    // later (see setupContentSlot) as extra Views — the canonical vsg
    // multi-viewport pattern: one render pass, later Views drawn on top.
    impl->viewer = ::vsg::ref_ptr<::vsg::Viewer>(new EmbeddedViewer());
    impl->viewer->addWindow(impl->window);

    // Window render graph (empty until the first content slot is created) +
    // command graph. The window target's graph IS this shared swapchain graph
    // (targets[nullptr].graph); every window content slot / PiP view is a
    // child of it.
    auto renderGraph      = ::vsg::RenderGraph::create(impl->window);
    renderGraph->contents = VK_SUBPASS_CONTENTS_INLINE;
    impl->targets[nullptr].graph = renderGraph;
    auto commandGraph     = ::vsg::CommandGraph::create(impl->window);
    commandGraph->addChild(renderGraph);
    impl->command_graph = commandGraph;
    impl->viewer->assignRecordAndSubmitTaskAndPresentation(::vsg::CommandGraphs{ commandGraph });

    const auto compileResult = impl->viewer->compile();
    if (!compileResult) {
        std::fprintf(stderr, "[VsgRenderer] compile failed: %s\n", compileResult.message.c_str());
        shutdown();
        return false;
    }

    impl->initialized = true;
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
    if (impl->viewer != nullptr) {
        impl->viewer->deviceWaitIdle();
        // Detach the window from the viewer so its command graphs are dropped
        // before the viewer is released.
        if (impl->window != nullptr) {
            impl->viewer->removeWindow(impl->window);
        }
        impl->viewer->close();
        impl->viewer = nullptr;
    }
    if (impl->window != nullptr) {
        // Release the native handle the platform window wraps. When the
        // reference is dropped below, the Win32_Window destructor would call
        // ::DestroyWindow() (and ::UnregisterClass()) on the HOST's window —
        // here a Qt-owned HWND that Qt is itself tearing down. releaseWindow()
        // nulls the internal HWND so the destructor leaves Qt's window alone.
        impl->window->releaseWindow();
        impl->window = nullptr;
    }
    // Drop the retained graphs / scenes and the per-geometry / material
    // caches. Compiled pipelines and descriptor sets hold a reference to the
    // old vsg::Device; if they survive a surface-recreate re-init, the next
    // Window::create() allocates a second Device and trips vsg's
    // VSG_MAX_DEVICES limit (== 1 in this build) with an uncaught exception
    // (Device.cpp:63). Everything below must be released before a re-init can
    // create a fresh device.
    // Release every target's retained content slots (views, per-slot bridge
    // caches, lights) and PiP slots. Cleaned before a surface-recreate re-init
    // so no compiled pipeline keeps a reference to the old vsg::Device (see
    // the VSG_MAX_DEVICES note above).
    for (auto& target : impl->targets) {
        for (auto& slot_entry : target.second.content_slots) {
            slot_entry.second.view        = {};
            slot_entry.second.root        = {};
            slot_entry.second.light_group = {};
            slot_entry.second.vsg_camera  = {};
            slot_entry.second.bridge.clearCache();
        }
        target.second.content_slots.clear();
        target.second.screen_slots.clear();
    }
    // Drop the whole unified output-target table: this also releases every
    // off-screen target's attachments / render graph and the window entry's
    // PiP screen slots. Safe here because the viewer and window were already
    // torn down above, so no compiled pipeline keeps a reference to the old
    // vsg::Device.
    impl->targets.clear();
    impl->depth_on_shader_set  = nullptr;
    impl->depth_off_shader_set = nullptr;
    impl->materialManager.clear();
    impl->bound_handle = nullptr;
    impl->initialized  = false;
}

void VsgRenderer::beginFrame()
{
    if (impl->viewer == nullptr) {
        return;
    }
    impl->viewer->advanceToNextFrame();
    impl->viewer->handleEvents();
}

void VsgRenderer::endFrame()
{
    if (impl->viewer == nullptr) {
        return;
    }
    impl->viewer->update();
}

void VsgRenderer::setRenderTarget(vine::raw_ptr<vine::graphics::RenderTarget> target)
{
    // Queue the target for the next render() call (mirrors setViewport()).
    impl->active_target = target;
}

void VsgRenderer::setLights(const std::vector<vine::raw_ptr<const vine::graphics::Light>>& lights)
{
    // Queue the lights for the next render() call (mirrors setViewport()): the
    // light nodes are built when the matching view is reconciled in render().
    impl->pending_lights.clear();
    impl->pending_lights.reserve(lights.size());
    for (const auto* light : lights) {
        impl->pending_lights.push_back(light);
    }
}

bool VsgRenderer::supportsRenderTargets()
{
    return true;
}

void VsgRenderer::render(const std::vector<vine::graphics::RenderCommand>& commands, vine::raw_ptr<const vine::graphics::Camera> camera)
{
    if (!impl->initialized || impl->viewer == nullptr) {
        return;
    }

    // Consume the sub-viewport queued by setViewport() just before this pass
    // (overlays); the main pass never sets one and renders the full surface.
    const bool has_vp          = impl->has_pending_viewport;
    const int  vp_x            = impl->pending_viewport[0];
    const int  vp_y            = impl->pending_viewport[1];
    const int  vp_w            = impl->pending_viewport[2];
    const int  vp_h            = impl->pending_viewport[3];
    impl->has_pending_viewport = false;

    // Consume the lights queued by setLights() just before this pass (from the
    // content scene). Empty keeps each view's default light(s).
    std::vector<const vine::graphics::Light*> lights = std::move(impl->pending_lights);
    impl->pending_lights.clear();

    // Consume the explicit pass order queued by setPassOrder() just before
    // this pass (the engine announces each pass's addPass() order). It is the
    // content-slot key under this camera AND the stacking order (ascending) —
    // setupContentSlot keeps each target's slot views sorted by it, so stacking
    // always follows the user-set pipeline order, whatever the creation order.
    const int pass_order     = impl->pending_pass_order;
    impl->pending_pass_order = 0;

    // Decide main vs on-top from the clear() marker queued by
    // RenderPass::execute: a pass clears (clearEnabled) right before it
    // renders the main scene into the window, so a window render that
    // followed a clear() is the main (depth-on) layer; one without a clear()
    // is an on-top (HUD) layer. The marker is consumed here for every render,
    // so an off-screen pass's clear cannot leak into a later window render.
    // This decides the slot's depth STYLE; its stacking position is governed
    // by pass_order, not by main/on-top semantics.
    const bool on_top     = !impl->main_pending;
    impl->main_pending    = false;

    // The active target (setRenderTarget, nullptr = the window). Every target
    // shares ONE content-slot path (renderContentSlot): only the GPU
    // attachment kind differs, and it is ensured here before the slot draws
    // (window = the shared swapchain graph from initialize(); off-screen =
    // owned attachments + graph, built / rebuilt to the target's size).
    vine::graphics::RenderTarget* key = impl->active_target;
    impl->active_target               = nullptr;

    if (key != nullptr && (camera == nullptr || !key->valid() || (!key->hasColor() && !key->hasDepth()))) {
        // Off-screen target unusable (no camera, invalid, or neither colour
        // nor depth attachment): nothing to draw this pass.
        impl->needs_submit = true;
        return;
    }

    auto& target = impl->targets[key];
    if (key == nullptr) {
        // Window target: its graph is the shared swapchain graph built at
        // initialize() and stored on this entry — nothing to (re)build here.
    }
    else if (target.graph == nullptr || target.width != key->width() || target.height != key->height()) {
        // First render into this off-screen target, or it was resized: build
        // (or rebuild) its attachments + render graph. Any content slots
        // compiled against an older graph are dropped by buildOffscreenTarget.
        buildOffscreenTarget(key);
        if (target.graph == nullptr) {
            return; // off-screen target could not be built
        }
    }

    // Render into a content slot keyed by (camera, pass order) under the
    // active target — window and off-screen share the same slot machinery
    // (C6.4). The slot's depth style (main vs on-top / HUD) is carried per
    // call; its stacking position follows the pass's explicit order.
    if (camera != nullptr) {
        renderContentSlot(key, commands, camera, lights, on_top, pass_order, has_vp ? vp_x : 0, has_vp ? vp_y : 0, has_vp ? vp_w : 0, has_vp ? vp_h : 0);
    }

    // Submission is deferred to swapBuffers() so one frame (main pass + all
    // overlay passes) is recorded and presented exactly once.
    impl->needs_submit = true;
}

void VsgRenderer::buildOffscreenTarget(vine::graphics::RenderTarget* target)
{
    // (Re)build an off-screen target's GPU attachments + render graph, sized
    // to the target. The graph is created EMPTY: content-slot Views are
    // appended by setupContentSlot() as passes render into this target (C6.4:
    // one RT can hold several content slots, like the window target).
    // EXPERIMENTAL: must be validated on a real Vulkan device before
    // production use.
    if (target == nullptr || impl->window == nullptr) {
        return;
    }
    auto& t = impl->targets[target];

    // A rebuild (target resized) must first release the previous graph: it
    // may still be referenced by an in-flight command buffer, and every
    // content slot compiled against it must be dropped with it (per-view
    // pipelines bind the old render pass).
    if (t.graph != nullptr) {
        if (impl->command_graph != nullptr) {
            auto& children = impl->command_graph->children;
            children.erase(std::remove_if(children.begin(),
                                          children.end(),
                                          [&t](const ::vsg::ref_ptr<::vsg::Node>& child) { return child.get() == t.graph.get(); }),
                           children.end());
        }
        // Wait for any in-flight command buffer that may still reference the
        // old framebuffer/images before their Vk handles are destroyed.
        if (impl->viewer != nullptr) {
            impl->viewer->deviceWaitIdle();
        }
        for (auto& slot_entry : t.content_slots) {
            slot_entry.second.bridge.clearCache();
        }
        t.content_slots.clear();
        t.screen_slots.clear();
        t.color_images.clear();
        t.color_views.clear();
        t.depth_image          = {};
        t.depth_view           = {};
        t.render_pass          = {};
        t.framebuffer          = {};
        t.graph                = {};
        t.depth_on_shader_set  = {};
        t.depth_off_shader_set = {};
        t.width                = 0;
        t.height               = 0;
    }

    const uint32_t w = static_cast<uint32_t>(target->width());
    const uint32_t h = static_cast<uint32_t>(target->height());
    if (w == 0 || h == 0) {
        return;
    }
    t.width             = static_cast<int>(w);
    t.height            = static_cast<int>(h);
    auto       device     = impl->window->getOrCreateDevice();
    const int  color_count = target->colorCount();
    const bool has_color   = color_count > 0;
    const bool has_depth   = target->hasDepth();

    std::vector<VkFormat> color_formats;
    color_formats.reserve(static_cast<std::size_t>(color_count));
    for (int i = 0; i < color_count; ++i) {
        color_formats.push_back(toColorFormat(target->colorFormat(i)));
    }

    ::vsg::ImageViews attachments;
    attachments.reserve(static_cast<std::size_t>(color_count) + (has_depth ? 1u : 0u));
    // One colour image + view per attachment: each is written by fragment
    // output location i, then usable as a sampled texture on its own
    // (VK_IMAGE_USAGE_SAMPLED_BIT) or as a blit source for compositing.
    t.color_images.resize(static_cast<std::size_t>(color_count));
    t.color_views.resize(static_cast<std::size_t>(color_count));
    for (int i = 0; i < color_count; ++i) {
        auto color           = ::vsg::Image::create();
        color->imageType     = VK_IMAGE_TYPE_2D;
        color->format        = color_formats[static_cast<std::size_t>(i)];
        color->extent        = VkExtent3D{ w, h, 1 };
        color->mipLevels     = 1;
        color->arrayLayers   = 1;
        color->tiling        = VK_IMAGE_TILING_OPTIMAL;
        color->usage         = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        color->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        t.color_images[static_cast<std::size_t>(i)] = color;
        // createImageView compiles the Image (creates VkImage + allocates/binds
        // device memory) and creates+compiles the ImageView (VkImageView).
        // WITHOUT this the VkImage/VkImageView stay VK_NULL_HANDLE and the
        // Framebuffer holds a corrupt handle -> vkCmdBeginRenderPass crashes.
        t.color_views[static_cast<std::size_t>(i)] = ::vsg::createImageView(device.get(), color, VK_IMAGE_ASPECT_COLOR_BIT);
        attachments.push_back(t.color_views[static_cast<std::size_t>(i)]);
    }
    if (has_depth) {
        auto depth           = ::vsg::Image::create();
        depth->imageType     = VK_IMAGE_TYPE_2D;
        depth->format        = toDepthFormat(target->depthFormat());
        depth->extent        = VkExtent3D{ w, h, 1 };
        depth->mipLevels     = 1;
        depth->arrayLayers   = 1;
        depth->tiling        = VK_IMAGE_TILING_OPTIMAL;
        depth->usage         = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        depth->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        t.depth_image        = depth;
        t.depth_view         = ::vsg::createImageView(device.get(), depth, VK_IMAGE_ASPECT_DEPTH_BIT);
        attachments.push_back(t.depth_view);
    }

    t.render_pass = has_color ? makeSampleableRenderPass(device.get(),
                                                         color_formats,
                                                         has_depth ? toDepthFormat(target->depthFormat()) : VK_FORMAT_UNDEFINED)
                              : makeDepthOnlyRenderPass(device.get(), toDepthFormat(target->depthFormat()));
    t.framebuffer = ::vsg::Framebuffer::create(t.render_pass, attachments, w, h, 1);

    t.graph              = ::vsg::RenderGraph::create();
    t.graph->framebuffer = t.framebuffer;
    t.graph->renderArea  = VkRect2D{
        { 0, 0 },
        { w, h }
    };
    t.graph->contents      = VK_SUBPASS_CONTENTS_INLINE;
    t.graph->viewportState = ::vsg::ViewportState::create(VkExtent2D{ w, h });
    // Clear values match the attachment order (colour attachments in order,
    // then depth). Depth is cleared to the far plane for depth-only (shadow)
    // targets and to the previously proven value for colour+RT targets.
    // Off-screen targets keep this fixed clear (the engine clear() targets
    // the window graph — see clear()). The first colour attachment keeps the
    // legacy grey; extra MRT attachments clear transparent black (empty
    // regions stay black until a fragment writes them).
    t.graph->clearValues.clear();
    for (int i = 0; i < color_count; ++i) {
        VkClearValue color_clear = {};
        if (i == 0) {
            color_clear.color = VkClearColorValue{
                { 0.2f, 0.2f, 0.2f, 1.0f }
            };
        }
        t.graph->clearValues.push_back(color_clear);
    }
    if (has_depth) {
        VkClearValue depth_clear = {};
        depth_clear.depthStencil = VkClearDepthStencilValue{ has_color ? 0.0f : 1.0f, 0 };
        t.graph->clearValues.push_back(depth_clear);
    }

    if (impl->command_graph != nullptr) {
        // Record off-screen graphs BEFORE the main window graph so the colour
        // texture they produce is current when a later pass samples it in the
        // same frame.
        impl->command_graph->children.insert(impl->command_graph->children.begin(), t.graph);
    }
    std::fprintf(stderr, "[VsgRenderer] EXPERIMENTAL off-screen target %ux%u attached\n", w, h);
    // NOTE: no compile here — the graph is empty until its first content slot
    // is added; setupContentSlot() compiles the (whole) command graph then.
}

void VsgRenderer::drawScreenTexture(vine::graphics::RenderTarget* source, int attachment)
{
    if (!impl->initialized || impl->viewer == nullptr || impl->window == nullptr || source == nullptr) {
        return;
    }

    // Consume the sub-viewport queued by setViewport() (the ScreenPass's PiP
    // rectangle); mirrors how render() consumes one for overlays.
    const bool has_vp          = impl->has_pending_viewport;
    const int  vp_x            = impl->pending_viewport[0];
    const int  vp_y            = impl->pending_viewport[1];
    const int  vp_w            = impl->pending_viewport[2];
    const int  vp_h            = impl->pending_viewport[3];
    impl->has_pending_viewport = false;

    auto src_it = impl->targets.find(source);
    if (src_it == impl->targets.end() || src_it->second.color_views.empty()) {
        std::fprintf(stderr, "[VsgRenderer] drawScreenTexture: source target has no colour attachment\n");
        return;
    }
    const auto& src = src_it->second;
    if (src.width <= 0 || src.height <= 0) {
        return;
    }
    // Select the colour attachment to sample (MRT / G-buffer targets expose
    // several sampleable textures under one target). Out-of-range indexes
    // clamp to the last attachment so a mis-set consumer still draws.
    std::size_t attachment_index = 0;
    if (attachment > 0) {
        attachment_index = static_cast<std::size_t>(attachment);
        if (attachment_index >= src.color_views.size()) {
            attachment_index = src.color_views.size() - 1;
        }
    }
    const auto source_view = src.color_views[attachment_index];

    // PiP (screen) views are children of the single window render graph, so
    // their slots live in the window entry (nullptr key) of the target table.
    // Each slot is keyed by (sampled target, attachment): two PiPs may sample
    // different attachments of the same MRT target.
    auto& window_entry = impl->targets[nullptr];
    const Impl::ScreenKey key{ source, static_cast<int>(attachment_index) };

    // Drop a stale slot when the sampled target was resized (its colour view
    // was rebuilt, so the old descriptor would sample a destroyed image).
    {
        const auto old = window_entry.screen_slots.find(key);
        if (old != window_entry.screen_slots.end() && old->second.ready && (old->second.source_w != src.width || old->second.source_h != src.height)) {
            if (window_entry.graph != nullptr) {
                auto& children = window_entry.graph->children;
                children.erase(std::remove_if(children.begin(),
                                              children.end(),
                                              [&old](const ::vsg::ref_ptr<::vsg::Node>& child) { return child.get() == old->second.view.get(); }),
                               children.end());
            }
            window_entry.screen_slots.erase(old);
        }
    }

    auto& slot = window_entry.screen_slots[key];

    // Destination rectangle: the pass's sub-viewport, else the full surface.
    const auto surface = impl->window->extent2D();
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
        rect_x           = surf_w - w - margin;
        rect_y           = surf_h - h - margin;
        rect_w           = w;
        rect_h           = h;
    }

    if (!slot.ready) {
        slot.source_w    = src.width;
        slot.source_h    = src.height;
        slot.source_view = source_view;

        // Full-screen textured triangle sampling the off-screen colour
        // attachment, drawn as a second View of the main window render graph
        // (like overlays) so the sub-viewport clips the picture-in-picture
        // rectangle.
        auto content = makeScreenTextureNode(source_view, surface);
        if (content == nullptr) {
            window_entry.screen_slots.erase(key);
            return;
        }
        auto camera           = ::vsg::Camera::create();
        camera->viewportState = ::vsg::ViewportState::create(rect_x, rect_y, static_cast<uint32_t>(rect_w), static_cast<uint32_t>(rect_h));
        slot.camera           = camera;
        auto view             = ::vsg::View::create(camera);
        auto group            = ::vsg::Group::create();
        group->addChild(content);
        view->addChild(group);
        slot.view = view;
        if (window_entry.graph != nullptr) {
            window_entry.graph->addChild(view);
        }
        // Compile the new view (its pipeline is built against the window
        // render pass) before it is first recorded.
        const auto compileResult = impl->viewer->compile();
        if (!compileResult) {
            std::fprintf(stderr, "[VsgRenderer] screen pass compile failed: %s\n", compileResult.message.c_str());
            // Drop the half-compiled view so it is never recorded.
            if (window_entry.graph != nullptr) {
                auto& children = window_entry.graph->children;
                children.erase(
                    std::remove_if(children.begin(), children.end(), [&view](const ::vsg::ref_ptr<::vsg::Node>& child) { return child.get() == view.get(); }),
                    children.end());
            }
            window_entry.screen_slots.erase(key);
            return;
        }
        slot.ready         = true;
        impl->needs_submit = true;
        std::fprintf(stderr, "[VsgRenderer] EXPERIMENTAL screen PiP %dx%d (att %zu) -> %d,%d %dx%d attached\n", src.width, src.height, attachment_index, rect_x, rect_y, rect_w, rect_h);
    }

    // Follow the requested sub-viewport each frame (dynamic viewport + scissor).
    slot.camera->viewportState = ::vsg::ViewportState::create(rect_x, rect_y, static_cast<uint32_t>(rect_w), static_cast<uint32_t>(rect_h));
    impl->needs_submit         = true;
}

namespace
{

/**
 * @brief Computes the world -> view rotation basis for a look-at camera.
 *
 * @param camera Vine camera (eye / target / up).
 * @param r      Receives the view-space X axis in world coords (right).
 * @param u      Receives the view-space Y axis in world coords (up).
 * @param f      Receives the view-space -Z axis in world coords (forward).
 */
void viewRotation(const vine::graphics::Camera* camera, double r[3], double u[3], double f[3])
{
    const auto eye    = camera->eye();
    const auto center = camera->target();
    const auto up_vec = camera->up();
    double fx = center.x - eye.x;
    double fy = center.y - eye.y;
    double fz = center.z - eye.z;
    const double fl = std::sqrt(fx * fx + fy * fy + fz * fz);
    if (fl > 1e-12) {
        fx /= fl;
        fy /= fl;
        fz /= fl;
    }
    else {
        fx = 0.0;
        fy = 0.0;
        fz = -1.0;
    }
    // r = normalize(f x up), u = r x f.
    double rx = fy * up_vec.z - fz * up_vec.y;
    double ry = fz * up_vec.x - fx * up_vec.z;
    double rz = fx * up_vec.y - fy * up_vec.x;
    const double rl = std::sqrt(rx * rx + ry * ry + rz * rz);
    if (rl > 1e-12) {
        rx /= rl;
        ry /= rl;
        rz /= rl;
    }
    else {
        rx = 1.0;
        ry = 0.0;
        rz = 0.0;
    }
    const double ux = ry * fz - rz * fy;
    const double uy = rz * fx - rx * fz;
    const double uz = rx * fy - ry * fx;
    r[0] = rx;
    r[1] = ry;
    r[2] = rz;
    u[0] = ux;
    u[1] = uy;
    u[2] = uz;
    f[0] = fx;
    f[1] = fy;
    f[2] = fz;
}

/**
 * @brief Fills a fullscreen light push block for a deferred-lighting pass.
 *
 * The G-buffer stores view-space normals / positions, so directional lights
 * are pre-transformed from world to view space on the CPU (the fragment
 * shader then never needs a view matrix). Supports the first ambient plus up
 * to two directional lights — enough for the current demo rig; further lights
 * are ignored (documented S2a limitation).
 *
 * @param camera Camera whose view transforms the lights (may be null).
 * @param lights Scene lights to bake (borrowed).
 * @param block  Receives the packed block (zeroed first).
 */
void fillLightPushBlock(const vine::graphics::Camera*                               camera,
                        const std::vector<const vine::graphics::Light*>&            lights,
                        LightPushBlock&                                             block)
{
    block = LightPushBlock{};
    block.misc[0] = 32.0f; // fixed specular shininess approximation (S2a)
    if (camera == nullptr) {
        return;
    }
    double r[3] = {}, u[3] = {}, f[3] = {};
    viewRotation(camera, r, u, f);
    int dirlight = 0;
    for (const auto* light : lights) {
        if (light == nullptr || !light->isEnabled()) {
            continue;
        }
        const auto c = light->color();
        switch (light->type()) {
        case vine::graphics::LightType::Ambient:
            block.ambient[0] = c.r;
            block.ambient[1] = c.g;
            block.ambient[2] = c.b;
            block.ambient[3] = light->intensity();
            break;
        case vine::graphics::LightType::Directional:
            if (dirlight >= 2) {
                break;
            }
            {
                const auto d = light->direction();
                // world -> view direction (rotation only): rows r, u, -f.
                double vx = r[0] * d.x + r[1] * d.y + r[2] * d.z;
                double vy = u[0] * d.x + u[1] * d.y + u[2] * d.z;
                double vz = -f[0] * d.x - f[1] * d.y - f[2] * d.z;
                const double vl = std::sqrt(vx * vx + vy * vy + vz * vz);
                if (vl > 1e-9) {
                    vx /= vl;
                    vy /= vl;
                    vz /= vl;
                }
                float* dd = (dirlight == 0) ? block.dir0 : block.dir1;
                float* cc = (dirlight == 0) ? block.col0 : block.col1;
                dd[0] = static_cast<float>(vx);
                dd[1] = static_cast<float>(vy);
                dd[2] = static_cast<float>(vz);
                dd[3] = 0.0f;
                cc[0] = c.r;
                cc[1] = c.g;
                cc[2] = c.b;
                cc[3] = light->intensity();
                ++dirlight;
            }
            break;
        default:
            break;
        }
    }
}

} // namespace

void VsgRenderer::drawScreenProgram(vine::graphics::RenderTarget*              source,
                                    vine::raw_ptr<const vine::graphics::ShaderProgram> program,
                                    vine::raw_ptr<const vine::graphics::Camera>        camera)
{
    if (!impl->initialized || impl->viewer == nullptr || impl->window == nullptr || source == nullptr || program == nullptr) {
        return;
    }

    // Consume the sub-viewport queued by setViewport() (the pass's rectangle).
    const bool has_vp          = impl->has_pending_viewport;
    const int  vp_x            = impl->pending_viewport[0];
    const int  vp_y            = impl->pending_viewport[1];
    const int  vp_w            = impl->pending_viewport[2];
    const int  vp_h            = impl->pending_viewport[3];
    impl->has_pending_viewport = false;

    auto src_it = impl->targets.find(source);
    if (src_it == impl->targets.end() || src_it->second.color_views.size() < 3u) {
        std::fprintf(stderr, "[VsgRenderer] drawScreenProgram: source target needs >= 3 colour attachments\n");
        return;
    }
    const auto& src = src_it->second;
    if (src.width <= 0 || src.height <= 0) {
        return;
    }

    // Fullscreen-program slots are drawn under the window graph (nullptr key).
    auto& window_entry = impl->targets[nullptr];
    auto& slot         = window_entry.program_slots[source];

    // Destination rectangle: the pass's sub-viewport, else the full surface
    // (clamped into the surface — the fullscreen draw has no auto-fit).
    const auto surface = impl->window->extent2D();
    const int  surf_w  = static_cast<int>(surface.width);
    const int  surf_h  = static_cast<int>(surface.height);
    int rect_x = 0, rect_y = 0, rect_w = surf_w, rect_h = surf_h;
    if (has_vp && vp_w > 0 && vp_h > 0) {
        rect_x = vp_x;
        rect_y = vp_y;
        rect_w = vp_w;
        rect_h = vp_h;
    }
    if (rect_x < 0) {
        rect_w += rect_x;
        rect_x = 0;
    }
    if (rect_y < 0) {
        rect_h += rect_y;
        rect_y = 0;
    }
    if (rect_w <= 0 || rect_h <= 0) {
        return;
    }
    if (rect_x + rect_w > surf_w) {
        rect_w = surf_w - rect_x;
    }
    if (rect_y + rect_h > surf_h) {
        rect_h = surf_h - rect_y;
    }
    if (rect_w <= 0 || rect_h <= 0) {
        return;
    }

    // (Re)build the retained slot when it is missing, the source was resized
    // (its colour views were rebuilt), or the program changed.
    const bool stale = !slot.ready || slot.source_w != src.width || slot.source_h != src.height || slot.program != program;
    if (stale) {
        if (slot.view != nullptr && window_entry.graph != nullptr) {
            auto& children = window_entry.graph->children;
            children.erase(std::remove_if(children.begin(),
                                          children.end(),
                                          [&slot](const ::vsg::ref_ptr<::vsg::Node>& child) { return child.get() == slot.view.get(); }),
                           children.end());
        }
        slot = Impl::ProgramSlot{};
        slot.push_data = ::vsg::ubyteArray::create(128); // range; block uses the first 96
        auto node      = makeFullscreenProgramNode(program, src.color_views, surface, slot.push_data);
        if (node == nullptr) {
            window_entry.program_slots.erase(source);
            return;
        }
        slot.source_w = src.width;
        slot.source_h = src.height;
        slot.program  = const_cast<vine::graphics::ShaderProgram*>(program);
        slot.node     = node;

        auto camera_vsg           = ::vsg::Camera::create();
        camera_vsg->viewportState = ::vsg::ViewportState::create(rect_x, rect_y, static_cast<uint32_t>(rect_w), static_cast<uint32_t>(rect_h));
        slot.camera               = camera_vsg;
        auto view                 = ::vsg::View::create(camera_vsg);
        auto group                = ::vsg::Group::create();
        group->addChild(node);
        view->addChild(group);
        slot.view = view;
        if (window_entry.graph != nullptr) {
            // The fullscreen program view is the window's main content in
            // deferred mode: draw it FIRST (front), so content slots created
            // later (HUD overlays) stack above it (see setupContentSlot's
            // INT_MIN ordering for program views).
            window_entry.graph->children.insert(window_entry.graph->children.begin(), view);
        }
        const auto compileResult = impl->viewer->compile();
        if (!compileResult) {
            std::fprintf(stderr, "[VsgRenderer] fullscreen program compile failed: %s\n", compileResult.message.c_str());
            if (window_entry.graph != nullptr) {
                auto& children = window_entry.graph->children;
                children.erase(
                    std::remove_if(children.begin(), children.end(), [&view](const ::vsg::ref_ptr<::vsg::Node>& child) { return child.get() == view.get(); }),
                    children.end());
            }
            window_entry.program_slots.erase(source);
            return;
        }
        slot.ready         = true;
        impl->needs_submit = true;
        std::fprintf(stderr, "[VsgRenderer] EXPERIMENTAL deferred fullscreen program %dx%d -> %d,%d %dx%d attached\n", src.width, src.height, rect_x, rect_y, rect_w, rect_h);
    }

    // Consume the lights queued by setLights() (from the pass's content scene)
    // and push view-space light parameters each frame before record.
    std::vector<const vine::graphics::Light*> lights = std::move(impl->pending_lights);
    impl->pending_lights.clear();
    LightPushBlock block{};
    fillLightPushBlock(camera, lights, block);
    if (slot.push_data != nullptr && slot.push_data->dataSize() >= sizeof(block)) {
        std::memcpy(slot.push_data->dataPointer(), &block, sizeof(block));
    }

    // Follow the requested sub-viewport each frame.
    slot.camera->viewportState = ::vsg::ViewportState::create(rect_x, rect_y, static_cast<uint32_t>(rect_w), static_cast<uint32_t>(rect_h));
    impl->needs_submit         = true;
}

void VsgRenderer::releaseWindowLayer(vine::raw_ptr<const vine::graphics::Camera> camera, int order)
{
    if (camera == nullptr || !impl->initialized) {
        return;
    }
    // Window content slots live in the window target (nullptr key) of the
    // output-target table, keyed by (camera, explicit pass order). Off-screen
    // slots are released together with their whole target (releaseRenderTarget).
    auto& t  = impl->targets[nullptr];
    auto  it = t.content_slots.find(Impl::ContentKey{ const_cast<vine::graphics::Camera*>(camera), order });
    if (it == t.content_slots.end()) {
        return;
    }
    // Detach the slot's View from the window render graph so it is no longer
    // recorded each frame, then drop it (releases its compiled pipelines and
    // the per-slot bridge cache). Slot removal is rare, so a device wait
    // before the drop keeps the release safe against an in-flight frame.
    if (t.graph != nullptr && it->second.view != nullptr) {
        auto& children = t.graph->children;
        children.erase(
            std::remove_if(children.begin(), children.end(), [&it](const ::vsg::ref_ptr<::vsg::Node>& child) { return child.get() == it->second.view.get(); }),
            children.end());
    }
    if (impl->viewer != nullptr) {
        impl->viewer->deviceWaitIdle();
    }
    it->second.bridge.clearCache();
    t.content_slots.erase(it);
}

void VsgRenderer::releaseRenderTarget(vine::graphics::RenderTarget* target)
{
    if (target == nullptr || !impl->initialized) {
        return;
    }
    bool released = false;
    auto ot       = impl->targets.find(target);
    if (ot != impl->targets.end()) {
        // Remove the target's off-screen graph from the command graph before
        // dropping its images / views / render pass / framebuffer / slots.
        auto& t = ot->second;
        if (impl->command_graph != nullptr && t.graph != nullptr) {
            auto& children = impl->command_graph->children;
            children.erase(
                std::remove_if(children.begin(), children.end(), [&t](const ::vsg::ref_ptr<::vsg::Node>& child) { return child.get() == t.graph.get(); }),
                children.end());
        }
        if (impl->viewer != nullptr) {
            impl->viewer->deviceWaitIdle();
        }
        for (auto& slot_entry : t.content_slots) {
            slot_entry.second.bridge.clearCache();
        }
        // Drop the target's whole table entry (its off-screen attachments /
        // render graph / content slots). Any PiP slot sampling it lives in
        // another target's screen_slots and is removed right below.
        impl->targets.erase(ot);
        released = true;
    }
    // PiP (screen) slots live in the target that draws them (the window
    // target today); drop any that sample this target's colour (any of its
    // colour attachments).
    for (auto& target_entry : impl->targets) {
        auto& slots = target_entry.second.screen_slots;
        for (auto it = slots.begin(); it != slots.end();) {
            if (it->first.target != target) {
                ++it;
                continue;
            }
            if (it->second.view != nullptr && target_entry.second.graph != nullptr) {
                auto& children = target_entry.second.graph->children;
                children.erase(std::remove_if(children.begin(),
                                              children.end(),
                                              [&it](const ::vsg::ref_ptr<::vsg::Node>& child) { return child.get() == it->second.view.get(); }),
                               children.end());
            }
            if (impl->viewer != nullptr) {
                impl->viewer->deviceWaitIdle();
            }
            it = slots.erase(it);
            released = true;
        }
    }
    // Fullscreen-program slots (deferred lighting) live under the drawing
    // target (the window today); drop any sampling the removed target.
    for (auto& target_entry : impl->targets) {
        auto& slots = target_entry.second.program_slots;
        auto  it    = slots.find(target);
        if (it == slots.end()) {
            continue;
        }
        if (it->second.view != nullptr && target_entry.second.graph != nullptr) {
            auto& children = target_entry.second.graph->children;
            children.erase(std::remove_if(children.begin(),
                                          children.end(),
                                          [&it](const ::vsg::ref_ptr<::vsg::Node>& child) { return child.get() == it->second.view.get(); }),
                           children.end());
        }
        if (impl->viewer != nullptr) {
            impl->viewer->deviceWaitIdle();
        }
        slots.erase(it);
        released = true;
    }
    if (released) {
        std::fprintf(stderr, "[VsgRenderer] released GPU resources for removed render target\n");
    }
}

void VsgRenderer::setupContentSlot(vine::graphics::RenderTarget* target, vine::graphics::Camera* cam, int order, bool on_top)
{
    // Content slots are retained Views under the TARGET's render graph — the
    // window target (target == nullptr) and every off-screen target share
    // this one mechanism. Each (camera, pass order) pair is its own View +
    // bridge, so several passes sharing one camera can stack under one target.
    auto& t          = impl->targets[target];
    auto& content    = t.content_slots[Impl::ContentKey{ cam, order }];
    if (content.ready) {
        return;
    }
    if (t.graph == nullptr) {
        // No graph yet (e.g. an off-screen target that failed to build): drop
        // the half-made slot.
        t.content_slots.erase(Impl::ContentKey{ cam, order });
        return;
    }
    content.order      = order;
    content.on_top     = on_top;
    content.vsg_camera = impl->cameraBridge.create(cam);
    if (content.vsg_camera == nullptr) {
        t.content_slots.erase(Impl::ContentKey{ cam, order });
        return;
    }
    content.root = ::vsg::Group::create();

    // Per-slot pipeline bridge. vsg compiles pipelines per viewID, so every
    // content slot keeps its own SceneBridge (sharing already-compiled
    // pipelines across views crashes GraphicsPipeline::vk()); the bridge's
    // shader set bakes the slot's depth policy.
    if (target == nullptr) {
        // Window slots share the renderer's (window-sized) shader sets.
        content.bridge.setShaderSet(on_top ? impl->depth_off_shader_set : impl->depth_on_shader_set);
    }
    else {
        // Off-screen slots get a per-target shader set baked at the target's
        // size (created lazily).
        auto& set_ref = on_top ? t.depth_off_shader_set : t.depth_on_shader_set;
        if (set_ref == nullptr) {
            set_ref = buildShaderSet(impl->shader_preset,
                                     VkExtent2D{ static_cast<uint32_t>(t.width), static_cast<uint32_t>(t.height) },
                                     !on_top,
                                     target->colorCount());
        }
        content.bridge.setShaderSet(set_ref);
    }
    content.bridge.setMaterialManager(&impl->materialManager);
    content.bridge.clearCache();

    // Seed the slot's default light before the first compile. On-top (HUD)
    // slots get a single ambient light: with a directional headlight the axis
    // went dark/black from diagonal views because the light direction is fixed
    // while the slot camera (which mirrors the source) rotates. Ambient-only
    // lighting makes phong's colour independent of surface orientation, giving
    // flat sticks. The window's main slot gets vsg's default headlight; an
    // off-screen main slot keeps its ambient fill (matches the pre-
    // unification off-screen view).
    content.light_group = ::vsg::Group::create();
    if (on_top) {
        auto ambient  = ::vsg::AmbientLight::create();
        ambient->name = "content_ambient";
        ambient->color.set(1.0f, 1.0f, 1.0f);
        ambient->intensity = 1.0f;
        content.light_group->addChild(ambient);
    }
    else if (target == nullptr) {
        // Seed the window's main slot with vsg's default headlight. Content
        // lights (setLights) replace the group's children at render time each
        // frame (cheap: light nodes are collected into the lightData uniform
        // at record time, no recompile), so the seed only needs to yield a
        // valid light for the first compile.
        content.light_group->addChild(::vsg::createHeadlight());
    }
    else {
        auto ambient  = ::vsg::AmbientLight::create();
        ambient->name = "offscreen_ambient";
        ambient->color.set(1.0f, 1.0f, 1.0f);
        ambient->intensity = 1.0f;
        content.light_group->addChild(ambient);
    }

    content.view = ::vsg::View::create(content.vsg_camera);
    content.view->addChild(content.light_group);
    content.view->addChild(content.root);

    // Append the slot as another View of the target's render graph, then
    // compile it before it is first recorded. Content slots are only created
    // from render() calls that follow initialize(), so the target's graph is
    // always present here.
    //
    // Within a target the slot views are stacked in ASCENDING pass order —
    // the order the caller gave addPass() and the engine already runs passes
    // in. The slot stores that explicit @p order (announced via
    // RenderBackend::setPassOrder) and is inserted so the children stay
    // sorted by it: stable for equal orders (registration order), and Views
    // that are not content slots (PiP screen views, always drawn last) are
    // treated as the highest order. No main/on-top semantic constrains the
    // order — a pass positioned by the user at any order draws exactly there.
    // Ordering by the explicit value also keeps the pre-frame warm-up safe:
    // warm-up may create a higher-order (on-top) slot before a lower-order
    // (main) slot has run, but the main slot is inserted ahead of it by its
    // smaller order when it is finally created.
    {
        auto& children  = t.graph->children;
        auto  insert_at = children.end();
        for (auto it = children.begin(); it != children.end(); ++it) {
            // Order of an already-placed child: content slots carry their own
            // explicit order; fullscreen-program views (deferred lighting,
            // the window's main content in deferred mode) are drawn FIRST
            // (INT_MIN) so later HUD content slots stack above them; any other
            // view (a PiP screen view, always drawn last) is INT_MAX.
            int other_order = std::numeric_limits<int>::max();
            for (const auto& kv : t.content_slots) {
                if (kv.second.ready && kv.second.view == *it) {
                    other_order = kv.second.order;
                    break;
                }
            }
            if (other_order == std::numeric_limits<int>::max()) {
                for (const auto& kv : t.program_slots) {
                    if (kv.second.ready && kv.second.view == *it) {
                        other_order = std::numeric_limits<int>::min();
                        break;
                    }
                }
            }
            if (other_order > order) {
                insert_at = it; // first child with a strictly higher order
                break;
            }
        }
        if (insert_at != children.end()) {
            children.insert(insert_at, content.view);
        }
        else {
            children.push_back(content.view);
        }
    }
    content.ready = true;
    if (impl->viewer != nullptr) {
        impl->viewer->compile();
    }
}

void VsgRenderer::renderContentSlot(vine::graphics::RenderTarget*                     target,
                                    const std::vector<vine::graphics::RenderCommand>& commands,
                                    vine::raw_ptr<const vine::graphics::Camera>       camera,
                                    const std::vector<const vine::graphics::Light*>&  lights,
                                    bool                                              on_top,
                                    int                                               order,
                                    int                                               vp_x,
                                    int                                               vp_y,
                                    int                                               vp_w,
                                    int                                               vp_h)
{
    auto* cam   = const_cast<vine::graphics::Camera*>(camera);
    auto& t     = impl->targets[target];
    auto  it    = t.content_slots.find(Impl::ContentKey{ cam, order });
    if (it == t.content_slots.end() || !it->second.ready) {
        setupContentSlot(target, cam, order, on_top);
        it = t.content_slots.find(Impl::ContentKey{ cam, order });
    }
    if (it == t.content_slots.end() || !it->second.ready) {
        return; // slot could not be built (e.g. camera bridge failed)
    }
    auto& content = it->second;

    // Full target extent for this slot's viewport: the live swapchain size
    // for the window target, the off-screen target's logical size otherwise.
    const int surf_w = (target == nullptr) ? static_cast<int>(impl->window->extent2D().width) : t.width;
    const int surf_h = (target == nullptr) ? static_cast<int>(impl->window->extent2D().height) : t.height;

    // Keep the slot's vsg camera viewport in step with its role each frame:
    // on-top (HUD) slots carry their sub-viewport (zero size = full target);
    // main (scene) slots always fill the whole target — the slot is created
    // lazily on its first render, so this also covers the first frame and any
    // resize that happened before the slot existed.
    if (content.on_top) {
        if (vp_w <= 0 || vp_h <= 0) {
            vp_w = surf_w;
            vp_h = surf_h;
        }
        content.vsg_camera->viewportState = ::vsg::ViewportState::create(vp_x, vp_y, static_cast<uint32_t>(vp_w), static_cast<uint32_t>(vp_h));
    }
    else {
        content.vsg_camera->viewportState = ::vsg::ViewportState::create(VkExtent2D{ static_cast<uint32_t>(surf_w), static_cast<uint32_t>(surf_h) });
    }

    impl->cameraBridge.apply(cam, content.vsg_camera);

    // Content lights: replace the slot's default light each frame; on-top
    // (HUD) slots keep their seeded ambient light.
    if (!content.on_top) {
        setGroupLights(content.light_group.get(), lights);
    }

    // The command stream is the source of truth: reconcile the retained slot
    // root against it (in-place for moves/material edits). The legacy own-
    // window debug path skips syncing the window's main slot.
    if (!(target == nullptr && forceOwnWindow() && !content.on_top)) {
        std::vector<::vsg::ref_ptr<::vsg::Node>> created;
        content.bridge.syncRenderCommands(commands, content.root.get(), &created);
        if (!created.empty()) {
            // A full-graph compile keeps newly built/rebuild subtrees correct
            // (see design doc §9 Phase 3 for the incremental plan).
            const auto compileResult = impl->viewer->compile();
            if (!compileResult) {
                std::fprintf(stderr, "[VsgRenderer] content slot compile failed: %s\n", compileResult.message.c_str());
            }
        }
    }
}

void VsgRenderer::setViewport(int x, int y, int width, int height)
{
    impl->pending_viewport[0]  = x;
    impl->pending_viewport[1]  = y;
    impl->pending_viewport[2]  = width;
    impl->pending_viewport[3]  = height;
    impl->has_pending_viewport = true;
}

void VsgRenderer::setPassOrder(int order)
{
    // Queue the pass's explicit pipeline order for the next render() call: the
    // engine announces each pass's addPass() order before it executes, so the
    // target's content slots can be keyed and stacked by that order in
    // setupContentSlot().
    impl->pending_pass_order = order;
}

void VsgRenderer::submitFrame()
{
    if (!impl->initialized || impl->viewer == nullptr || !impl->needs_submit) {
        return;
    }
    impl->needs_submit = false;
    impl->viewer->recordAndSubmit();
    impl->viewer->present();
}

void VsgRenderer::clear(const vine::Color& backgroundColor, bool clearDepth)
{
    (void)clearDepth;
    // A clear marks the next render() as main (depth-on) content; a render
    // without a preceding clear is styled on-top (HUD, depth-off). The style
    // is consumed in render(); the clear colour is pushed to the window graph
    // below so the pass clear state reaches the GPU clear.
    impl->main_pending = true;
    auto& window_target = impl->targets[nullptr];
    if (window_target.graph != nullptr) {
        // The window render graph captured the clear colour when it was
        // created; push the requested colour through so the pass clear state
        // actually reaches the GPU clear. The colour components are floats in
        // [0,1]; the Vine Color stores them as 0-255 bytes.
        const VkClearColorValue clear_value{
            { backgroundColor.r / 255.0f, backgroundColor.g / 255.0f, backgroundColor.b / 255.0f, backgroundColor.a / 255.0f }
        };
        window_target.graph->setClearValues(clear_value, VkClearDepthStencilValue{ 0.0f, 0 });
    }
}

void VsgRenderer::swapBuffers()
{
    // One record+submit+present per frame, after all passes were synced.
    submitFrame();
}

vine::raw_ptr<vine::graphics::MaterialManager> VsgRenderer::materialManager()
{
    return &impl->materialManager;
}

void VsgRenderer::setShaderPreset(vine::graphics::ShaderPreset preset)
{
    impl->shader_preset = preset;
}

void VsgRenderer::setWindowHandle(void* native_handle)
{
    impl->bound_handle = native_handle;
}

void VsgRenderer::resize(int width, int height)
{
    (void)width;
    (void)height;
    if (impl->window != nullptr) {
        impl->window->resize();
    }
    // Every window main (full-target) content slot's camera viewport follows
    // the live window size so the render graph's render area tracks a resize
    // (renderContentSlot also re-derives each slot's viewport every frame;
    // refreshing here keeps slots correct even before their next render).
    // On-top (HUD) slots keep their own sub-viewport, re-set per frame by
    // their pass.
    auto& window_target = impl->targets[nullptr];
    if (impl->window == nullptr) {
        return;
    }
    const auto extent = impl->window->extent2D();
    for (auto& kv : window_target.content_slots) {
        auto& slot = kv.second;
        if (slot.ready && slot.vsg_camera != nullptr && !slot.on_top) {
            slot.vsg_camera->viewportState = ::vsg::ViewportState::create(extent);
        }
    }
}

void* VsgRenderer::nativeHandle() const
{
    return impl->bound_handle;
}

void VsgRenderer::frame()
{
    if (!impl->initialized || impl->viewer == nullptr) {
        return;
    }
    // VSG frame order: advance -> handleEvents -> update -> record -> present.
    // No Vine content is bound to the renderer: the engine drives content per
    // pass, so this convenience hook only presents whatever the passes synced
    // (submitFrame() skips when nothing was rendered this frame).
    beginFrame();
    endFrame();
    swapBuffers();
}

::vsg::ref_ptr<::vsg::Viewer> VsgRenderer::viewer() const
{
    return impl->viewer;
}

V_VSG_NS_END
