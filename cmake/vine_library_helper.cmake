function(vine_internal_add_library short_name out_target_name)
    file(GLOB SRC_FILE_LIST LIST_DIRECTORIES false *.cpp)
    file(GLOB HEADER_FILE_LIST LIST_DIRECTORIES false *.h)
    file(GLOB RC_FILE_LIST LIST_DIRECTORIES false *.rc)

    set(LIB_FULL_NAME ${VINE_SHARED_LIBRARY_PREFIX}${short_name})
    set(LIB_ALIAS ${VINE_SHARED_LIBRARY_PREFIX}::${short_name})
    message(----------------${LIB_ALIAS})
    add_library(${LIB_FULL_NAME} SHARED ${SRC_FILE_LIST} ${HEADER_FILE_LIST} ${RC_FILE_LIST})
    add_library(${LIB_ALIAS} ALIAS ${LIB_FULL_NAME})
    set_target_properties(${LIB_FULL_NAME} PROPERTIES OUTPUT_NAME ${LIB_FULL_NAME})
    
    string(TOUPPER VINE_${short_name}_LIB LIB_COMPILE_DEF)
    target_compile_definitions(${LIB_FULL_NAME} PRIVATE ${LIB_COMPILE_DEF})

    if ("${out_target_name}" STREQUAL "")   
    else ()
        set(${out_target_name} ${LIB_FULL_NAME} PARENT_SCOPE)
    endif ()

endfunction(vine_internal_add_library target)
