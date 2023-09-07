#include "Convert.h"

VI_APPFWGUI_NS_BEGIN

Point Convert::toPoint(const QPoint &pt)
{
    return Point(pt.x(), pt.y());
}
QPoint Convert::toQPoint(const Point &pt)
{
    return QPoint(pt.x, pt.y);
}

Size Convert::toSize(const QSize &s)
{
    return Size(s.width(), s.height());
}
QSize Convert::toQSize(const Size &s)
{
    return QSize(s.x, s.y);
}

VI_APPFWGUI_NS_END