add_custom_target(DeployQt COMMENT "Deploy Qt runtime using windeployqt")

if (TARGET Qt6::windeployqt)
    get_target_property(WINDEPLOYQT_EXE Qt6::windeployqt IMPORTED_LOCATION_RELEASE)
    if (NOT WINDEPLOYQT_EXE)
        get_target_property(WINDEPLOYQT_EXE Qt6::windeployqt IMPORTED_LOCATION)
    endif()
endif()

if (NOT WINDEPLOYQT_EXE)
    message(FATAL_ERROR "Qt6::windeployqt not found")
endif()

add_custom_command(
        TARGET DeployQt
        POST_BUILD
        COMMAND ${WINDEPLOYQT_EXE} --verbose 0 $<TARGET_FILE:vi::Appfw>
        COMMENT "--------Run:windeployqt")

set_target_properties(DeployQt PROPERTIES FOLDER CMakeUtilTargets)
set_target_properties(DeployQt PROPERTIES SOURCES ${CMAKE_CURRENT_LIST_DIR}/VineDeployQt.cmake)