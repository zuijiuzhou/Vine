include(FetchContent)

FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.15.1
)
set(SPDLOG_INSTALL OFF CACHE BOOL "" FORCE)
set(SPDLOG_FMT_EXTERNAL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(spdlog)

v_group_targets_folder(${spdlog_SOURCE_DIR} third_party)

v_add_library(LOGGING_TARGET_NAME Logging)

target_link_libraries(${LOGGING_TARGET_NAME} PUBLIC vi::Global)
target_link_libraries(${LOGGING_TARGET_NAME} PRIVATE spdlog::spdlog_header_only)
