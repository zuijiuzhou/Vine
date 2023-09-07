#pragma once

#include "gui_global.h"

#include "Control.h"

VI_APPFWGUI_NS_BEGIN

class VI_APPFWGUI_API StatusBar : public Control
{
    VI_OBJECT_META

friend class MainWindow;

private:
    VI_DISABLE_COPY_MOVE(StatusBar)

public:
    StatusBar(MainWindow* wnd);

private:
    VI_OBJECT_DATA
};

VI_APPFWGUI_NS_END