#include <iostream>
#include <array>
#include <ctime>
#include <vector>

#include <di/Registration.h>
#include <di/Container.h>

#include <appfw/AddinManager.h>
#include <appfw/gui/GuiApplication.h>
#include <appfw/gui/MainWindow.h>

namespace fw = vine::appfw;
namespace guifw = fw::gui;

int main(int argc, char **argv)
{

    vine::RefPtr<guifw::GuiApplication> app = guifw::GuiApplication::create(argc, argv);
    app->init();
    guifw::MainWindowPtr wnd = guifw::MainWindow::create();
    wnd->show();

    vine::ObjectPtr objptr = app.get();
vine::ObjectPtr objptr2;
objptr2 = app.get();
    vine::String s = U"Abc你好123DBS";
    vine::String s2 = s.toLower2();

    vine::di::Container c;
    auto reg = vine::di::Registration::create<vine::Object>()->impl<fw::AddinManager>()->lifetime(vine::di::Lifetime::Singleton);
    c.add(reg);

    return app->run();
}