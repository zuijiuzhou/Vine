v_add_library(VSG_TARGET_NAME VsGraphics)

# VSG is installed locally (built in Linux) at /opt/opensrc/VSG.
# find_package(vsg CONFIG) resolves vsg::vsg with its include dirs.
find_package(vsg CONFIG REQUIRED)

# vsg::vsg is PUBLIC because SceneBridge exposes vsg::ref_ptr<vsg::Node> in its API.
target_link_libraries(${VSG_TARGET_NAME} PUBLIC vi::Graphics)
target_link_libraries(${VSG_TARGET_NAME} PUBLIC vsg::vsg)
