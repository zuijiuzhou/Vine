# find_package(icu COMPONENTS uc)

vi_add_library(CORE_TARGET_NAME Core)

target_link_libraries(${CORE_TARGET_NAME} PUBLIC vi::Global)