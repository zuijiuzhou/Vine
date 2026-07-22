 
v_add_library(APPFW_TARGET_NAME RoboticsCore)

target_link_libraries(${APPFW_TARGET_NAME}
    PUBLIC vi::Core vi::Math vi::Global)
