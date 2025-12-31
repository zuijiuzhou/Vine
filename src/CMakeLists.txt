
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DUNICODE -D_UNICODE)


# include_directories(
#     "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>")

if(WIN32)
    #link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>)
endif()

# set(CMAKE_SHARED_LIBRARY_PREFIX vi)
# # Prefix all static libraries with 'vi'.
# set(CMAKE_STATIC_LIBRARY_PREFIX vi)

set(VI_SHARED_LIBRARY_PREFIX vi)

include(VineLibraryHelper)

add_subdirectory(base)
add_subdirectory(viz)
add_subdirectory(fw)
add_subdirectory(app)

file(GLOB VI_GLOBAL_HEADER_FILES LIST_DIRECTORIES false ${CMAKE_SOURCE_DIR}/include/vine/*.h)

install(FILES ${VI_GLOBAL_HEADER_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/vine)
