#pragma once

#include "gui_global.h"

VINE_APPFWGUI_BEGIN

class VINE_APPFWGUI_API MainWindow
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

VINE_APPFWGUI_NS_END