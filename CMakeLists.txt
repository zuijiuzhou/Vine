cmake_minimum_required(VERSION "3.20")

project(
    Vine
    VERSION "1.0.0" 
    DESCRIPTION "Vine 3d modeling"
    HOMEPAGE_URL "https://github.com/zuijiuzhou/Vine"
    LANGUAGES C CXX)

option(VI_DEPLOY_WHEN_BUILDING "Whether to run the deploy command when building." OFF)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# make cache variables for install destinations
include(GNUInstallDirs)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_DEBUG_POSTFIX d)
set(CMAKE_RELEASE_POSTFIX "")
set(CMAKE_RELWITHDEBINFO_POSTFIX "")
set(CMAKE_MINSIZEREL_POSTFIX "")

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake)

add_definitions(-DUNICODE -D_UNICODE)
add_definitions(-DVI_ROOT_NS=vine)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(VineThirdParty)

# link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory(src)

if(VI_DEPLOY_WHEN_BUILDING)
include(VineDeploy)
endif()

enable_testing()
add_subdirectory(tests)

include(CMakePackageConfigHelpers)

message(${CMAKE_CURRENT_BINARY_DIR})
message(${CMAKE_BINARY_DIR})
configure_package_config_file(${CMAKE_SOURCE_DIR}/cmake/VineConfig.cmake.in
  "${CMAKE_BINARY_DIR}/VineConfig.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/VineConfig
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