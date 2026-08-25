#pragma once

#include <vine/appfw/ConfigManager.hpp>
#include <vine/appfw/ConfigRegistry.hpp>

#include "Window.hpp"

V_APPFWGUI_NS_BEGIN

/**
 * \brief 配置窗口：按注册表（ConfigRegistry）条目渲染编辑器，值存取 ConfigManager。
 *
 * 每条目按类型生成编辑器（String→QLineEdit、Bool→QCheckBox、Int→QSpinBox、
 * Double→QDoubleSpinBox、Choice→QComboBox），按 group 分组展示。
 * 编辑即写回 ConfigManager（changed 事件随之触发）；提供 refresh() 从存储重载、
 * reset() 恢复默认值。继承 Window，可 show() 非模态或 exec() 模态弹窗显示。
 */
class V_APPFW_API ConfigWindow : public Window {
    V_OBJECT_META_DECL

  public:
    /// 以 registry 条目为模板、config 为数据源构建窗口内容。
    ConfigWindow(ConfigRegistry* registry, ConfigManager* config);
    ~ConfigWindow() override;

  public:
    /// 从 ConfigManager 重新加载所有编辑器的显示值。
    void refresh();
    /// 恢复默认：有默认值者写回默认值，无默认值者移除该 key。
    void reset();

    /// 关联的注册表。
    ConfigRegistry* registry() const;
    /// 关联的配置管理器。
    ConfigManager* config() const;

  private:
    struct Data;
    Data*       dptr();
    const Data* dptr() const;
};

V_APPFWGUI_NS_END
