#pragma once

#include "gui_global.h"
#include "core/Inherit.h"

VINE_APPFWGUI_BEGIN

class VINE_APPFWGUI_API MainWindow : public Inherit<Object, MainWindow>
{
public:
MainWindow();
virtual ~MainWindow();

public:
    void show();
    void close();

private:
    struct Data;
    Data *d;
};

using MainWindowPtr = RefPtr<MainWindow>;

VINE_APPFWGUI_NS_END