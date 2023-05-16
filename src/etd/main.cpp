#include <QApplication>
#include <QMainWindow>
#include <appfw/application.h>

int main(int argc, char** argv){
    QApplication qapp(argc, argv);

    etd::appfw::Application app;
    
    QMainWindow wnd;
    wnd.show();

    int code = qapp.exec();
    return code;
}