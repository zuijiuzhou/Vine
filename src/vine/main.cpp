#include <iostream>
#include <array>
#include <ctime>
#include <vector>

#include <di/Registration.h>
#include <di/Container.h>

#include <appfw/AddinManager.h>
#include <appfw/gui/GuiApplication.h>
#include <appfw/gui/MainWindow.h>

#include <appfw/gui/RibbonBar.h>
#include <appfw/gui/RibbonTab.h>
#include <appfw/gui/RibbonGroup.h>
#include <appfw/gui/RibbonDropDownItem.h>

namespace fw = vine::appfw;
namespace guifw = fw::gui;

int main(int argc, char **argv)
{
    std::vector<double> ints ;
    std::vector<double> ints2 ;
    auto b2 = memcmp(ints.data(), ints2.data(), sizeof(int) * ints.size());

    guifw::GuiApplicationPtr app = new guifw::GuiApplication(argc, argv);
    app->init();
    guifw::MainWindowPtr wnd = new guifw::MainWindow();
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

    auto bar = wnd->ribbonBar();
    auto rtab = new guifw::RibbonTab();
    rtab->title(U"Tab1");
    bar->addTab(rtab);
    auto rgroup = new guifw::RibbonGroup();
    rgroup->title(U"Group1");
    rtab->addGroup(rgroup);

    auto mi1 = new guifw::RibbonDropDownItem();
    mi1->text(U"Open")->data((void*)123);
    bar->appendApplicationMenu(mi1);

    return app->run();
}