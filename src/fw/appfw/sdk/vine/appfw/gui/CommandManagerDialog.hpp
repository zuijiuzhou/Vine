#pragma once

#include <vine/appfw/CommandManager.hpp>

#include <vine/appfw/gui/Window.hpp>

V_APPFWGUI_NS_BEGIN

/**
 * @brief Command manager dialog: lists registered commands (name, aliases,
 * source plugin, group, description) with a filter, and allows unregistering
 * a command.
 */
class V_APPFW_API CommandManagerDialog : public Window {
    V_OBJECT_META_DECL;

  public:
    explicit CommandManagerDialog(vine::appfw::CommandManager* manager);
    ~CommandManagerDialog() override;

  public:
    /**
     * @brief Rebuilds the command table from the manager, applying the current
     * filter.
     */
    void refresh();

  private:
    void applyFilter();
    void unregisterSelected();

    struct Impl;
    Impl*       dptr();
    const Impl* dptr() const;
};

V_APPFWGUI_NS_END
