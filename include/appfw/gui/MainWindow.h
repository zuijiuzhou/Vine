#pragma once

#include "appfw_gui_export.h"

ETD_APPFW_GUI_BEGIN

class ETD_APPFW_GUI_API MainWindow
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

ETD_APPFW_GUI_NS_END