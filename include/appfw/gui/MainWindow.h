#pragma once

#include "gui_global.h"
#include "core/Inherit.h"
#include "Control.h"

VINE_APPFWGUI_NS_BEGIN

class RibbonBar;
class StatusBar;
class VINE_APPFWGUI_API MainWindow : public Control
{

VI_OBJECT_META

friend class RibbonBar;
friend class StatusBar;

public:
MainWindow();
virtual ~MainWindow();

public:
    void show();
    void close();

    RibbonBar* ribbonBar() const;
    StatusBar* statusBar() const;

private:
    VI_OBJECT_DATA
};

using MainWindowPtr = RefPtr<MainWindow>;

VINE_APPFWGUI_NS_END