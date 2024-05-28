#pragma once

#include "gui_global.h"
#include "core/Inherit.h"
#include "Control.h"

VI_APPFWGUI_NS_BEGIN

class RibbonBar;
class StatusBar;
class VI_APPFWGUI_API MainWindow : public Control
{
    VI_OBJECT_META

    friend class RibbonBar;
    friend class StatusBar;
public:
    enum StartupPosition {
        MANUAL,
        CENTER_SCREEN
    };

    enum WindowState{
        NORMAL,
        MINIMIZED,
        MAXIMIZED,
    };

public:
    MainWindow();
    virtual ~MainWindow();

public:
    MainWindow* startupPosition(StartupPosition position);
    StartupPosition startupPosition() const;

    MainWindow* windowState(WindowState state);
    WindowState windowState() const;
    
    MainWindow* activate();
    MainWindow* setEnabled();
    MainWindow* setDisabled();

    bool isActive() const;
    bool isEnabled() const;

    void show();
    void close();

    RibbonBar *ribbonBar() const;
    StatusBar *statusBar() const;

private:
    VI_OBJECT_DATA
};

using MainWindowPtr = RefPtr<MainWindow>;

VI_APPFWGUI_NS_END