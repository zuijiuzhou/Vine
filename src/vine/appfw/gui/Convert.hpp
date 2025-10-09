#pragma once
#include <QDockWidget>
#include <QPoint>
#include <QSize>

#include <vine/appfw/gui/Gui.hpp>
#include <vine/appfw/gui/gui_global.hpp>

VI_APPFWGUI_NS_BEGIN

class Convert {
  public:
    static Point  toPoint(const QPoint& pt);
    static QPoint toQPoint(const Point& pt);

    static Size  toSize(const QSize& pt);
    static QSize toQSize(const Size& pt);

    static Qt::DockWidgetAreas toQDockAreas(DockAreas areas);
    static DockAreas           toDockAreas(Qt::DockWidgetAreas qareas);

    static QDockWidget::DockWidgetFeatures toQDockFeatures(DockFeatures features);
    static DockFeatures                    toDockFeatures(QDockWidget::DockWidgetFeatures qfeatures);
};

VI_APPFWGUI_NS_END