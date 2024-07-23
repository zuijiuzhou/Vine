#include <array>
#include <ctime>
#include <iostream>
#include <vector>

#include <ge/Point3d.h>
#include <ge/Vector3d.h>

#include <core/Ptr.h>
#include <core/stl++.h>

#include <di/Container.h>
#include <di/Registration.h>

#include <appfw/AddinManager.h>
#include <appfw/gui/GuiApplication.h>
#include <appfw/gui/MainWindow.h>

#include <appfw/gui/RibbonBar.h>
#include <appfw/gui/RibbonDropDownItem.h>
#include <appfw/gui/RibbonGroup.h>
#include <appfw/gui/RibbonTab.h>
#include <appfw/gui/DockPanel.h>

namespace fw    = vine::appfw;
namespace guifw = fw::gui;
namespace di    = vine::di;


int main(int argc, char** argv) {

    guifw::GuiApplicationPtr app = new guifw::GuiApplication(argc, argv);
    app->init();
    guifw::MainWindowPtr wnd = new guifw::MainWindow();
    wnd->show();

    di::Container c;
    auto reg = di::Registration::create<vine::Object>()->impl<fw::AddinManager>()->lifetime(di::Lifetime::Singleton);
    c.add(reg);

    auto bar  = wnd->ribbonBar();
    auto rtab = new guifw::RibbonTab();
    rtab->title(U"Tab1");
    bar->addTab(rtab);
    auto rgroup = new guifw::RibbonGroup();
    rgroup->title(U"Group1");
    rtab->addGroup(rgroup);

    auto mi1 = new guifw::RibbonDropDownItem();
    mi1->text(U"Open");
    mi1->data((void*)123);
    bar->appendApplicationMenu(mi1);

    auto panel_left = new guifw::DockPanel();
    panel_left->setTitle(U"Left");
    wnd->addDockPanel(panel_left, guifw::DockAreas::Left);
    auto panel_top = new guifw::DockPanel();
    panel_top->setTitle(U"Top");
    wnd->addDockPanel(panel_top, guifw::DockAreas::Top);
    auto panel_right = new guifw::DockPanel();
    panel_right->setTitle(U"Right");
    wnd->addDockPanel(panel_right, guifw::DockAreas::Right);
    auto panel_bottom = new guifw::DockPanel();
    panel_bottom->setTitle(U"Bottom");
    wnd->addDockPanel(panel_bottom, guifw::DockAreas::Bottom);

    return app->run();
}