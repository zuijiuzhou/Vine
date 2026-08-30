v_add_library(MODELIO_TARGET_NAME ModelIo)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(FetchContent)

if(VINE_USE_FETCHCONTENT)
    # assimp is baked into the ModelIo DLL as a static library. Only the library
    # itself is built: no samples, no tests, no CLI tools, no viewer.
    FetchContent_Declare(assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG        v5.4.3
        GIT_SHALLOW    TRUE
    )
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE) # Static lib, baked into the DLL.
    set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
    set(ASSIMP_BUILD_ASSIMP_VIEW OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(assimp)

    # assimp builds itself with -Wall -Werror (code/CMakeLists.txt); clang flags
    # some memcpy calls as errors (-Wnontrivial-memcall) that GCC does not, so
    # downgrade all warnings back to non-errors for the vendored library.
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(assimp PRIVATE -Wno-error)
    endif()

    v_group_targets_folder(${assimp_SOURCE_DIR} third_party)
else()
    # Consume assimp from vcpkg or a local install.
    find_package(assimp CONFIG REQUIRED)
endif()

target_link_libraries(${MODELIO_TARGET_NAME} PUBLIC vi::Geometry)
target_link_libraries(${MODELIO_TARGET_NAME} PRIVATE assimp::assimp)
