#include <iostream>
#include <array>
#include <ctime>
#include <vector>

#include <appfw/gui/GuiApplication.h>
#include <appfw/gui/MainWindow.h>



int main(int argc, char** argv){

    etd::appfw::gui::GuiApplication app(argc, argv);
    etd::appfw::gui::GuiApplication::Ptr ptr(&app);
    etd::RefPtr<etd::appfw::gui::GuiApplication> ref_ptr(&app);
    app.init();
    bool b = app.isKindOf<etd::Object>();

    auto wnd = new etd::appfw::gui::MainWindow();

    wnd->show();


    return app.run();
}