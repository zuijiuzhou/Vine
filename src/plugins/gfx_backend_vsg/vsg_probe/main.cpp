// Standalone minimal vsg probe: renders a raw phong triangle with a red
// material using the EXACT same construction as Vine's
// VsgRenderer::makeRawDemoNode, but inside a vanilla vsg app (no Vine code).
// If this renders RED, Vine's VsgRenderer setup is at fault; if it also shows
// the fixed blue (35,116,208), the vendored vsg phong path itself is broken.

#include <vsg/vk/Instance.h>
#include <vsg/vk/PhysicalDevice.h>
#include <vsg/app/CommandGraph.h>
#include <vsg/app/RenderGraph.h>
#include <vsg/app/Viewer.h>
#include <vsg/commands/BindIndexBuffer.h>
#include <vsg/commands/BindVertexBuffers.h>
#include <vsg/commands/Commands.h>
#include <vsg/commands/DrawIndexed.h>
#include <vsg/nodes/StateGroup.h>
#include <vsg/nodes/VertexIndexDraw.h>
#include <vsg/lighting/AmbientLight.h>
#include <vsg/lighting/DirectionalLight.h>
#include <vsg/lighting/HardShadows.h>
#include <vsg/nodes/Group.h>
#include <vsg/state/ColorBlendState.h>
#include <vsg/state/DepthStencilState.h>
#include <vsg/state/InputAssemblyState.h>
#include <vsg/state/MultisampleState.h>
#include <vsg/state/RasterizationState.h>
#include <vsg/state/material.h>
#include <vsg/state/ViewportState.h>
#include <vsg/io/Options.h>
#include <vsg/utils/Builder.h>
#include <vsg/utils/ComputeBounds.h>
#include <vsg/utils/GraphicsPipelineConfigurator.h>
#include <vsg/utils/ShaderSet.h>

#include <cstdlib>
#include <exception>
#include <iostream>

vsg::ref_ptr<vsg::Node> makeRawDemoNode()
{
    auto shaderSet = vsg::createPhongShaderSet();
    auto raster = vsg::RasterizationState::create();
    raster->cullMode = VK_CULL_MODE_NONE;  // triangle winding may be CW; don't cull
    shaderSet->defaultGraphicsPipelineStates = vsg::GraphicsPipelineStates{
        vsg::DepthStencilState::create(),
        raster,
        vsg::ColorBlendState::create(),
        vsg::InputAssemblyState::create(),
        vsg::MultisampleState::create(),
        vsg::ViewportState::create(VkExtent2D{ 800, 600 }),
    };

    auto config = vsg::GraphicsPipelineConfigurator::create(shaderSet);

    auto vertices = vsg::vec3Array::create(3);
    (*vertices)[0] = vsg::vec3(-1.0f, -1.0f, 0.0f);
    (*vertices)[1] = vsg::vec3(1.0f, -1.0f, 0.0f);
    (*vertices)[2] = vsg::vec3(0.0f, 1.0f, 0.0f);
    auto normals = vsg::vec3Array::create(3);
    for (auto& normal : *normals) {
        normal = vsg::vec3(0.0f, 0.0f, 1.0f);
    }
    auto colors = vsg::vec4Value::create(vsg::vec4(1.0f, 1.0f, 1.0f, 1.0f));  // white INSTANCE colour
    auto indices = vsg::uintArray::create(3);
    (*indices)[0] = 0;
    (*indices)[1] = 1;
    (*indices)[2] = 2;

    vsg::DataList arrays;
    config->assignArray(arrays, "vsg_Vertex", VK_VERTEX_INPUT_RATE_VERTEX, vertices);
    config->assignArray(arrays, "vsg_Normal", VK_VERTEX_INPUT_RATE_VERTEX, normals);
    config->assignArray(arrays, "vsg_Color", VK_VERTEX_INPUT_RATE_INSTANCE, colors);

    auto material = vsg::PhongMaterialValue::create();
    material->value().ambient = vsg::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    material->value().diffuse = vsg::vec4(0.0f, 1.0f, 0.0f, 1.0f);  // GREEN diffuse
    material->value().specular = vsg::vec4(0.1f, 0.1f, 0.1f, 1.0f);
    material->value().emissive = vsg::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    material->value().shininess = 32.0f;
    config->assignDescriptor("material", material);

    config->init();
    auto stateGroup = vsg::StateGroup::create();
    config->copyTo(stateGroup, {});

    auto drawCommands = vsg::Commands::create();
    drawCommands->addChild(vsg::BindVertexBuffers::create(config->baseAttributeBinding, arrays));
    drawCommands->addChild(vsg::BindIndexBuffer::create(indices));
    drawCommands->addChild(vsg::DrawIndexed::create(3, 1, 0, 0, 0));
    stateGroup->addChild(drawCommands);
    return stateGroup;
}

