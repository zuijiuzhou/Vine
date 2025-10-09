#pragma once

#include "Control.hpp"
#include "gui_global.hpp"

VI_APPFWGUI_NS_BEGIN

class RibbonBar;
class StatusBar;
class DockPanel;
class VI_APPFWGUI_API MainWindow : public Control {
    VI_OBJECT_META

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

    void addDockPanel(DockPanel* panel, DockAreas area);

  private:
    VI_OBJECT_DATA
};

using MainWindowPtr = RefPtr<MainWindow>;

VI_APPFWGUI_NS_END