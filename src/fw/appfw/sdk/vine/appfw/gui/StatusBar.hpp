#pragma once

#include "Widget.hpp"

VI_APPFWGUI_NS_BEGIN

class MainWindow;
class VI_APPFW_API StatusBar : public Widget {
    VI_OBJECT_META

    friend class MainWindow;

  public:
    StatusBar(MainWindow* wnd);
    virtual ~StatusBar();

  private:
    VI_OBJECT_DATA
};

VI_APPFWGUI_NS_END