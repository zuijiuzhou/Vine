#pragma once

#include "gui_global.h"

VINE_APPFWGUI_NS_BEGIN

class VINE_APPFWGUI_API Point
{
public:
    Point();
    Point(int x, int y);

public:
    int 
        x = 0,
        y = 0;
};

VINE_APPFWGUI_NS_END