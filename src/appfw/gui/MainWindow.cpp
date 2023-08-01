#include <QMainWindow>

#include <appfw/gui/MainWindow.h>


ETD_APPFW_GUI_BEGIN

struct MainWindow::Data{
    QMainWindow* mwnd;
};


MainWindow::MainWindow()
: d(new Data)
{
    d->mwnd = new QMainWindow();
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

ETD_APPFW_GUI_NS_END