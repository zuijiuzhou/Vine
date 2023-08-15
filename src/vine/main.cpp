#include <iostream>
#include <array>
#include <ctime>
#include <vector>


#include <appfw/AddinManager.h>
#include <appfw/gui/GuiApplication.h>
#include <appfw/gui/MainWindow.h>

namespace fw = vine::appfw;
namespace guifw = fw::gui;

int main(int argc, char** argv){

    guifw::GuiApplicationPtr app = guifw::GuiApplication::create(argc, argv);
    app->init();
    guifw::MainWindowPtr wnd = guifw::MainWindow::create();
    wnd->show();

    return app->run();

    
}