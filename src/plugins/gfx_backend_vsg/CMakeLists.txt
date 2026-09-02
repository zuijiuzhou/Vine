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
    # Enable VSG's runtime shader compiler: VSG's precompiled phong SPIR-V
    # renders materials with a wrong fixed colour, so shaders must be compiled
    # from GLSL at runtime via glslang.
    FetchContent_Declare(glslang
        GIT_REPOSITORY https://github.com/KhronosGroup/glslang.git
        GIT_TAG        14.3.0
    )
    # glslang's SPIR-V optimizer pulls in SPIRV-Tools (extra dependency); VSG
    # only needs glslang's core shader compiler, so disable it.
    set(ENABLE_OPT OFF CACHE BOOL "" FORCE)
    set(ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(glslang)
    # glslang exports its CMake config into its build tree; point find_package
    # at it so VSG's shader-compiler probe succeeds.
    set(glslang_DIR "${glslang_BINARY_DIR}" CACHE PATH "glslang build config" FORCE)

    FetchContent_Declare(vsg
        GIT_REPOSITORY https://github.com/vsg-dev/VulkanSceneGraph.git
        GIT_TAG        v1.1.16
    )
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)           # Static lib, baked into the DLL.
    set(VSG_SUPPORTS_ShaderCompiler ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(vsg)

    # Group every third-party target under the third_party solution folder.
    v_group_targets_folder(${vsg_SOURCE_DIR} third_party)
    if(TARGET glslang)
        v_group_targets_folder(${glslang_SOURCE_DIR} third_party)
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
