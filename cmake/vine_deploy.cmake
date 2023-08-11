set(VINE_DEPLOY_TARGET Deploy)
add_custom_target(${VINE_DEPLOY_TARGET} ALL)
get_target_property(WINDEPLOYQT_EXE Qt6::windeployqt IMPORTED_LOCATION)
message(windeployqt:${WINDEPLOYQT_EXE})
message(--------VCINSTALLDIR:${VCINSTALLDIR})
if(WIN32)
    add_custom_command (
        TARGET ${VINE_DEPLOY_TARGET}
        POST_BUILD
        COMMAND ${WINDEPLOYQT_EXE} --verbose 2  $<TARGET_FILE:vi::AppfwGui>
        COMMENT "Run Qt6::windeployqt")
endif()