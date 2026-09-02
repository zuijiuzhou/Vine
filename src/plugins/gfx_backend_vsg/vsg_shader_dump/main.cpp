// Diagnostic tool: load the embedded phong ShaderSet via vsg and dump each
// shader stage's SPIR-V (and GLSL source, if present) to disk for inspection
// with spirv-dis. Removed after diagnosis.
#include <vsg/io/Options.h>
#include <vsg/state/ShaderModule.h>
#include <vsg/state/ShaderStage.h>
#include <vsg/utils/ShaderSet.h>

#include <fstream>
#include <iostream>

int main()
{
    try {
        auto shaderSet = vsg::createPhongShaderSet();
        std::cerr << "stages: " << shaderSet->stages.size() << "\n";
        std::cerr << "attributeBindings: " << shaderSet->attributeBindings.size() << "\n";
        for (const auto& ab : shaderSet->attributeBindings) {
            std::cerr << "  attr name=" << ab.name
                      << " location=" << ab.location
                      << " format=" << ab.format << "\n";
        }
        std::cerr << "descriptorBindings: " << shaderSet->descriptorBindings.size() << "\n";
        for (const auto& db : shaderSet->descriptorBindings) {
            std::cerr << "  desc name=" << db.name
                      << " set=" << db.set
                      << " binding=" << db.binding
                      << " type=" << db.descriptorType
                      << " stageFlags=" << db.stageFlags
                      << " count=" << db.descriptorCount << "\n";
        }

        for (size_t i = 0; i < shaderSet->stages.size(); ++i) {
            const auto& st = shaderSet->stages[i];
            const auto& module = st->module;
            std::cerr << "stage[" << i << "] stage=" << st->stage
                      << " sourceLen=" << module->source.size()
                      << " codeWords=" << module->code.size() << "\n";
            const std::string ext = (st->stage == VK_SHADER_STAGE_VERTEX_BIT) ? ".vert" : ".frag";
            {
                std::ofstream f("dump_stage_" + std::to_string(i) + ".spv", std::ios::binary);
                for (uint32_t w : module->code) {
                    f.write(reinterpret_cast<const char*>(&w), sizeof(w));
                }
            }
            if (!module->source.empty()) {
                std::ofstream f("dump_stage_" + std::to_string(i) + ext);
                f << module->source;
            }
        }
        std::cerr << "done\n";
    } catch (const std::exception& e) {
        std::cerr << "exception: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
