find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
qt_standard_project_setup()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

v_add_plugin(APP_SHELL_TARGET app_shell)

set_target_properties(${APP_SHELL_TARGET} PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)

target_link_libraries(${APP_SHELL_TARGET} PRIVATE vi::Appfw Qt6::Core Qt6::Gui Qt6::Widgets)
