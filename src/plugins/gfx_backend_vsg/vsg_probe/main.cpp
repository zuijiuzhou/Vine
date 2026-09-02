// Standalone minimal vsg probe: renders a raw phong triangle with a red
// material using the EXACT same construction as Vine's
// VsgRenderer::makeRawDemoNode, but inside a vanilla vsg app (no Vine code).
// If this renders RED, Vine's VsgRenderer setup is at fault; if it also shows
// the fixed blue (35,116,208), the vendored vsg phong path itself is broken.

#include <vsg/app/CommandGraph.h>
#include <vsg/app/RenderGraph.h>
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
#include <vsg/utils/GraphicsPipelineConfigurator.h>
#include <vsg/utils/ShaderSet.h>

#include <exception>
#include <iostream>

vsg::ref_ptr<vsg::Node> makeRawDemoNode()
{
    auto shaderSet = vsg::createPhongShaderSet();
    shaderSet->defaultGraphicsPipelineStates = vsg::GraphicsPipelineStates{
        vsg::DepthStencilState::create(),
        vsg::RasterizationState::create(),
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
    auto colors = vsg::vec4Array::create(3);
    for (auto& color : *colors) {
        color = vsg::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    auto indices = vsg::uintArray::create(3);
    (*indices)[0] = 0;
    (*indices)[1] = 1;
    (*indices)[2] = 2;

    vsg::DataList arrays;
    config->assignArray(arrays, "vsg_Vertex", VK_VERTEX_INPUT_RATE_VERTEX, vertices);
    config->assignArray(arrays, "vsg_Normal", VK_VERTEX_INPUT_RATE_VERTEX, normals);
    config->assignArray(arrays, "vsg_Color", VK_VERTEX_INPUT_RATE_VERTEX, colors);

    auto material = vsg::PhongMaterialValue::create();
    material->value().ambient = vsg::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    material->value().diffuse = vsg::vec4(0.0f, 1.0f, 0.0f, 1.0f);  // GREEN triangle
    material->value().specular = vsg::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    material->value().shininess = 32.0f;
    config->assignDescriptor("material", material);

    config->init();
    auto stateGroup = vsg::StateGroup::create();
    config->copyTo(stateGroup, {});

    auto vid = vsg::VertexIndexDraw::create();
    vid->assignArrays(arrays);
    vid->assignIndices(indices);
    vid->indexCount = 3;
    vid->firstBinding = config->baseAttributeBinding;
    stateGroup->addChild(vid);
    return stateGroup;
}

int main(int argc, char** argv)
{
    try {
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
        window->clearColor() = vsg::vec4(1.0f, 0.0f, 0.0f, 1.0f);  // RED clear (channel probe)

        auto viewer = vsg::Viewer::create();
        viewer->addWindow(window);

        auto scene = makeRawDemoNode();

        auto lookAt = vsg::LookAt::create(vsg::dvec3(0.0, 0.0, 5.0),
                                          vsg::dvec3(0.0, 0.0, 0.0),
                                          vsg::dvec3(0.0, 1.0, 0.0));
        const double aspect = static_cast<double>(window->extent2D().width)
            / static_cast<double>(window->extent2D().height);
        auto perspective = vsg::Perspective::create(45.0, aspect, 0.1, 20.0);
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
