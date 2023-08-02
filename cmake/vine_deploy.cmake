set(VINE_DEPLOY_TARGET Deploy)
add_custom_target(${VINE_DEPLOY_TARGET} ALL)
get_target_property(WINDEPLOYQT_EXE Qt6::windeployqt IMPORTED_LOCATION)
message(windeployqt:${WINDEPLOYQT_EXE})
if(WIN32)
    add_custom_command (
        TARGET ${VINE_DEPLOY_TARGET}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E env VCINSTALLDIR="D:/Program Files/Microsoft Visual Studio/2022/Enterprise/VC/Auxiliary/Build" ${WINDEPLOYQT_EXE} $<TARGET_FILE:Appfw>
        COMMENT "Run Qt6::windeployqt")
endif()