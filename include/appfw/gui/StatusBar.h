#pragma once

#include "gui_global.h"

#include "UIElement.h"

VINE_APPFWGUI_NS_BEGIN

class VINE_APPFWGUI_API StatusBar : public UIElement
{
    VI_OBJECT_META

private:
    VI_DISABLE_COPY_MOVE(StatusBar)

public:
    StatusBar();

private:
    VI_OBJECT_DATA
};

VINE_APPFWGUI_NS_END