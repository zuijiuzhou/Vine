#pragma once

#include <vector>

#include "UIElement.hpp"
#include "Gui.hpp"

V_APPFWGUI_NS_BEGIN

class RibbonBar;
class StatusBar;
class DockPanelManager;

class V_APPFW_API MainWindow : public UIElement {
    V_OBJECT_META_DECL

    friend class RibbonBar;
    friend class StatusBar;

  public:
    MainWindow();
    virtual ~MainWindow();

  public:
    void            startupPosition(StartupPosition position);
    StartupPosition startupPosition() const;

    void        windowState(WindowState state);
    WindowState windowState() const;

    void activate();
    void setEnabled();
    void setDisabled();

    bool isActive() const;
    bool isEnabled() const;

    void show();
    void close();

    RibbonBar* ribbonBar() const;
    StatusBar* statusBar() const;
    DockPanelManager* dockPanelManager() const;

  private:
    struct Data;
    Data*       dptr();
    const Data* dptr() const;
};

V_APPFWGUI_NS_END
