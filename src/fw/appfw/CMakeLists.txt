
find_package(Qt6 CONFIG REQUIRED COMPONENTS Core Gui Widgets)
qt_standard_project_setup()

set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

vi_add_library(APPFW_TARGET_NAME Appfw)

set_target_properties(${APPFW_TARGET_NAME} PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)

target_link_libraries(${APPFW_TARGET_NAME} PUBLIC vi::Core vi::Di vi::Math vi::Global PRIVATE tp::SARibbon tp::DockingPanes Qt6::Core Qt6::Gui Qt6::Widgets )