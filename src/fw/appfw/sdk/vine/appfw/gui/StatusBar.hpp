#pragma once

#include <vine/appfw/appfw_global.hpp>

#include "Widget.hpp"

V_APPFWGUI_NS_BEGIN

class MainWindow;

class V_APPFW_API StatusBar : public Widget {
    V_OBJECT_META_DECL

    friend class MainWindow;

  public:
    StatusBar(MainWindow* wnd);
    virtual ~StatusBar();

  private:
    struct Data;
    Data* const d;
};

V_APPFWGUI_NS_END
