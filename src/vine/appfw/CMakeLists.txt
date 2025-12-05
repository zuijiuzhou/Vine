
find_package(Qt6 CONFIG REQUIRED COMPONENTS Core Gui Widgets)
qt_standard_project_setup()

vi_add_library(APPFW_TARGET_NAME Appfw)

target_link_libraries(${APPFW_TARGET_NAME} PRIVATE Qt6::Core PUBLIC vi::Core vi::Di)

add_subdirectory(gui)