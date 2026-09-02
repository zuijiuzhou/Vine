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
        if (useBuilder) {
            scene = makeBuilderBoxNode();
        } else if (useFlat) {
            scene = makeFlatRedTriangle();
        } else if (useBQuad) {
            scene = makeBuilderFlatRedQuad();
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
        }

        auto lookAt = vsg::LookAt::create(centre + vsg::dvec3(0.0, 0.0, distance),
                                          centre,
                                          vsg::dvec3(0.0, 1.0, 0.0));
        const double aspect = static_cast<double>(window->extent2D().width)
            / static_cast<double>(window->extent2D().height);
        auto perspective = vsg::Perspective::create(45.0, aspect, 0.1, distance * 10.0);
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
        while (viewer->advanceToNextFrame()) {
            viewer->handleEvents();
            viewer->update();
            viewer->recordAndSubmit();
            viewer->present();
        }
    } catch (const std::exception& e) {
        std::cerr << "vsg_probe: exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
