#pragma once
#include "system_global.hpp"

#include <string>

V_SYSTEM_NS_BEGIN

struct V_SYSTEM_API OperationSystemInfo {
    std::string name;
    std::string version;
    std::string architecture;
};

class V_SYSTEM_API OperationSystem {
  public:
    static const OperationSystemInfo& getInfo();
};

V_SYSTEM_NS_END
