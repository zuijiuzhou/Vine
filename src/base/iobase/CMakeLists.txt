v_add_library(IOBASE_TARGET_NAME IOBase)

include(FetchContent)
# madler/zlib always defines both a shared and a static target; the patch
# removes the shared one so only zlibstatic is built and listed.
FetchContent_Declare(zlib
    GIT_REPOSITORY https://github.com/madler/zlib.git
    GIT_TAG        v1.3.1
    PATCH_COMMAND  "${CMAKE_COMMAND}"
                    -DZLIB_CMAKE_FILE=${CMAKE_BINARY_DIR}/_deps/zlib-src/CMakeLists.txt
                    -P ${CMAKE_SOURCE_DIR}/cmake/patches/ZlibStaticOnly.cmake
)
set(ZLIB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # No example executables.
FetchContent_MakeAvailable(zlib)

# libzip's find_package(ZLIB) must resolve to the zlib built above. FindZLIB
# skips its search when ZLIB_LIBRARY is preset and keeps an existing ZLIB::ZLIB
# target, so point both at zlibstatic.
set(ZLIB_INCLUDE_DIR "${zlib_SOURCE_DIR}" CACHE PATH "" FORCE)
set(ZLIB_LIBRARY zlibstatic CACHE FILEPATH "" FORCE)
if(NOT TARGET ZLIB::ZLIB)
    add_library(ZLIB::ZLIB ALIAS zlibstatic)
endif()

# libzip-ng: static zip library only (no tools, tests, fuzzers, examples,
# documentation or install rules).
FetchContent_Declare(libzip
    GIT_REPOSITORY https://github.com/nih-at/libzip.git
    GIT_TAG        v1.11.4
)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE) # Static lib, baked into the DLL.
set(BUILD_TOOLS OFF CACHE BOOL "" FORCE)       # No zipcmp/zipmerge/ziptool.
set(BUILD_REGRESS OFF CACHE BOOL "" FORCE)     # No regression tests.
set(BUILD_OSSFUZZ OFF CACHE BOOL "" FORCE)     # No fuzzers.
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)    # No examples.
set(BUILD_DOC OFF CACHE BOOL "" FORCE)         # No documentation.
set(LIBZIP_DO_INSTALL OFF CACHE BOOL "" FORCE) # No install rules.
set(ENABLE_BZIP2 OFF CACHE BOOL "" FORCE)      # No extra codecs.
set(ENABLE_LZMA OFF CACHE BOOL "" FORCE)
set(ENABLE_ZSTD OFF CACHE BOOL "" FORCE)
set(ENABLE_OPENSSL OFF CACHE BOOL "" FORCE)    # Windows crypto covers AES.
set(ENABLE_GNUTLS OFF CACHE BOOL "" FORCE)
set(ENABLE_COMMONCRYPTO OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(libzip)

# Group every third-party target under the third_party solution folder.
v_group_targets_folder(${zlib_SOURCE_DIR} third_party)
v_group_targets_folder(${libzip_SOURCE_DIR} third_party)

target_link_libraries(${IOBASE_TARGET_NAME} PUBLIC vi::Global vi::Core)
# Static zlib and libzip are baked into the IOBase DLL.
target_link_libraries(${IOBASE_TARGET_NAME} PRIVATE zlibstatic zip)
