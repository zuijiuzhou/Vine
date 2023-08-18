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

    guifw::GuiApplicationPtr app = new guifw::GuiApplication(argc, argv);
    app->init();
    guifw::MainWindowPtr wnd = guifw::MainWindow::create();
    wnd->show();

    auto b = app->isKindOf<vine::Object>();
    auto bb = app->isKindOf<vine::di::Container>();

    vine::ObjectPtr objptr = app;
    auto n = objptr->numRefs();
    vine::String s = U"Abc你好123DBS";
    vine::String s2 = s.toLower2();

    vine::di::Container c;
    auto reg = vine::di::Registration::create<vine::Object>()->impl<fw::AddinManager>()->lifetime(vine::di::Lifetime::Singleton);
    c.add(reg);

    return app->run();
}