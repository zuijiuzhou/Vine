set(TEST_NAME test_gui)
file(GLOB SRC_FILE_LIST CONFIGURE_DEPENDS *.cpp)

find_package(Qt6 CONFIG REQUIRED COMPONENTS Core Gui Widgets)

add_executable(${TEST_NAME} ${SRC_FILE_LIST})
set_target_properties(${TEST_NAME} PROPERTIES FOLDER test)

# 注意：不直接链接 tp::SARibbon —— 它同时被 viAppfwd.dll 静态链接，再链一份
# 会与 DLL 内副本形成两份静态元对象，导致跨 DLL 边界的 qobject_cast 失败。
# 这里只加头文件路径供 static_cast（impl<T>）使用，不用 qobject_cast 断言。
target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/third_party/SARibbon)

target_link_libraries(${TEST_NAME}
    vi::Appfw
    Qt6::Core Qt6::Gui Qt6::Widgets)

add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
