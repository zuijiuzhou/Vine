// Diagnostic tool: dump the built-in vsg shader-set presets (flat / phong /
// pbr) with their attribute + descriptor bindings (incl. each descriptor's
// data type), used to check preset feasibility against the Vine material path.
#include <vsg/io/Options.h>
#include <vsg/utils/ShaderSet.h>

#include <iostream>

int main()
{
    try {
        struct Factory {
            const char* name;
            vsg::ref_ptr<vsg::ShaderSet> (*make)();
        };
        const Factory factories[] = {
            { "flat",  +[] { return vsg::createFlatShadedShaderSet(); } },
            { "phong", +[] { return vsg::createPhongShaderSet(); } },
            { "pbr",   +[] { return vsg::createPhysicsBasedRenderingShaderSet(); } },
        };
        for (const auto& f : factories) {
            const auto ss = f.make();
            std::cerr << "== " << f.name << "\n";
            std::cerr << "  stages=" << ss->stages.size()
                      << " attributeBindings=" << ss->attributeBindings.size()
                      << " descriptorBindings=" << ss->descriptorBindings.size()
                      << " pushConstantRanges=" << ss->pushConstantRanges.size()
                      << " optionalDefines=" << ss->optionalDefines.size()
                      << "\n";
            for (const auto& pc : ss->pushConstantRanges) {
                std::cerr << "  push name=" << pc.name
                          << " define=" << pc.define
                          << " stage=" << pc.range.stageFlags
                          << " offset=" << pc.range.offset
                          << " size=" << pc.range.size << "\n";
            }
            for (const auto& ab : ss->attributeBindings) {
                std::cerr << "  attr name=" << ab.name
                          << " loc=" << ab.location
                          << " format=" << ab.format << "\n";
            }
            for (const auto& db : ss->descriptorBindings) {
                std::cerr << "  desc name=" << db.name
                          << " set=" << db.set
                          << " binding=" << db.binding
                          << " type=" << db.descriptorType
                          << " data=" << (db.data ? db.data->className() : "(null)")
                          << "\n";
            }
        }
        std::cerr << "done\n";
    } catch (const std::exception& e) {
        std::cerr << "exception: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
