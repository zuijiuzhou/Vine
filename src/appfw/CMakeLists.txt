find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
qt_standard_project_setup()
 
vine_internal_add_library(Appfw CURRENT_TARGET_NAME)

target_link_libraries(${CURRENT_TARGET_NAME} PRIVATE Qt6::Core PUBLIC vi::Core)

add_subdirectory(gui)