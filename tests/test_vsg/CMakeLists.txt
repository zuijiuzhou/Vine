set(TEST_NAME test_vsg)
set(SRC_FILE_LIST
	GfxBackendVsgPluginTest.cpp
	RenderStateMapperTest.cpp
	GlslCompileTest.cpp
	SceneBridgePipelineSharingTest.cpp
)
add_executable(${TEST_NAME} ${SRC_FILE_LIST})
set_target_properties(${TEST_NAME} PROPERTIES FOLDER test)
# The backend is an appfw plugin (MODULE, cannot be linked). The plugin-load
# test boots an Application, loads the gfx_backend_vsg plugin through
# PluginManager, then creates the backend by name via RenderBackendRegistry —
# exercising the real plugin load → register → create path.
#
# The RenderStateMapperTest exercises the device-free SDK-state → vsg-state
# mapping, so this target also links vsg and includes the plugin's public
# headers directly.
#
# SceneBridgePipelineSharingTest instantiates SceneBridge itself, so the two
# device-free bridge sources (SceneBridge + its material manager) are compiled
# into the test alongside its public headers.
find_package(vsg CONFIG QUIET)
target_link_libraries(${TEST_NAME} vi::Appfw vi::Graphics)
if(vsg_FOUND)
	target_link_libraries(${TEST_NAME} vsg::vsg)
endif()
target_sources(${TEST_NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/plugins/gfx_backend_vsg/src/SceneBridge.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/plugins/gfx_backend_vsg/src/VsgMaterialManager.cpp)
target_include_directories(${TEST_NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/plugins/gfx_backend_vsg/include
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/plugins/gfx_backend_vsg/src)
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
