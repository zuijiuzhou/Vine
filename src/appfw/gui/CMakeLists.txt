set(CURRENT_LIB_NAME EtdAppfwGui)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
qt_standard_project_setup()

file(GLOB SRC_FILE_LIST LIST_DIRECTORIES false *.cpp)
file(GLOB HEADER_FILE_LIST LIST_DIRECTORIES false *.h)
file(GLOB RC_FILE_LIST LIST_DIRECTORIES false *.rc)

message(${SRC_FILE_LIST})

add_library(${CURRENT_LIB_NAME} SHARED ${SRC_FILE_LIST} ${HEADER_FILE_LIST} ${RC_FILE_LIST})
set_target_properties(${CURRENT_LIB_NAME} PROPERTIES OUTPUT_NAME_DEBUG ${CURRENT_LIB_NAME}d)
set_target_properties(${CURRENT_LIB_NAME} PROPERTIES OUTPUT_NAME_Release ${CURRENT_LIB_NAME})
target_link_libraries(${CURRENT_LIB_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets PUBLIC EtdCore EtdAppfw)
# add_compile_definitions(ETD_GE_LIB)
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ETD_APPFW_GUI_LIB)