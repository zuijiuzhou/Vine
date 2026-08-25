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
    RibbonBar*        ribbonBar() const;
    StatusBar*        statusBar() const;
    DockPanelManager* dockPanelManager() const;

  private:
    struct Data;
    Data*       dptr();
    const Data* dptr() const;
};

V_APPFWGUI_NS_END
