#pragma once

#include "gui_global.h"

#include "UIElement.h"

VI_APPFWGUI_NS_BEGIN

class VI_APPFWGUI_API RibbonDropDownItem : public UIElement
{
    VI_OBJECT_META
    friend class RibbonBar;

private:
    VI_DISABLE_COPY_MOVE(RibbonDropDownItem)

public:
    RibbonDropDownItem();
    virtual ~RibbonDropDownItem();

public:
    String text() const;
    RibbonDropDownItem *text(const String &txt);

    void *data() const;
    RibbonDropDownItem *data(void *v);

private:
    VI_OBJECT_DATA
};

VI_APPFWGUI_NS_END