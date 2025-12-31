set(VI_DEPLOY_TARGET Deploy)
add_custom_target(${VI_DEPLOY_TARGET} ALL)
get_target_property(WINDEPLOYQT_EXE Qt6::windeployqt IMPORTED_LOCATION)
if(WIN32)
    message(--------windeployqt=${WINDEPLOYQT_EXE})
    message(--------VCINSTALLDIR=${VCINSTALLDIR})
    
    add_custom_command(
        TARGET ${VI_DEPLOY_TARGET}
        POST_BUILD
        COMMAND ${WINDEPLOYQT_EXE} --verbose 0 $<TARGET_FILE:vi::Appfw>
        COMMENT "--------Run:windeployqt")
endif()
