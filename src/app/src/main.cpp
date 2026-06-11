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
#include <vine/appfw/gui/DockPanelManager.hpp>
#include <vine/appfw/gui/RibbonBar.hpp>
#include <vine/appfw/gui/RibbonButton.hpp>
#include <vine/appfw/gui/RibbonDropDownItem.hpp>
#include <vine/appfw/gui/RibbonGroup.hpp>
#include <vine/appfw/gui/RibbonTab.hpp>

namespace fw    = vine::appfw;
namespace guifw = fw::gui;

int main(int argc, char** argv)
{
    using namespace guifw;

    GuiApplication app(argc, argv);
    app.init();

    MainWindow wnd;
    wnd.show();

    // ---- Ribbon tabs & groups ----
    auto* bar  = wnd.ribbonBar();

    auto* tab1 = new RibbonTab();
    tab1->title(u8"Home");
    bar->addTab(tab1);

    auto* group1 = new RibbonGroup();
    group1->title(u8"Clipboard");
    tab1->addGroup(group1);

    auto* btn1 = new RibbonButton();
    btn1->text(u8"Paste");
    group1->addButton(btn1);

    auto* btn2 = new RibbonButton();
    btn2->text(u8"Cut");
    group1->addButton(btn2);

    auto* group2 = new RibbonGroup();
    group2->title(u8"View");
    tab1->addGroup(group2);

    auto* btn3 = new RibbonButton();
    btn3->text(u8"Zoom In");
    group2->addButton(btn3);

    // Second tab
    auto* tab2 = new RibbonTab();
    tab2->title(u8"Tools");
    bar->addTab(tab2);

    auto* group3 = new RibbonGroup();
    group3->title(u8"Options");
    tab2->addGroup(group3);

    auto* btn4 = new RibbonButton();
    btn4->text(u8"Settings");
    group3->addButton(btn4);

    // ---- Application menu ----
    auto* miOpen = new RibbonDropDownItem();
    miOpen->text(u8"Open");
    bar->appendApplicationMenu(miOpen);

    auto* miSave = new RibbonDropDownItem();
    miSave->text(u8"Save");
    bar->appendApplicationMenu(miSave);

    // ---- Dock panels ----
    auto* mgr = wnd.dockPanelManager();

    auto* panelLeft = mgr->createDockPanel(DockAreas::Left);
    panelLeft->setTitle(u8"Project");
    panelLeft->setId(u8"dock_project");

    auto* panelRight = mgr->createDockPanel(DockAreas::Right);
    panelRight->setTitle(u8"Properties");
    panelRight->setId(u8"dock_properties");

    auto* panelBottom = mgr->createDockPanel(DockAreas::Bottom);
    panelBottom->setTitle(u8"Output");
    panelBottom->setId(u8"dock_output");

    // Exercise queries
    std::cout << "Dock panel count: " << mgr->count() << std::endl;
    auto* found = mgr->findById(u8"dock_output");
    if (found) {
        auto utf16 = found->getTitle().toUtf16();
        std::string title(utf16.begin(), utf16.end());
        std::cout << "Found output panel, title=" << title << std::endl;
    }

    // Test floating
    panelRight->setFloating(true);
    std::cout << "Properties floating=" << panelRight->isFloating() << std::endl;

    return app.run();
}
