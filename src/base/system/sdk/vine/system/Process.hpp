#include "system_global.hpp"

V_SYSTEM_NS_BEGIN

class V_SYSTEM_API Process {
  public:
    /**
     * @brief Get the current process ID.
     * @return Process ID.
     */
    static int getCurrentProcessId();
};

V_SYSTEM_NS_END
