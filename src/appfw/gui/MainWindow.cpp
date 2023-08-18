#include <QMainWindow>

#include <SARibbon.h>

#include <appfw/gui/MainWindow.h>
#include <appfw/gui/RibbonBar.h>
#include <appfw/gui/StatusBar.h>



VINE_APPFWGUI_NS_BEGIN

struct MainWindow::Data{
    SARibbonMainWindow* mwnd;
    RibbonBar* ribbon_bar;
    StatusBar* status_bar;
};


MainWindow::MainWindow()
: d(new Data)
{
    d->mwnd = new SARibbonMainWindow(nullptr, true);
    d->ribbon_bar = new RibbonBar();
    d->status_bar = new StatusBar();
}

MainWindow::~MainWindow(){
    delete d;
}

void MainWindow::show(){
    d->mwnd->show();
}

void MainWindow::close(){
    d->mwnd->close();
}

RibbonBar* MainWindow::ribbonBar() const{
    return d->ribbon_bar;
}

StatusBar* MainWindow::statusBar() const{
    return d->status_bar;
}

VINE_APPFWGUI_NS_END