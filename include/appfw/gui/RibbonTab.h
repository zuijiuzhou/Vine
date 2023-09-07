#pragma once
#include "gui_global.h"

#include "Control.h"

VI_APPFWGUI_NS_BEGIN

class VI_APPFWGUI_API RibbonTab : public Control
{
    VI_OBJECT_META

friend class RibbonBar;

private:
    VI_DISABLE_COPY_MOVE(RibbonTab)

public:
    RibbonTab(void *impl);

private:
    VI_OBJECT_DATA
};

VI_APPFWGUI_NS_END
