find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
qt_standard_project_setup()

vine_internal_add_library(AppfwGui OUT_TARGET_NAME)
message(dddd${THIRD_SARIBBON_ROOT})
target_include_directories(${OUT_TARGET_NAME} PRIVATE ${THIRD_SARIBBON_ROOT})

target_link_libraries(${OUT_TARGET_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets PUBLIC vi::Core vi::Appfw)