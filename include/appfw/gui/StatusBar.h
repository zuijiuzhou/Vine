#pragma once

#include "gui_global.h"

#include "Control.h"

VI_APPFWGUI_NS_BEGIN

class MainWindow;
class VI_APPFWGUI_API StatusBar : public Control {
    VI_OBJECT_META

    friend class MainWindow;

  public:
    StatusBar(MainWindow* wnd);
    virtual ~StatusBar();

  private:
    VI_OBJECT_DATA
};

VI_APPFWGUI_NS_END