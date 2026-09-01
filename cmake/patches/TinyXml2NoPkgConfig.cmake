# Patches the FetchContent-fetched tinyxml2 to skip pkg-config generation.
#
# tinyxml2's CMakeLists.txt writes tinyxml2.pc with file(GENERATE ...), whose
# content differs per configuration type; under multi-config generators (MSVC)
# CMake rejects it with "Evaluation file to be written multiple times with
# different content". We never install tinyxml2 (it is baked into the
# RoboticsIO DLL), so the pkg-config block is removed.
#
# Usage:
#   cmake -DTINYXML2_CMAKE_FILE=<path to tinyxml2 CMakeLists.txt> -P TinyXml2NoPkgConfig.cmake

if(NOT DEFINED TINYXML2_CMAKE_FILE)
    message(FATAL_ERROR "TINYXML2_CMAKE_FILE must point to tinyxml2's CMakeLists.txt")
endif()
if(NOT EXISTS "${TINYXML2_CMAKE_FILE}")
    message(FATAL_ERROR "TINYXML2_CMAKE_FILE does not exist: ${TINYXML2_CMAKE_FILE}")
endif()

file(READ "${TINYXML2_CMAKE_FILE}" _content)

# Keep the original line-ending style (Windows checkouts may use CRLF).
set(_had_crlf OFF)
if(_content MATCHES "\r\n")
    set(_had_crlf ON)
endif()
string(REPLACE "\r\n" "\n" _content "${_content}")

if(NOT _content MATCHES "project\\(tinyxml2")
    message(FATAL_ERROR "${TINYXML2_CMAKE_FILE} does not look like tinyxml2's CMakeLists.txt")
endif()

set(_old [==[
## pkg-config

configure_file(cmake/tinyxml2.pc.in tinyxml2.pc.gen @ONLY)
file(GENERATE OUTPUT tinyxml2.pc INPUT "${CMAKE_CURRENT_BINARY_DIR}/tinyxml2.pc.gen")
install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/tinyxml2.pc"
    DESTINATION "${tinyxml2_INSTALL_PKGCONFIGDIR}"
    COMPONENT tinyxml2_development
)
]==])
string(REPLACE "${_old}" "" _content "${_content}")

if(_had_crlf)
    string(REPLACE "\n" "\r\n" _content "${_content}")
endif()

file(WRITE "${TINYXML2_CMAKE_FILE}" "${_content}")
message(STATUS "TinyXml2NoPkgConfig: removed pkg-config generation from ${TINYXML2_CMAKE_FILE}")
