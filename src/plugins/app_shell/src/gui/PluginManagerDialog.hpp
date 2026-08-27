#pragma once

#include <vine/appfw/PluginManager.hpp>

#include <vine/appfw/gui/Window.hpp>

V_APPFWGUI_NS_BEGIN

/**
 * @brief Plugin manager dialog: lists the currently loaded plugins and allows
 * loading a plugin library from disk (QFileDialog). Loaded plugins appear in
 * the list immediately; a Refresh button re-queries the manager.
 */
class PluginManagerDialog : public Window {
    V_OBJECT_META_DECL;

  public:
    explicit PluginManagerDialog(vine::appfw::PluginManager* manager);
    ~PluginManagerDialog() override;

  public:
    /// Rebuilds the loaded-plugin list from the manager.
    void refresh();

  private:
    struct Data;
    Data*       dptr();
    const Data* dptr() const;
};

V_APPFWGUI_NS_END
