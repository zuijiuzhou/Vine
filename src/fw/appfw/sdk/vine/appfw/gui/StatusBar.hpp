#pragma once

#include "Control.hpp"

V_APPFWGUI_NS_BEGIN

class V_APPFW_API StatusBar : public Control {
    V_OBJECT_META_DECL

  public:
    StatusBar();
    StatusBar(UIElement* parent);
    virtual ~StatusBar();

    void showMessage(const String& msg, int timeout_ms = 0);

  private:
    struct Data;
    Data*       dptr();
    const Data* dptr() const;
};

V_APPFWGUI_NS_END
