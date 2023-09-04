#pragma once

#include "Size.h"
#include "Point.h"

VINE_APPFWGUI_NS_BEGIN

class VINE_APPFWGUI_API Rect
{
public:
    Rect();
    Rect(int x, int y, int w, int h);

public:
    Point poistion() const;
    Size size() const;
    
public:
    int 
        x = 0,
        y = 0,
        w = 0,
        h = 0;
};

VINE_APPFWGUI_NS_END