// Builder path copied verbatim from vsgExamples lighting/vsglights
// createTestScene(). Uses vsg::Builder's own createBox, which is the code path
// every official vsg tool (vsgviewer etc.) relies on.
vsg::ref_ptr<vsg::Node> makeBuilderBoxNode()
{
    auto options = vsg::Options::create();
    options->sharedObjects = vsg::SharedObjects::create();

    auto builder = vsg::Builder::create();
    builder->options = options;

    vsg::GeometryInfo geomInfo;
    vsg::StateInfo stateInfo;
    return builder->createBox(geomInfo, stateInfo);
}

// Flat-shaded red triangle: NO material, NO light. The flat shader outputs the
// per-vertex vsg_Color directly. Uses the explicit BindVertexBuffers +
// BindIndexBuffer + DrawIndexed pattern from vsgExamples'
// vsggraphicspipelineconfigurator (the proven manual construction).
vsg::ref_ptr<vsg::Node> makeFlatRedTriangle()
{
    auto shaderSet = vsg::createFlatShadedShaderSet();
    for (auto& ps : shaderSet->defaultGraphicsPipelineStates) {
        if (auto raster = ps.cast<vsg::RasterizationState>()) {
            raster->cullMode = VK_CULL_MODE_NONE;
        }
    }

    auto config = vsg::GraphicsPipelineConfigurator::create(shaderSet);

    auto vertices = vsg::vec3Array::create(3);
    (*vertices)[0] = vsg::vec3(-1.0f, -1.0f, 0.0f);
    (*vertices)[1] = vsg::vec3(1.0f, -1.0f, 0.0f);
    (*vertices)[2] = vsg::vec3(0.0f, 1.0f, 0.0f);
    auto normals = vsg::vec3Array::create(3);
    for (auto& normal : *normals) {
        normal = vsg::vec3(0.0f, 0.0f, 1.0f);
    }
    auto colors = vsg::vec4Value::create(vsg::vec4(1.0f, 0.0f, 0.0f, 1.0f));  // single INSTANCE colour, like the working example
    auto indices = vsg::uintArray::create(3);
    (*indices)[0] = 0;
    (*indices)[1] = 1;
    (*indices)[2] = 2;

    vsg::DataList arrays;
    config->assignArray(arrays, "vsg_Vertex", VK_VERTEX_INPUT_RATE_VERTEX, vertices);
    config->assignArray(arrays, "vsg_Normal", VK_VERTEX_INPUT_RATE_VERTEX, normals);
    config->assignArray(arrays, "vsg_Color", VK_VERTEX_INPUT_RATE_INSTANCE, colors);

    // Even the "flat" shader multiplies vertexColor by material.diffuseColor,
    // so a material descriptor MUST be bound (Builder always does this).
    auto material = vsg::PhongMaterialValue::create();
    material->value().ambient = vsg::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    material->value().diffuse = vsg::vec4(1.0f, 1.0f, 1.0f, 1.0f);  // white -> vertexColor shows through
    material->value().specular = vsg::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    material->value().shininess = 0.0f;
    config->assignDescriptor("material", material);

    config->init();
    auto stateGroup = vsg::StateGroup::create();
    config->copyTo(stateGroup, {});

    auto drawCommands = vsg::Commands::create();
    drawCommands->addChild(vsg::BindVertexBuffers::create(config->baseAttributeBinding, arrays));
    drawCommands->addChild(vsg::BindIndexBuffer::create(indices));
    drawCommands->addChild(vsg::DrawIndexed::create(3, 1, 0, 0, 0));
    stateGroup->addChild(drawCommands);
    return stateGroup;
}

// Builder flat-shaded RED quad: uses vsg::Builder's own working construction
// path (like the box that renders), but with the flat shader (lighting off) and
// a red per-vertex colour. If this shows a red rectangle, Builder's flat path
// works and the failure is specific to our hand-rolled GraphicsPipelineConfig.
vsg::ref_ptr<vsg::Node> makeBuilderFlatRedQuad()
{
    auto options = vsg::Options::create();
    options->sharedObjects = vsg::SharedObjects::create();

    auto builder = vsg::Builder::create();
    builder->options = options;

    vsg::GeometryInfo geomInfo;
    vsg::StateInfo stateInfo;
    stateInfo.lighting = false;                                // flat shader
    geomInfo.color = vsg::vec4(1.0f, 0.0f, 0.0f, 1.0f);        // red
    return builder->createQuad(geomInfo, stateInfo);
}

/**
 * @brief One Builder box (position = centre, dx/dy/dz = FULL extents).
 */
