#include <iostream>
#include <array>
#include <ctime>
#include <vector>

#include <appfw/gui/GuiApplication.h>
#include <appfw/gui/MainWindow.h>

namespace fw = vine::appfw;
namespace guifw = fw::gui;


int main(int argc, char** argv){

    guifw::GuiApplication app(argc, argv);
    app.init();
    bool b = app.isKindOf<vine::Object>();

    auto wnd = new guifw::MainWindow();
    wnd->show();



    return app.run();
}