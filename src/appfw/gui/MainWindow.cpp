#include <QMainWindow>

#include <appfw/gui/MainWindow.h>


VINE_APPFWGUI_BEGIN

struct MainWindow::Data{
    QMainWindow* mwnd;
};


MainWindow::MainWindow()
: d(new Data)
{
    d->mwnd = new QMainWindow();
    QString s("Abc");
    auto x = * s.end();
    auto y = * s.rend();
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

VINE_APPFWGUI_NS_END