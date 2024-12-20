find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
qt_standard_project_setup()
vi_internal_add_library(AppfwGui OUT_TARGET_NAME)

file(GLOB THIRD_SARIBBON_SRC_FILE_LIST ${THIRD_SARIBBON_ROOT}/*.cpp)
file(GLOB THIRD_SARIBBON_RES_FILE_LIST ${THIRD_SARIBBON_ROOT}/*.qrc)

target_sources(${OUT_TARGET_NAME} PRIVATE ${THIRD_SARIBBON_SRC_FILE_LIST} ${THIRD_SARIBBON_RES_FILE_LIST})
source_group(qres FILES ${THIRD_SARIBBON_RES_FILE_LIST})
target_include_directories(${OUT_TARGET_NAME} PRIVATE ${THIRD_SARIBBON_ROOT})

target_link_libraries(${OUT_TARGET_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets PUBLIC vi::Ge vi::Core vi::Appfw)

set_target_properties(${OUT_TARGET_NAME} PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)

target_compile_definitions(${OUT_TARGET_NAME} PUBLIC VI_APPFWGUI_IMPL_QT)