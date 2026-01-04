# find_package(icu COMPONENTS uc)

vi_add_library(GLOBAL_TARGET_NAME Global)

target_compile_definitions(${GLOBAL_TARGET_NAME} INTERFACE VI_ROOT_NS=vine)