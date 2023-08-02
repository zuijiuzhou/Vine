#include <iostream>
#include <array>
#include <ctime>
#include <vector>

#include <appfw/gui/GuiApplication.h>
#include <appfw/gui/MainWindow.h>



int main(int argc, char** argv){

    vine::appfw::gui::GuiApplication app(argc, argv);
    vine::appfw::gui::GuiApplication::Ptr ptr(&app);
    vine::RefPtr<vine::appfw::gui::GuiApplication> ref_ptr(&app);
    app.init();
    bool b = app.isKindOf<vine::Object>();

    auto wnd = new vine::appfw::gui::MainWindow();

    wnd->show();


    return app.run();
}