#include <vine/appfw/gui/StatusBar.hpp>

#include <QStatusBar>

#include <vine/Ptr.hpp>
#include <vine/appfw/gui/MainWindow.hpp>

V_APPFWGUI_NS_BEGIN

V_OBJECT_META_IMPL(StatusBar, Widget)

struct StatusBar::Data {
    RefPtr<MainWindow> wnd;
};

StatusBar::StatusBar(MainWindow* wnd)
  : Widget(new QStatusBar())
  , d(new Data())
{
    d->wnd = wnd;
}

StatusBar::~StatusBar()
{
    delete d;
}

V_APPFWGUI_NS_END
