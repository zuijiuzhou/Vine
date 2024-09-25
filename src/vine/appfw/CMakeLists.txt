
find_package(Qt6 CONFIG REQUIRED COMPONENTS Core Gui Widgets)
qt_standard_project_setup()

vi_internal_add_library(Appfw OUT_TARGET_NAME)

target_link_libraries(${OUT_TARGET_NAME} PRIVATE Qt6::Core PUBLIC vi::Core vi::Di)

add_subdirectory(gui)