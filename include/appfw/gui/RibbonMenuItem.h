#pragma once

#include "gui_global.h"

#include "UIElement.h"

VI_APPFWGUI_NS_BEGIN

class VI_APPFWGUI_API RibbonMenuItem : public UIElement
{
    VI_OBJECT_META
    friend class RibbonBar;

private:
    VI_DISABLE_COPY_MOVE(RibbonMenuItem)

public:
    RibbonMenuItem();
    virtual ~RibbonMenuItem();

public:
    String text() const;
    RibbonMenuItem *text(const String &txt);

    void *data() const;
    RibbonMenuItem *data(void *v);

private:
    VI_OBJECT_DATA
};

VI_APPFWGUI_NS_END