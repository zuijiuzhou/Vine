#pragma once
#include <appfw/gui/gui_global.h>
#include <QPoint>
#include <QSize>
#include <appfw/gui/Gui.h>

VI_APPFWGUI_NS_BEGIN

class Convert{
public:
    static Point toPoint(const QPoint& pt);
    static QPoint toQPoint(const Point& pt);

    static Size toSize(const QSize& pt);
    static QSize toQSize(const Size& pt);
};

VI_APPFWGUI_NS_END