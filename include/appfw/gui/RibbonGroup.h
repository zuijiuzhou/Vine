#pragma once
#include "gui_global.h"

#include "Control.h"

VI_APPFWGUI_NS_BEGIN

class RibbonTab;

class VI_APPFWGUI_API RibbonGroup : public Control
{
    VI_OBJECT_META

    friend class RibbonTab;

private:
    VI_DISABLE_COPY_MOVE(RibbonGroup)

public:
    RibbonGroup();
    virtual ~RibbonGroup();

public:
    String title() const;
    RibbonGroup *title(const String &ti);

private:
    VI_OBJECT_DATA
};

VI_APPFWGUI_NS_END
