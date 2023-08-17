#pragma once

#include "gui_global.h"
#include "core/Inherit.h"

VINE_APPFWGUI_NS_BEGIN

class RibbonBar;
class StatusBar;
class VINE_APPFWGUI_API MainWindow : public Inherit<Object, MainWindow>
{
public:
MainWindow();
virtual ~MainWindow();

public:
    void show();
    void close();

    RibbonBar* ribbonBar() const;
    StatusBar* statusBar() const;


private:
    struct Data;
    Data *d;
};

using MainWindowPtr = RefPtr<MainWindow>;

VINE_APPFWGUI_NS_END