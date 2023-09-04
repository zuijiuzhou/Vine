#include <appfw/gui/Rect.h>

VINE_APPFWGUI_NS_BEGIN

Rect::Rect()
{
}
Rect::Rect(int x, int y, int w, int h)
    : x(x), y(y), w(w), h(h)
{
    
}

Point Rect::poistion() const{
    return Point(x, y);
}

Size Rect::size() const{
    return Size(w, h);
}

VINE_APPFWGUI_NS_END