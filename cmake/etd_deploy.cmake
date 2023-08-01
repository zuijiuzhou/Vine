set(ETD_DEPLOY_TARGET EtdDeploy)
add_custom_target(${ETD_DEPLOY_TARGET} ALL)
get_target_property(WINDEPLOYQT_EXE Qt6::windeployqt IMPORTED_LOCATION)
message(windeployqt:${WINDEPLOYQT_EXE})
if(WIN32)
    add_custom_command (
        TARGET ${ETD_DEPLOY_TARGET}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E env VCINSTALLDIR="D:/Program Files/Microsoft Visual Studio/2022/Enterprise/VC/Auxiliary/Build" ${WINDEPLOYQT_EXE} $<TARGET_FILE:EtdAppfw>
        COMMENT "Run Qt6::windeployqt")
endif()