#include <SARibbon.h>
#include <appfw/gui/MainWindow.h>
#include <appfw/gui/RibbonBar.h>
#include <appfw/gui/StatusBar.h>

VI_APPFWGUI_NS_BEGIN

VI_OBJECT_META_IMPL(MainWindow, Control)

struct MainWindow::Data{
    RefPtr<RibbonBar> ribbon_bar;
    RefPtr<StatusBar> status_bar;
};

namespace{
    using itype = SARibbonMainWindow;
}

MainWindow::MainWindow()
: Control(new SARibbonMainWindow(nullptr, true))
, d(new Data)
{
    size(Size(600, 400));
    d->ribbon_bar = new RibbonBar(this);
    d->status_bar = new StatusBar(this);

    SAFramelessHelper* helper = impl<itype>()->framelessHelper();
    helper->setRubberBandOnResize(false);
    impl<itype>()->setWindowTitle(("Vine"));
      
    impl<itype>()->setStatusBar(d->status_bar->impl<QStatusBar>());

    SARibbonBar* ribbon = impl<itype>()->ribbonBar();
    //通过setContentsMargins设置ribbon四周的间距
    ribbon->setContentsMargins(5, 0, 5, 0);
    //设置applicationButton
    ribbon->applicationButton()->setText(("File"));
}

MainWindow::~MainWindow(){
    delete d;
}

void MainWindow::show(){
    impl<itype>()->show();
}

void MainWindow::close(){
    impl<itype>()->close();
}

RibbonBar* MainWindow::ribbonBar() const{
    return d->ribbon_bar.get();
}

StatusBar* MainWindow::statusBar() const{
    return d->status_bar.get();
}


VI_APPFWGUI_NS_END