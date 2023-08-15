set(CURRENT_LIB_NAME Vine)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
qt_standard_project_setup()

file(GLOB SRC_FILE_LIST LIST_DIRECTORIES false *.cpp)
file(GLOB HEADER_FILE_LIST LIST_DIRECTORIES false *.h)
file(GLOB RC_FILE_LIST LIST_DIRECTORIES false *.rc)



qt_add_executable(${CURRENT_LIB_NAME} WIN32 ${SRC_FILE_LIST} ${HEADER_FILE_LIST} ${RC_FILE_LIST})
set_target_properties(${CURRENT_LIB_NAME} PROPERTIES WIN32_EXECUTABLE TRUE MACOSX_BUNDLE TRUE)
target_link_libraries(${CURRENT_LIB_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets vi::AppfwGui vi::Di)

# add_custom_command(TARGET ${CURRENT_LIB_NAME} POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different
#         $<TARGET_FILE:Qt6::Core>
#         $<TARGET_FILE:Qt6::Gui>
#         $<TARGET_FILE:Qt6::Widgets>
#         $<TARGET_FILE_DIR:${CURRENT_LIB_NAME}>)
# set_target_properties(${CURRENT_LIB_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)

# copy_qt_dependencies(${CURRENT_LIB_NAME} Qt::Core Qt::Widgets Qt::Gui)

