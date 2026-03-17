# find_package(icu COMPONENTS uc)

v_add_library(GLOBAL_TARGET_NAME Global)

target_compile_definitions(${GLOBAL_TARGET_NAME} INTERFACE V_ROOT_NS=vine)