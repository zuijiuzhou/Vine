v_add_library(ASYNC_TARGET_NAME Async)

target_link_libraries(${ASYNC_TARGET_NAME} INTERFACE vi::Global vi::Core)
