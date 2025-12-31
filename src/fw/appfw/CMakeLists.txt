
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

target_link_libraries(${APPFW_TARGET_NAME} PUBLIC vi::Core vi::Di vi::Math vi::Global PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets )

file(GLOB THIRD_SARIBBON_SRC_FILE_LIST ${THIRD_SARIBBON_ROOT}/*.cpp)
#file(GLOB THIRD_SARIBBON_RES_FILE_LIST ${THIRD_SARIBBON_ROOT}/*.qrc)

#qt_add_resources(THIRD_SARIBBON_QT_RES_LIST ${THIRD_SARIBBON_RES_FILE_LIST})

target_sources(${APPFW_TARGET_NAME} PRIVATE ${THIRD_SARIBBON_SRC_FILE_LIST} )
#source_group(qres FILES ${THIRD_SARIBBON_QT_RES_LIST})

target_include_directories(${APPFW_TARGET_NAME} PRIVATE ${THIRD_SARIBBON_ROOT})