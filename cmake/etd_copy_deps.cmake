# 定义函数来自动拷贝依赖项
function(copy_qt_dependencies target)
    # 获取目标可执行文件的路径
    get_target_property(target_output_directory ${target} RUNTIME_OUTPUT_DIRECTORY)

    # 获取Qt的bin目录路径
    set(QT_BIN_DIR $ENV{QT6_DIR}/bin)
    
    # 构建拷贝命令
    set(copy_command "${CMAKE_COMMAND} -E copy_if_different ")
    foreach(module IN LISTS ARGV)
        set(module_path "${QT_BIN_DIR}/${module}.dll ")
        list(APPEND copy_command ${module_path})
    endforeach()
    message(---${copy_command})
    # 添加自定义命令
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${copy_command}
        DEPENDS ${ARGV}
        COMMENT "Copying Qt dependencies to output directory"
    )
endfunction()