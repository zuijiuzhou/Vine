#pragma once

#include "gui_global.h"
#include "core/Object.h"
#include "Control.h"

VI_APPFWGUI_NS_BEGIN

class VI_APPFWGUI_API DockPanel : public Control
{
    VI_OBJECT_META

private:
    VI_DISABLE_COPY_MOVE(DockPanel)

public:
    DockPanel();
    virtual ~DockPanel();
    
private:
    VI_OBJECT_DATA;
};

VI_APPFWGUI_NS_END