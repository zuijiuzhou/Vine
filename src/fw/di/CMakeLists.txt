vi_add_library(DI_TARGET_NAME Di)

target_link_libraries(${DI_TARGET_NAME} PUBLIC vi::Global)
target_link_libraries(${DI_TARGET_NAME} PUBLIC vi::Core)