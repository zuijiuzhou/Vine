find_package(
    Qt6
    COMPONENTS
        Core
        Gui
        Widgets
        Xml
    REQUIRED
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

file(GLOB_RECURSE SRC_FILES *.h * .cpp *.png *.qrc)

add_library(DockingPanes STATIC ${SRC_FILES})

source_group(TREE ${CMAKE_CURRENT_LIST_DIR} PREFIX "" FILES ${SRC_FILES})

set_target_properties(DockingPanes PROPERTIES FOLDER third_party)

target_link_libraries(
    DockingPanes
    PUBLIC
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Xml
)

