function(vi_add_library VAR_TARGET_NAME SHORT_NAME)
    # CMAKE_SOURCE_DIR顶级当前CMakeLists.txt文件所在目录
    # CMAKE_CURRENT_SOURCE_DIR 当前CMakeLists.txt所在目录
    file(RELATIVE_PATH SOURCE_REL_DIR ${CMAKE_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR})
    # SDK头文件所在目录
    set(SDK_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include/${SOURCE_REL_DIR})
    # SDK头文件
    file(GLOB SDK_HEADER_FILE_LIST ${SDK_INCLUDE_DIR}/*.hpp ${SDK_INCLUDE_DIR}/*.h)
    # CPP文件
    file(GLOB SRC_FILE_LIST LIST_DIRECTORIES false *.cpp)
    # 头文件
    file(GLOB HEADER_FILE_LIST LIST_DIRECTORIES false *.hpp *.h)
    # 资源文件
    file(GLOB RC_FILE_LIST LIST_DIRECTORIES false *.rc)
    # 目标名称
    set(TARGET_NAME ${SHORT_NAME})
    # 目标别名
    set(TARGET_ALIAS ${VI_SHARED_LIBRARY_PREFIX}::${SHORT_NAME})
    # 库文件名称
    set(LIB_FILE_NAME $<$<PLATFORM_ID:Linux>:lib>${VI_SHARED_LIBRARY_PREFIX}${SHORT_NAME})

    # 创建目标
    add_library(${TARGET_NAME} SHARED ${SDK_HEADER_FILE_LIST} ${HEADER_FILE_LIST} ${SRC_FILE_LIST} ${RC_FILE_LIST})
    # 设置目标别名
    add_library(${TARGET_ALIAS} ALIAS ${TARGET_NAME})
    # 设置输出文件名
    set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME ${LIB_FILE_NAME})
    # 设置输出分组
    set_target_properties(${TARGET_NAME} PROPERTIES FOLDER vi)

    set_target_properties(${TARGET_NAME} PROPERTIES PREFIX "")
    # 设置源文件分组
    source_group(sdk FILES ${SDK_HEADER_FILE_LIST})
    # source_group(inc FILES ${HEADER_FILE_LIST})
    # source_group(src FILES ${SRC_FILE_LIST})
    # source_group(res FILES ${RC_FILE_LIST})

    string(TOUPPER VI_${SHORT_NAME}_LIB LIB_COMPILE_DEF)
    target_compile_definitions(${TARGET_NAME} PRIVATE ${LIB_COMPILE_DEF})

    if("${VAR_TARGET_NAME}" STREQUAL "")
        message(SEND_ERROR "输出的目标名称变量名不能为空")
    else()
        set(${VAR_TARGET_NAME} ${TARGET_NAME} PARENT_SCOPE)
    endif()

    # PRIVATE   仅作用与目标自身
    # PUBLIC    同时作用于目标自身与依赖该目标的目标
    # INTERFACE 仅作用于依赖该目标的目标
    target_include_directories(${TARGET_NAME}
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:include>")

    # string(TOLOWER ${SHORT_NAME} ShortName_lowercase)
    # string(TOLOWER ${CMAKE_PROJECT_NAME} proj_name_lowercase)

    # 安装头文件
    install(FILES ${SDK_HEADER_FILE_LIST} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${SOURCE_REL_DIR})
    # 安装目标文件
    install(
        TARGETS ${TARGET_NAME}
        EXPORT ${CMAKE_PROJECT_NAME}Targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

        

    message(--------AddLib:${TARGET_ALIAS})

endfunction()
