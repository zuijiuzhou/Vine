# gfx_backend_vsg: VSG render backend delivered as an appfw plugin.
#
# The plugin is a MODULE DLL (v_add_plugin) containing the full implementation:
# SceneBridge, CameraBridge, VsgRenderer, VsgMaterialManager and the backend
# factory. When loaded by PluginManager, GfxBackendVsgPlugin::load() registers
# the "vsg" backend into RenderBackendRegistry.
#
# Tests cannot link a MODULE library; test_vsg compiles the implementation
# sources directly (see tests/test_vsg/CMakeLists.txt).

v_add_plugin(GFX_BACKEND_VSG_TARGET gfx_backend_vsg)

# v_add_plugin derives the export macro from the short name (gfx_backend_vsg ->
# V_GFX_BACKEND_VSG_LIB), but vsg_global.hpp checks V_VSG_LIB. Define it
# explicitly so V_VSG_API expands to dllexport when building this plugin.
target_compile_definitions(${GFX_BACKEND_VSG_TARGET} PRIVATE V_VSG_LIB)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(FetchContent)
if(VINE_USE_FETCHCONTENT)
    # A FetchContent-built VSG (no glslang) uses its embedded precompiled phong
    # SPIR-V, which renders correctly (verified). No runtime shader compiler is
    # required. If an installed VSG (e.g. from vcpkg) is already on the search
    # path, prefer it; otherwise fetch VSG v1.1.16.
    find_package(vsg CONFIG QUIET)
    if(NOT vsg_FOUND)
        FetchContent_Declare(vsg
            GIT_REPOSITORY https://github.com/vsg-dev/VulkanSceneGraph.git
            GIT_TAG        v1.1.16
        )
        set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)   # Static lib, baked into the DLL.
        FetchContent_MakeAvailable(vsg)
        v_group_targets_folder(${vsg_SOURCE_DIR} third_party)
    else()
        message(STATUS "gfx_backend_vsg: using installed vsg")
    endif()
else()
    # Consume vsg from vcpkg or a local install (e.g. /opt/opensrc/VSG on Linux).
    find_package(vsg CONFIG REQUIRED)
endif()

# vsg::vsg is PUBLIC because SceneBridge exposes vsg::ref_ptr<vsg::Node> in its API.
# The plugin needs appfw for the Plugin base class and graphics for the backend
# interface + registry.
target_link_libraries(${GFX_BACKEND_VSG_TARGET} PRIVATE
    vi::Appfw
    vi::Graphics
    vsg::vsg)

# TEMP diagnostic: standalone minimal vsg executable that renders a red box
# through vsg's own Builder/phong path, to isolate whether the vendored vsg
# renders correct colors independent of Vine. Remove after diagnosis.
add_executable(vsg_color_probe ${CMAKE_CURRENT_SOURCE_DIR}/vsg_probe/main.cpp)
target_link_libraries(vsg_color_probe PRIVATE vsg::vsg)
set_target_properties(vsg_color_probe PROPERTIES FOLDER third_party)

# TEMP diagnostic: dump the embedded phong ShaderSet SPIR-V for inspection.
# Remove after diagnosis.
add_executable(vsg_shader_dump ${CMAKE_CURRENT_SOURCE_DIR}/vsg_shader_dump/main.cpp)
target_link_libraries(vsg_shader_dump PRIVATE vsg::vsg)
set_target_properties(vsg_shader_dump PROPERTIES FOLDER third_party)
