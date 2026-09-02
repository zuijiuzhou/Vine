v_add_library(WINDOW_TARGET_NAME Window)

target_link_libraries(${WINDOW_TARGET_NAME} PUBLIC vi::Core vi::Global)
