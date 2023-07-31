#include <QMainWindow>

#include <appfw/gui/MainWindow.h>


ETD_APPFW_GUI_BEGIN

struct MainWindow::Data{
    QMainWindow* mwnd;
};


MainWindow::MainWindow()
: d_ptr(new Data)
{
    d_ptr->mwnd = new QMainWindow();
}

MainWindow::~MainWindow(){
    delete d_ptr;
}

void MainWindow::show(){
    d_ptr->mwnd->show();
}

void MainWindow::close(){
    d_ptr->mwnd->close();
}

ETD_APPFW_GUI_NS_END