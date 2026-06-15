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

#include <QLabel>

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

    // 中央客户区
    struct CentralWidget : UIElement {
        QLabel* label;
        CentralWidget()
            : UIElement(new QLabel())
            , label(impl<QLabel>())
        {
            label->setText(u8"Welcome to Vine\n中央工作区");
            label->setAlignment(Qt::AlignCenter);
            label->setStyleSheet("font-size: 18px; color: #888;");
        }
    };
    mgr->setCentralWidget(new CentralWidget());

    // 左侧面板：不允许关闭（无关闭按钮）
    auto* panelLeft = new DockPanel();
    panelLeft->setTitle(u8"Project");
    panelLeft->setId(u8"dock_project");
    panelLeft->setFeatures(DockFeatures::None);  // 无 Closable
    mgr->addDockPanel(panelLeft, DockAreas::Left);

    // 右侧面板：不允许浮动（拖拽不会脱离停靠）
    auto* panelRight = new DockPanel();
    panelRight->setTitle(u8"Properties");
    panelRight->setId(u8"dock_properties");
    panelRight->setFeatures(DockFeatures::Closable);  // 无 Floatable
    mgr->addDockPanel(panelRight, DockAreas::Right);

    // 底部面板：不允许拖动（固定在原位）
    auto* panelBottom = new DockPanel();
    panelBottom->setTitle(u8"Output");
    panelBottom->setId(u8"dock_output");
    panelBottom->setFeatures(DockFeatures::Closable);  // 无 Movable
    mgr->addDockPanel(panelBottom, DockAreas::Bottom);

    // 顶部面板：全部特性启用（默认行为）
    auto* panelTop = new DockPanel();
    panelTop->setTitle(u8"Toolbox");
    panelTop->setId(u8"dock_toolbox");
    panelTop->setFeatures(DockFeatures::All);
    mgr->addDockPanel(panelTop, DockAreas::Top);

    // 另一个底部面板：完全锁定
    auto* panelBottom2 = new DockPanel();
    panelBottom2->setTitle(u8"Log");
    panelBottom2->setId(u8"dock_log");
    panelBottom2->setFeatures(DockFeatures::None);  // 不可关闭、不可拖动、不可浮动
    mgr->addDockPanel(panelBottom2, DockAreas::Bottom);

    // Exercise queries
    std::cout << "Dock panel count: " << mgr->count() << std::endl;
    for (auto* p : mgr->panels()) {
        auto  utf16  = p->getTitle().toUtf16();
        std::string title(utf16.begin(), utf16.end());
        std::cout << "  - " << title
                  << "  closable=" << vine::testFlag(p->getFeatures(), DockFeatures::Closable)
                  << std::endl;
    }

    // Test floating
    panelRight->setFloating(true);
    std::cout << "Properties floating=" << panelRight->isFloating() << std::endl;

    return app.run();
}
