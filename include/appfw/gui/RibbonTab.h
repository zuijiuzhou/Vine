#pragma once
#include "gui_global.h"

#include "Control.h"

VINE_APPFWGUI_NS_BEGIN

class VINE_APPFWGUI_API RibbonTab : public Control
{
    VI_OBJECT_META

    private:
    VI_DISABLE_COPY_MOVE(RibbonTab)
    
    public:
        RibbonTab(void* impl);

private:
    VI_OBJECT_DATA

};

VINE_APPFWGUI_NS_END