vsg::ref_ptr<vsg::Node> probeBox(vsg::Builder& builder,
                                 const vsg::vec3& centre,
                                 const vsg::vec3& extents,
                                 const vsg::vec4& color)
{
    vsg::GeometryInfo geomInfo;
    geomInfo.position = centre;
    geomInfo.dx       = vsg::vec3(extents.x, 0.0f, 0.0f);
    geomInfo.dy       = vsg::vec3(0.0f, extents.y, 0.0f);
    geomInfo.dz       = vsg::vec3(0.0f, 0.0f, extents.z);
    geomInfo.color    = color;
    vsg::StateInfo stateInfo; // lighting = true -> phong
    return builder.createBox(geomInfo, stateInfo);
}

/**
 * @brief Fills a light group with ambient + HardShadows directional.
 *
 * @param light_group Target group (children are replaced).
 */
void addProbeLights(vsg::ref_ptr<vsg::Group>& light_group);

/**
 * @brief Minimal vsg built-in shadow repro: ground + box stack lit by an
 * ambient + a directional HardShadows sun, viewed from an elevated 3/4 angle.
 *
 * Mirrors the geometry that Vine's demo uses (sun travelling toward the
 * camera so the cast shadows fall on the visible ground). This exercises ONLY
 * vanilla vsg (createRenderGraphForView + Builder geometry + DirectionalLight
 * with HardShadows) with none of Vine's custom wiring, so it isolates whether
 * vsg's built-in shadow path works at all on this driver.
 *
 * The lights live in a dedicated nested group (like Vine's per-view light
 * group), so a caller can replace them every frame to mimic Vine's
 * setGroupLights() behaviour.
 *
 * @param out_group      Receives the root group (lights + geometry).
 * @param out_light_group Receives the nested group holding the lights.
 */
void makeShadowScene(vsg::ref_ptr<vsg::Group>& out_group, vsg::ref_ptr<vsg::Group>& out_light_group)
{
    auto options = vsg::Options::create();
    options->sharedObjects = vsg::SharedObjects::create();
    auto builder = vsg::Builder::create();
    builder->options = options;

    auto group = vsg::Group::create();
    auto light_group = vsg::Group::create();
    group->addChild(light_group);

    // Ground: top at y = 0.
    group->addChild(probeBox(*builder, vsg::vec3(0.0f, -0.05f, 0.0f), vsg::vec3(5.2f, 0.1f, 5.2f),
                             vsg::vec4(0.45f, 0.47f, 0.52f, 1.0f)));
    // Stack of boxes rising from the ground.
    group->addChild(probeBox(*builder, vsg::vec3(0.0f, 0.5f, 0.0f), vsg::vec3(1.6f, 1.0f, 1.6f),
                             vsg::vec4(0.25f, 0.75f, 0.25f, 1.0f)));
    group->addChild(probeBox(*builder, vsg::vec3(0.0f, 1.35f, 0.0f), vsg::vec3(1.1f, 0.7f, 1.1f),
                             vsg::vec4(0.85f, 0.30f, 0.25f, 1.0f)));
    group->addChild(probeBox(*builder, vsg::vec3(0.0f, 2.02f, 0.0f), vsg::vec3(0.64f, 0.64f, 0.64f),
                             vsg::vec4(0.25f, 0.45f, 0.90f, 1.0f)));
    group->addChild(probeBox(*builder, vsg::vec3(1.7f, 0.35f, -1.4f), vsg::vec3(1.0f, 0.7f, 1.0f),
                             vsg::vec4(0.95f, 0.72f, 0.10f, 1.0f)));

    addProbeLights(light_group);
    out_group = group;
    out_light_group = light_group;
}

/**
 * @brief Fills a light group with ambient + HardShadows directional.
 *
 * @param light_group Target group (children are replaced).
 */
void addProbeLights(vsg::ref_ptr<vsg::Group>& light_group)
{
    light_group->children.clear();
    auto ambient = vsg::AmbientLight::create();
    ambient->color.set(1.0f, 1.0f, 1.0f);
    ambient->intensity = 0.25f;
    light_group->addChild(ambient);
    auto sun = vsg::DirectionalLight::create();
    sun->color.set(1.0f, 1.0f, 1.0f);
    sun->intensity = 1.0f;
    sun->direction.set(0.35f, -0.75f, 0.35f);  // travels toward +X/+Z (camera side)
    sun->shadowSettings = vsg::HardShadows::create();
    light_group->addChild(sun);
}

