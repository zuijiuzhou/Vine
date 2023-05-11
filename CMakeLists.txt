cmake_minimum_required(VERSION "3.2")

project(Etd 
VERSION "1.0.0" 
DESCRIPTION "Everything 3d"
HOMEPAGE_URL "https://github.com/zuijiuzhou/Etd"
LANGUAGES C CXX)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_definitions(-DUNICODE -D_UNICODE)
set(CMAKE_CXX_STANDARD 20)
include_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

add_subdirectory(src)

enable_testing()
add_subdirectory(tests)