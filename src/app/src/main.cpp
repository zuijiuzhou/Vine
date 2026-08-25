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
#include <vine/appfw/ConfigItem.hpp>
#include <vine/appfw/ConfigRegistry.hpp>
#include <vine/appfw/gui/ConfigWindow.hpp>
#include <vine/appfw/gui/GuiApplication.hpp>
#include <vine/appfw/gui/MainWindow.hpp>

#include <vine/appfw/gui/DockPanel.hpp>
#include <vine/appfw/gui/DockPanelManager.hpp>
#include <vine/appfw/gui/RibbonBar.hpp>
#include <vine/appfw/gui/RibbonButton.hpp>
#include <vine/appfw/gui/RibbonAction.hpp>
#include <vine/appfw/gui/RibbonGroup.hpp>
#include <vine/appfw/gui/RibbonTab.hpp>

#include <QLabel>

namespace fw    = vine::appfw;
namespace guifw = fw::gui;

int main(int argc, char** argv)
{
    using namespace guifw;
GuiApplication::desc();
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
    // 演示图标：借用 DockingPanes 的现有资源（仅作示例，无独立图标资源）
    btn1->icon(Icon(u8":/img/docking_bitmaps/tab.png"));
    group1->addButton(btn1);

    auto* btn2 = new RibbonButton();
    btn2->text(u8"Cut");
    group1->addButton(btn2);

    // 下拉按钮：加入下拉项后自动转成 QMenu 挂到按钮上
    auto* btnInsert = new RibbonButton();
    btnInsert->text(u8"Insert");

    auto* miPic   = new RibbonAction();
    miPic->text(u8"Picture");
    auto* miTable = new RibbonAction();
    miTable->text(u8"Table");
    auto* miChart = new RibbonAction();
    miChart->text(u8"Chart");
    miChart->icon(Icon(u8":/img/docking_bitmaps/tab.png"));

    btnInsert->addDropDownItem(miPic);
    btnInsert->addDropDownItem(miTable);
    btnInsert->addDropDownItem(miChart);

    group1->addButton(btnInsert);

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
    auto* miOpen = new RibbonAction();
    miOpen->text(u8"Open");
    bar->appendApplicationMenu(miOpen);

    auto* miSave = new RibbonAction();
    miSave->text(u8"Save");
    bar->appendApplicationMenu(miSave);

    // ---- Dock panels ----
    auto* mgr = wnd.dockPanelManager();

    // ---- 设置面板演示：注册配置项，ConfigPanel 渲染 ----------------
    auto* config  = app.configManager();
    auto* configs = app.configRegistry();
    configs->addItem(vine::appfw::ConfigItem(u8"editor.font.size", u8"字号", vine::appfw::ConfigItemType::Int)
                         .group(u8"编辑器")
                         .description(u8"编辑器字号（8-72）")
                         .range(8, 72)
                         .defaultValue(14));
    configs->addItem(vine::appfw::ConfigItem(u8"editor.wordWrap", u8"自动换行", vine::appfw::ConfigItemType::Bool)
                         .group(u8"编辑器")
                         .defaultValue(true));
    configs->addItem(vine::appfw::ConfigItem(u8"app.theme", u8"主题", vine::appfw::ConfigItemType::Choice)
                         .group(u8"外观")
                         .choices({ u8"浅色", u8"深色", u8"跟随系统" })
                         .defaultValue(u8"浅色"));

    auto* settingsWin = new ConfigWindow(configs, config);
    settingsWin->windowTitle(u8"设置");
    settingsWin->resize(480, 360);
    settingsWin->show();

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
    panelLeft->title(u8"Project");
    panelLeft->id(u8"dock_project");
    panelLeft->features(DockFeatures::None);  // 无 Closable
    mgr->addDockPanel(panelLeft, DockAreas::Left);

    // 右侧面板：不允许浮动（拖拽不会脱离停靠）
    auto* panelRight = new DockPanel();
    panelRight->title(u8"Properties");
    panelRight->id(u8"dock_properties");
    panelRight->features(DockFeatures::Closable);  // 无 Floatable
    mgr->addDockPanel(panelRight, DockAreas::Right);

    // 底部面板：不允许拖动（固定在原位）
    auto* panelBottom = new DockPanel();
    panelBottom->title(u8"Output");
    panelBottom->id(u8"dock_output");
    panelBottom->features(DockFeatures::Closable);  // 无 Movable
    mgr->addDockPanel(panelBottom, DockAreas::Bottom);

    // 顶部面板：全部特性启用（默认行为）
    auto* panelTop = new DockPanel();
    panelTop->title(u8"Toolbox");
    panelTop->id(u8"dock_toolbox");
    panelTop->features(DockFeatures::All);
    mgr->addDockPanel(panelTop, DockAreas::Top);

    // 另一个底部面板：完全锁定
    auto* panelBottom2 = new DockPanel();
    panelBottom2->title(u8"Log");
    panelBottom2->id(u8"dock_log");
    panelBottom2->features(DockFeatures::None);  // 不可关闭、不可拖动、不可浮动
    mgr->addDockPanel(panelBottom2, DockAreas::Bottom);

    // Exercise queries
    std::cout << "Dock panel count: " << mgr->count() << std::endl;
    for (auto* p : mgr->panels()) {
        auto  utf16  = p->title().toUtf16();
        std::string title(utf16.begin(), utf16.end());
        std::cout << "  - " << title
                  << "  closable=" << vine::testFlag(p->features(), DockFeatures::Closable)
                  << std::endl;
    }

    return app.run();
}