int main(int argc, char** argv)
{
    try {
        if (std::getenv("VINE_PROBE_DIAG")) {
            // Low-level Vulkan diagnostics before full window creation.
            try {
                vsg::Names exts;
                vsg::Names layers;
                auto instance = vsg::Instance::create(exts, layers);
                std::cerr << "diag: instance ok, physicalDevices="
                          << instance->getPhysicalDevices().size() << "\n";
                for (const auto& pd : instance->getPhysicalDevices()) {
                    if (pd) {
                        std::cerr << "diag:   device: "
                                  << pd->getProperties().deviceName
                                  << " type=" << pd->getProperties().deviceType << "\n";
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "diag: instance exception: " << e.what() << "\n";
            }
        }
        const char* mode = std::getenv("VINE_PROBE_MODE");
        const bool useBuilder = (mode && std::string(mode) == "box");
        const bool useFlat = (mode && std::string(mode) == "flat");
        const bool useBQuad = (mode && std::string(mode) == "bquad");
        const bool useShadow = (mode && std::string(mode) == "shadow");
        const bool recreateLights = std::getenv("VINE_PROBE_RECREATE_LIGHTS") != nullptr;
        long maxFrames = 0;
        if (const char* f = std::getenv("VINE_PROBE_FRAMES")) maxFrames = std::atol(f);

        auto traits = vsg::WindowTraits::create();
        traits->windowTitle = "vsg_probe";
        traits->width = 800;
        traits->height = 600;
        traits->debugLayer = true;

        auto window = vsg::Window::create(traits);
        if (!window) {
            std::cerr << "vsg_probe: failed to create window" << std::endl;
            return 1;
        }
        window->clearColor() = vsg::vec4(0.3f, 0.3f, 0.3f, 1.0f);  // GRAY clear (distinguish from geometry)

        auto viewer = vsg::Viewer::create();
        viewer->addWindow(window);

        vsg::ref_ptr<vsg::Node> scene;
        vsg::ref_ptr<vsg::Group> light_group;
        if (useBuilder) {
            scene = makeBuilderBoxNode();
        } else if (useFlat) {
            scene = makeFlatRedTriangle();
        } else if (useBQuad) {
            scene = makeBuilderFlatRedQuad();
        } else if (useShadow) {
            vsg::ref_ptr<vsg::Group> group;
            makeShadowScene(group, light_group);
            scene = group;
        } else {
            scene = makeRawDemoNode();
        }

        vsg::dvec3 centre{ 0.0, 0.0, 0.0 };
        double distance = 5.0;
        if (useBuilder) {
            auto bounds = vsg::visit<vsg::ComputeBounds>(scene).bounds;
            centre = (bounds.min + bounds.max) * 0.5;
            distance = vsg::length(bounds.max - bounds.min) * 3.0;
            std::cerr << "vsg_probe: box centre=" << centre
                      << " distance=" << distance << std::endl;
        } else if (useShadow) {
            // Elevated 3/4 view so the ground and any shadow on it is clearly
            // in frame (a level straight-on view can never show ground shadows).
            centre = vsg::dvec3(0.0, 0.6, 0.0);
            distance = vsg::length(vsg::dvec3(6.5, 5.0, 6.5) - centre);
        }

        auto lookAt = vsg::LookAt::create(centre + vsg::dvec3(0.0, 0.0, distance),
                                          centre,
                                          vsg::dvec3(0.0, 1.0, 0.0));
        if (useShadow) {
            lookAt = vsg::LookAt::create(vsg::dvec3(6.5, 5.0, 6.5), centre, vsg::dvec3(0.0, 1.0, 0.0));
        }
        const double aspect = static_cast<double>(window->extent2D().width)
            / static_cast<double>(window->extent2D().height);
        auto perspective = vsg::Perspective::create(45.0, aspect, 0.1, distance * 10.0);
        if (const char* far_s = std::getenv("VINE_PROBE_FAR")) {
            perspective->farDistance = std::atof(far_s);
            std::cerr << "vsg_probe: far overridden to " << perspective->farDistance << std::endl;
        }
        auto camera = vsg::Camera::create(perspective, lookAt,
                                          vsg::ViewportState::create(window->extent2D()));

        auto renderGraph = vsg::createRenderGraphForView(
            window, camera, scene, VK_SUBPASS_CONTENTS_INLINE, true);
        auto commandGraph = vsg::CommandGraph::create(window);
        commandGraph->addChild(renderGraph);
        viewer->assignRecordAndSubmitTaskAndPresentation(vsg::CommandGraphs{ commandGraph });

        if (!viewer->compile()) {
            std::cerr << "vsg_probe: compile failed" << std::endl;
            return 1;
        }

        std::cerr << "vsg_probe: running" << std::endl;
        long frame = 0;
        while (viewer->advanceToNextFrame()) {
            viewer->handleEvents();
            if (recreateLights && light_group) {
                // Mimic Vine's setGroupLights(): destroy + rebuild the light
                // nodes (incl. the HardShadows directional) every frame.
                addProbeLights(light_group);
            }
            viewer->update();
            viewer->recordAndSubmit();
            viewer->present();
            if (maxFrames > 0 && ++frame >= maxFrames) break;
        }
        std::cerr << "vsg_probe: done (" << frame << " frames)" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "vsg_probe: exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
