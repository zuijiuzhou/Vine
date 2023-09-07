#include <appfw/gui/StatusBar.h>

#include <QStatusBar>
#include <appfw/gui/MainWindow.h>

VI_APPFWGUI_NS_BEGIN

VI_OBJECT_META_IMPL(StatusBar, UIElement)

struct StatusBar::Data
{
    MainWindow* wnd;
};

StatusBar::StatusBar(MainWindow* wnd)
    : Control(new QStatusBar())
    , d(new Data())
{
    d->wnd = wnd;
}

VI_APPFWGUI_NS_END