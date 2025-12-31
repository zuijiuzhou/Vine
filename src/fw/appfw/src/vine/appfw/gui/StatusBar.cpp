#include <vine/appfw/gui/StatusBar.hpp>

#include <QStatusBar>

#include <vine/appfw/gui/MainWindow.hpp>
#include <vine/Ptr.hpp>

VI_APPFWGUI_NS_BEGIN

VI_OBJECT_META_IMPL(StatusBar, Control)

struct StatusBar::Data {
    RefPtr<MainWindow> wnd;
};

StatusBar::StatusBar(MainWindow* wnd)
  : Control(new QStatusBar())
  , d(new Data()) {
    d->wnd = wnd;
}

StatusBar::~StatusBar() {
    delete d;
}

VI_APPFWGUI_NS_END