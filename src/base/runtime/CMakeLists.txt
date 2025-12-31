vi_add_library(RUNTIME_TARGET_NAME Runtime)

target_link_libraries(${RUNTIME_TARGET_NAME} PUBLIC vi::Global)