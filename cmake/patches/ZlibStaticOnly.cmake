# Patches the FetchContent-fetched zlib to build only the static library.
#
# madler/zlib's CMakeLists.txt unconditionally defines both a shared `zlib`
# and a static `zlibstatic` target with no option to disable either. This
# script removes the shared target so it no longer appears in the solution.
#
# Usage:
#   cmake -DZLIB_CMAKE_FILE=<path to zlib CMakeLists.txt> -P ZlibStaticOnly.cmake

if(NOT DEFINED ZLIB_CMAKE_FILE)
    message(FATAL_ERROR "ZLIB_CMAKE_FILE must point to zlib's CMakeLists.txt")
endif()
if(NOT EXISTS "${ZLIB_CMAKE_FILE}")
    message(FATAL_ERROR "ZLIB_CMAKE_FILE does not exist: ${ZLIB_CMAKE_FILE}")
endif()

file(READ "${ZLIB_CMAKE_FILE}" _content)

# Keep the original line-ending style (Windows checkouts may use CRLF).
set(_had_crlf OFF)
if(_content MATCHES "\r\n")
    set(_had_crlf ON)
endif()
string(REPLACE "\r\n" "\n" _content "${_content}")

if(NOT _content MATCHES "project\\(zlib")
    message(FATAL_ERROR "${ZLIB_CMAKE_FILE} does not look like zlib's CMakeLists.txt")
endif()

# Replace the shared-library block with the static-only equivalent.
set(_old [==[
add_library(zlib SHARED ${ZLIB_SRCS} ${ZLIB_DLL_SRCS} ${ZLIB_PUBLIC_HDRS} ${ZLIB_PRIVATE_HDRS})
target_include_directories(zlib PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
add_library(zlibstatic STATIC ${ZLIB_SRCS} ${ZLIB_PUBLIC_HDRS} ${ZLIB_PRIVATE_HDRS})
target_include_directories(zlibstatic PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(zlib PROPERTIES DEFINE_SYMBOL ZLIB_DLL)
set_target_properties(zlib PROPERTIES SOVERSION 1)

if(NOT CYGWIN)
    # This property causes shared libraries on Linux to have the full version
    # encoded into their final filename.  We disable this on Cygwin because
    # it causes cygz-${ZLIB_FULL_VERSION}.dll to be created when cygz.dll
    # seems to be the default.
    #
    # This has no effect with MSVC, on that platform the version info for
    # the DLL comes from the resource file win32/zlib1.rc
    set_target_properties(zlib PROPERTIES VERSION ${ZLIB_FULL_VERSION})
endif()

if(UNIX)
    # On unix-like platforms the library is almost always called libz
   set_target_properties(zlib zlibstatic PROPERTIES OUTPUT_NAME z)
   if(NOT APPLE AND NOT(CMAKE_SYSTEM_NAME STREQUAL AIX))
     set_target_properties(zlib PROPERTIES LINK_FLAGS "-Wl,--version-script,\"${CMAKE_CURRENT_SOURCE_DIR}/zlib.map\"")
   endif()
elseif(BUILD_SHARED_LIBS AND WIN32)
    # Creates zlib1.dll when building shared library version
    set_target_properties(zlib PROPERTIES SUFFIX "1.dll")
endif()

if(NOT SKIP_INSTALL_LIBRARIES AND NOT SKIP_INSTALL_ALL )
    install(TARGETS zlib zlibstatic
]==])

set(_new [==[
add_library(zlibstatic STATIC ${ZLIB_SRCS} ${ZLIB_PUBLIC_HDRS} ${ZLIB_PRIVATE_HDRS})
target_include_directories(zlibstatic PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

if(UNIX)
    # On unix-like platforms the library is almost always called libz
   set_target_properties(zlibstatic PROPERTIES OUTPUT_NAME z)
endif()

if(NOT SKIP_INSTALL_LIBRARIES AND NOT SKIP_INSTALL_ALL )
    install(TARGETS zlibstatic
]==])

string(REPLACE "${_old}" "${_new}" _content "${_content}")

if(_content MATCHES "add_library\\(zlib SHARED")
    message(FATAL_ERROR "zlib patch did not apply: the shared zlib target is still present")
endif()

if(_had_crlf)
    string(REPLACE "\n" "\r\n" _content "${_content}")
endif()
file(WRITE "${ZLIB_CMAKE_FILE}" "${_content}")

message(STATUS "zlib patched: only the static zlibstatic target is defined")
