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
    
    vine::String s = U"你好Abc456";
    auto s1 = s.substr(2);
    auto s2 = s.substr(2, 3);
    auto s3 = s.substr(8, 1);

    auto z =strlen(y);
    return app.run();

    
}