function(vi_add_library target_name_var short_name)
    # CMAKE_SOURCE_DIR顶级当前CMakeLists.txt文件所在目录
    # CMAKE_CURRENT_SOURCE_DIR 当前CMakeLists.txt所在目录
    file(RELATIVE_PATH src_rel_dir ${CMAKE_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR})
    # SDK头文件所在目录
    set(sdk_inc_root_dir ${CMAKE_SOURCE_DIR}/include)
    set(sdk_inc_dir ${sdk_inc_root_dir}/${src_rel_dir})
    
    # SDK头文件
    file(GLOB_RECURSE sdk_header_file_list ${sdk_inc_dir}/*.hpp ${sdk_inc_dir}/*.h)
    # CPP文件
    file(GLOB_RECURSE src_file_list LIST_DIRECTORIES false *.cpp)
    # 头文件
    file(GLOB_RECURSE header_file_list LIST_DIRECTORIES false *.hpp *.h)
    # 资源文件
    file(GLOB_RECURSE rc_file_list LIST_DIRECTORIES false *.rc *.qrc)
    # 目标名称
    set(target_name ${short_name})
    # 目标别名
    set(target_alias ${VI_SHARED_LIBRARY_PREFIX}::${short_name})
    # 库文件名称
    set(lib_file_name $<$<PLATFORM_ID:Linux>:lib>${VI_SHARED_LIBRARY_PREFIX}${short_name})

    # 创建目标
    add_library(${target_name} SHARED ${sdk_header_file_list} ${header_file_list} ${src_file_list} ${rc_file_list})
    # 设置目标别名
    add_library(${target_alias} ALIAS ${target_name})
    # 设置输出文件名
    set_target_properties(${target_name} PROPERTIES OUTPUT_NAME ${lib_file_name})
    # 设置输出分组
    set_target_properties(${target_name} PROPERTIES FOLDER vi)

    set_target_properties(${target_name} PROPERTIES PREFIX "")
    # 设置源文件分组
    source_group(TREE ${sdk_inc_dir} PREFIX sdk FILES ${sdk_header_file_list})
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX headers FILES ${header_file_list})
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX src FILES ${src_file_list})
    # source_group(res FILES ${rc_file_list})

    string(TOUPPER VI_${short_name}_LIB lib_compile_def)
    target_compile_definitions(${target_name} PRIVATE ${lib_compile_def})

    if("${target_name_var}" STREQUAL "")
        message(SEND_ERROR "输出的目标名称变量名不能为空")
    else()
        set(${target_name_var} ${target_name} PARENT_SCOPE)
    endif()

    # PRIVATE   仅作用与目标自身
    # PUBLIC    同时作用于目标自身与依赖该目标的目标
    # INTERFACE 仅作用于依赖该目标的目标
    target_include_directories(${target_name}
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:include>")

    # string(TOLOWER ${short_name} ShortName_lowercase)
    # string(TOLOWER ${CMAKE_PROJECT_NAME} proj_name_lowercase)

    # 安装头文件
    # install(FILES ${sdk_header_file_list} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${src_rel_dir})
    # install(DIRECTORY ${sdk_inc_dir} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${src_rel_dir})
    # message(----------${sdk_inc_dir})
    # message(----------${CMAKE_INSTALL_PREFIX}/include/${src_rel_dir})
    foreach(sdk_file ${sdk_header_file_list})
        get_filename_component(sdk_file_dir ${sdk_file} DIRECTORY)
        file(RELATIVE_PATH sdk_file_rel_dir ${sdk_inc_root_dir} ${sdk_file_dir})
        install(FILES ${sdk_file} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${sdk_file_rel_dir})
    endforeach()

    # 安装目标文件
    install(
        TARGETS ${target_name}
        EXPORT ${CMAKE_PROJECT_NAME}Targets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})



    message(--------AddLib:${target_alias})

endfunction()
