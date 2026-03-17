#pragma once

#include "Widget.hpp"

V_APPFWGUI_NS_BEGIN

class RibbonBar;
class StatusBar;
class DockPanel;

class V_APPFW_API MainWindow : public Widget {
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

    void addDockPanel(DockPanel* panel, DockAreas area);

  private:
    struct Data;
    Data* const d;
};

using MainWindowPtr = RefPtr<MainWindow>;

V_APPFWGUI_NS_END
