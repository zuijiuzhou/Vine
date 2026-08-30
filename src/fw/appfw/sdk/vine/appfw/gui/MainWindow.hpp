#pragma once

#include <vector>

#include "Gui.hpp"
#include "Window.hpp"

V_APPFWGUI_NS_BEGIN

class RibbonBar;
class StatusBar;
class DockPanelManager;

class V_APPFW_API MainWindow : public Window {
    V_OBJECT_META_DECL

    friend class RibbonBar;
    friend class StatusBar;

  public:
    MainWindow();
    ~MainWindow() override;

  public:
    /**
     * @brief Returns the currently shown main window, or nullptr if none.
     *
     * @return The main window instance.
     */
    static MainWindow* current();

  public:
    RibbonBar*        ribbonBar() const;
    StatusBar*        statusBar() const;
    DockPanelManager* dockPanelManager() const;

  private:
    struct Impl;
    Impl*       dptr();
    const Impl* dptr() const;
};

V_APPFWGUI_NS_END
