cmake_minimum_required(VERSION 3.20)

project(
    Vine
    VERSION "1.0.0"
    DESCRIPTION "Vine 3d modeling"
    HOMEPAGE_URL "https://github.com/zuijiuzhou/Vine"
    LANGUAGES C CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# CMAKE_INSTALL_BINDIR: Executable programs
include(GNUInstallDirs)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_DEBUG_POSTFIX d)
set(CMAKE_RELEASE_POSTFIX "")
set(CMAKE_RELWITHDEBINFO_POSTFIX "")
set(CMAKE_MINSIZEREL_POSTFIX "")
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# UNIX, WIN32, LINUX, APPLE, ANDROID, IOS, MACOS, etc. 目标平台类型(不是构建平台类型)，与编译器无关
# MSVC, GNU, Clang, AppleClang, etc. 编译器类型，与目标平台无关
# CMAKE_HOST_SYSTEM_NAME, CMAKE_HOST_SYSTEM_VERSION, CMAKE_HOST_SYSTEM_PROCESSOR 主机系统信息(编译器所在的系统)
# CMAKE_SYSTEM_NAME, CMAKE_SYSTEM_VERSION, CMAKE_SYSTEM_PROCESSOR 目标系统信息(编译器要生成的程序运行的系统)

if(UNIX AND NOT ANDROID)
    # 设置RUNPATH的值，优先级低于LD_LIBRARY_PATH
    # set(CMAKE_BUILD_RPATH "$ORIGIN")
    set(CMAKE_INSTALL_RPATH
        $ORIGIN
        $ORIGIN/../lib)
    # RUNPATH使用相对于ORIGIN的路径
    set(CMAKE_BUILD_RPATH_USE_ORIGIN ON)
    # BUILD附加CMAKE_INSTALL_RPATH
    set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
    # 安装时添加link_directories target_link_directories的路径到RUNPATH
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH OFF)
endif()

if(WIN32)
    add_definitions(-DUNICODE -D_UNICODE)
endif()

if(MSVC)
    # equivalent to /source-charset:utf-8 /execution-charset:utf-8
    add_compile_options(/utf-8)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake)
include(VineThirdParty)

# link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(src)

if(WIN32)
    include(VineDeployQt)
endif()

enable_testing()
add_subdirectory(tests)

include(CMakePackageConfigHelpers)

message(${CMAKE_CURRENT_BINARY_DIR})
message(${CMAKE_BINARY_DIR})

configure_package_config_file(${CMAKE_SOURCE_DIR}/cmake/VineConfig.cmake.in
    "${CMAKE_BINARY_DIR}/VineConfig.cmake" # The output file
    INSTALL_DESTINATION lib/cmake/vine # The install location for the generated config file, relative to CMAKE_INSTALL_PREFIX
)
install(
    FILES
    "${CMAKE_BINARY_DIR}/VineConfig.cmake"
    #"${CMAKE_BINARY_DIR}/VineConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vine
)
install(EXPORT VineTargets
    NAMESPACE vi::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vine
)
