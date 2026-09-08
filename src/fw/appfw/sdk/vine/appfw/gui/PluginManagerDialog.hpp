#pragma once

#include <vine/appfw/PluginManager.hpp>

#include <vine/appfw/gui/Window.hpp>

V_APPFWGUI_NS_BEGIN

/**
 * @brief Plugin manager dialog: left plugin list, right detail page.
 *
 * The left pane lists the loaded plugins; selecting one shows its detail on
 * the right: static metadata (description, vendor, dependencies, library path)
 * plus the commands and config items it registered. A right-click menu offers
 * "view details" and loading a plugin library from disk (QFileDialog).
 */
class V_APPFW_API PluginManagerDialog : public Window {
    V_OBJECT_META_DECL;

  public:
    explicit PluginManagerDialog(vine::appfw::PluginManager* manager);
    ~PluginManagerDialog() override;

  public:
    /// Rebuilds the loaded-plugin list from the manager.
    void refresh();

  private:
    /// Opens a file picker and loads the selected plugin library.
    void loadPlugin();

    /// Fills the right-hand detail page for the given plugin (empty hides it).
    void showDetail(const vine::String& name);

  private:
    struct Impl;
    Impl*       dptr();
    const Impl* dptr() const;
};

V_APPFWGUI_NS_END
