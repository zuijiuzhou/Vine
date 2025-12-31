#include <array>
#include <ctime>
#include <iostream>
#include <vector>

#include <vine/math/Point3.hpp>
#include <vine/math/Vector3.hpp>

#include <vine/Ptr.hpp>
#include <vine/stl++.hpp>

#include <vine/di/Container.hpp>
#include <vine/di/Registration.hpp>

#include <vine/appfw/AddinManager.hpp>
#include <vine/appfw/gui/GuiApplication.hpp>
#include <vine/appfw/gui/MainWindow.hpp>

#include <vine/appfw/gui/DockPanel.hpp>
#include <vine/appfw/gui/RibbonBar.hpp>
#include <vine/appfw/gui/RibbonDropDownItem.hpp>
#include <vine/appfw/gui/RibbonGroup.hpp>
#include <vine/appfw/gui/RibbonTab.hpp>

namespace fw    = vine::appfw;
namespace guifw = fw::gui;
namespace di    = vine::di;


int main(int argc, char** argv) {

    printf("-------");
    guifw::GuiApplicationPtr app = new guifw::GuiApplication(argc, argv);
    app->init();
    guifw::MainWindowPtr wnd = new guifw::MainWindow();
    wnd->show();

    di::Container c;
    auto          reg =
        di::Registration::create<vine::RefObject>()->impl(fw::AddinManager::desc())->lifetime(di::Lifetime::Singleton);
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