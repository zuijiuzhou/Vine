#pragma once

#include "gui_global.h"

VINE_APPFWGUI_NS_BEGIN

class VINE_APPFWGUI_API Size
{
public:
    Size();
    Size(int w, int h);

public:
    int 
        w = 0,
        h = 0;
};

VINE_APPFWGUI_NS_END