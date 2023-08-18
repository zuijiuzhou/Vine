#pragma once

#include "gui_global.h"
#include "core/Object.h"
#include "UIElement.h"

VINE_APPFWGUI_NS_BEGIN

class VINE_APPFWGUI_API DockPanel : public Object
{
    VI_OBJECT_META

private:
    VI_DISABLE_COPY_MOVE(DockPanel)

public:
    DockPanel();

private:
    VI_OBJECT_DATA;
};

VINE_APPFWGUI_NS_END