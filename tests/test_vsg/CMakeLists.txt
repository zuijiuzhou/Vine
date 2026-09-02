set(TEST_NAME test_vsg)
set(SRC_FILE_LIST
	GfxBackendVsgPluginTest.cpp
)
add_executable(${TEST_NAME} ${SRC_FILE_LIST})
set_target_properties(${TEST_NAME} PROPERTIES FOLDER test)
# The backend is an appfw plugin (MODULE, cannot be linked). The test boots an
# Application, loads the gfx_backend_vsg plugin through PluginManager, then
# creates the backend by name via RenderBackendRegistry — exercising the real
# plugin load → register → create path.
target_link_libraries(${TEST_NAME} vi::Appfw vi::Graphics)
add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
