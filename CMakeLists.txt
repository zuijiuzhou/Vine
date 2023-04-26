cmake_minimum_required(VERSION "3.2")
project(Etd 
VERSION "1.0.0" 
DESCRIPTION "Everything 3d"
HOMEPAGE_URL "https://github.com/zuijiuzhou/Etd"
LANGUAGES C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(src)