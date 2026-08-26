#pragma once

#include "appfw_global.hpp"

V_APPFW_NS_BEGIN

class Application;
class ConfigRegistry;

/**
 * @brief Addin load context: passed to Addin::load(), exposing host capabilities.
 *
 * Inside load(), the addin obtains the config registry via configs() and
 * registers displayable config items there. More accessors such as
 * commandManager()/serviceManager() can be added later.
 */
class V_APPFW_API AddinLoadContext {
  public:
    /// Constructs the load context with Application as the host.
    explicit AddinLoadContext(Application* app);
    ~AddinLoadContext();

    /// Config registry: addins register config items (ConfigItem) here.
    ConfigRegistry* configs() const;

  private:
    struct Data;
    Data* const d;
};

V_APPFW_NS_END
