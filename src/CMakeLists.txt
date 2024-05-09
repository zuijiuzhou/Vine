set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DUNICODE -D_UNICODE)

set(CMAKE_CXX_STANDARD 20)

include_directories(${CMAKE_SOURCE_DIR}/include)
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>)

# set(CMAKE_SHARED_LIBRARY_PREFIX vi)
# # Prefix all static libraries with 'vi'.
# set(CMAKE_STATIC_LIBRARY_PREFIX vi)

set(VI_SHARED_LIBRARY_PREFIX vi)

include(vine_library_helper)

add_subdirectory(ge)
add_subdirectory(core)
add_subdirectory(di)
add_subdirectory(system)
add_subdirectory(graphics)
add_subdirectory(appfw)
add_subdirectory(vine)
