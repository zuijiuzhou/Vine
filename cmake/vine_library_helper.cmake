function(vi_internal_add_library short_name out_target_name)
    file(GLOB SRC_FILE_LIST LIST_DIRECTORIES false *.cpp)
    file(GLOB HEADER_FILE_LIST LIST_DIRECTORIES false *.h)
    file(GLOB RC_FILE_LIST LIST_DIRECTORIES false *.rc)
    # 添加SDK头文件
    file(RELATIVE_PATH REL_CURRENT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR})
    set(CURRENT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include/${REL_CURRENT_SOURCE_DIR})
    file(GLOB SDK_HEADER_FILE_LIST ${CURRENT_INCLUDE_DIR}/*.h)
    # 目标名称
    set(LIB_FULL_NAME ${VI_SHARED_LIBRARY_PREFIX}${short_name})
    # 目标别名
    set(LIB_ALIAS ${VI_SHARED_LIBRARY_PREFIX}::${short_name})

    message(--------AddLib:${LIB_ALIAS})
    # 创建目标
    add_library(${LIB_FULL_NAME} SHARED ${SDK_HEADER_FILE_LIST} ${HEADER_FILE_LIST} ${SRC_FILE_LIST} ${RC_FILE_LIST})
    # 设置目标别名
    add_library(${LIB_ALIAS} ALIAS ${LIB_FULL_NAME})
    # 设置输出文件名
    set_target_properties(${LIB_FULL_NAME} PROPERTIES OUTPUT_NAME ${LIB_FULL_NAME})

    set_target_properties(${LIB_FULL_NAME} PROPERTIES FOLDER vi)
    # 设置源文件分组
    source_group(sdk FILES ${SDK_HEADER_FILE_LIST})
    # source_group(inc FILES ${HEADER_FILE_LIST})
    # source_group(src FILES ${SRC_FILE_LIST})
    # source_group(res FILES ${RC_FILE_LIST})
    
    string(TOUPPER VI_${short_name}_LIB LIB_COMPILE_DEF)
    target_compile_definitions(${LIB_FULL_NAME} PRIVATE ${LIB_COMPILE_DEF})

    if ("${out_target_name}" STREQUAL "")   
    else ()
        set(${out_target_name} ${LIB_FULL_NAME} PARENT_SCOPE)
    endif ()

endfunction(vi_internal_add_library target)
