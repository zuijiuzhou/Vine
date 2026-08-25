#pragma once

#include "appfw_global.hpp"

V_APPFW_NS_BEGIN

class Application;
class ConfigRegistry;

/**
 * \brief 插件加载上下文：Addin::load() 时传入，暴露宿主能力。
 *
 * 插件在 load() 里经 configs() 取得配置注册表并注册可显示配置项。
 * 后续可继续补 commandManager()/serviceManager() 等访问器。
 */
class V_APPFW_API AddinLoadContext {
  public:
    /// 以 Application 为宿主构造加载上下文。
    explicit AddinLoadContext(Application* app);
    ~AddinLoadContext();

    /// 配置注册表：插件在此注册配置项（ConfigItem）。
    ConfigRegistry* configs() const;

  private:
    struct Data;
    Data* const d;
};

V_APPFW_NS_END
