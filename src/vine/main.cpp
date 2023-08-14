#include <iostream>
#include <array>
#include <ctime>
#include <vector>


#include <appfw/gui/GuiApplication.h>
#include <appfw/gui/MainWindow.h>

namespace fw = vine::appfw;
namespace guifw = fw::gui;

class 你好{
    int x;
};

int main(int argc, char** argv){

    guifw::GuiApplication app(argc, argv);
    app.init();
    bool b = app.isKindOf<vine::Object>();

    auto wnd = new guifw::MainWindow();
    wnd->show();

    auto& x = typeid(你好);
    auto y = x.name();
    
    vine::String s1 = U"abcAbc", s33;
    auto b1 = s1.endsWith(U"", true);
     
     auto c1 = s1.rend();
c1--;
    return app.run();

    
}