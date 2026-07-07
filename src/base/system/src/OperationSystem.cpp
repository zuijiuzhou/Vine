#include <vine/system/OperationSystem.hpp>

V_SYSTEM_NS_BEGIN

namespace {
    
}

const OperationSystemInfo& OperationSystem::getInfo()
{
    static OperationSystemInfo info;
    static bool                initialized = false;

    if (!initialized) {
// Platform-specific code to populate the info struct
// For example, on Windows:
#ifdef _WIN32
        info.name         = "Windows";
        info.version      = "10.0"; // This should be retrieved dynamically
        info.architecture = sizeof(void*) == 8 ? "x64" : "x86";
#elif __APPLE__
        info.name         = "macOS";
        info.version      = "11.0"; // This should be retrieved dynamically
        info.architecture = sizeof(void*) == 8 ? "x64" : "x86";
#elif __linux__
        info.name         = "Linux";
        info.version      = "5.4"; // This should be retrieved dynamically
        info.architecture = sizeof(void*) == 8 ? "x64" : "x86";
#else
        info.name         = "Unknown OS";
        info.version      = "Unknown Version";
        info.architecture = sizeof(void*) == 8 ? "x64" : "x86";
#endif

        initialized = true;
    }

    return info;
}

V_SYSTEM_NS_END