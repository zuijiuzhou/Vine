cmake_minimum_required(VERSION "3.20")

project(
Vine
VERSION "1.0.0" 
DESCRIPTION "Vine 3d modeling"
HOMEPAGE_URL "https://github.com/zuijiuzhou/Vine"
LANGUAGES C CXX)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_definitions(-DUNICODE -D_UNICODE)
add_definitions(-DVINE_NS_NAME=vine)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

add_subdirectory(src)

include(cmake/vine_deploy.cmake)

enable_testing()
add_subdirectory(tests)