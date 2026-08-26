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
#include <vine/appfw/gui/RibbonAction.hpp>
#include <vine/appfw/gui/RibbonBar.hpp>
#include <vine/appfw/gui/RibbonButton.hpp>
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
    auto* bar = wnd.ribbonBar();

    auto* tab1 = new RibbonTab();
    tab1->setTitle(u8"Home");
    bar->addTab(tab1);

    auto* group1 = new RibbonGroup();
    group1->setTitle(u8"Clipboard");
    tab1->addGroup(group1);

    auto* btn1 = new RibbonButton();
    btn1->setText(u8"Paste");
    // 演示图标：借用 DockingPanes 的现有资源（仅作示例，无独立图标资源）
    btn1->setIcon(Icon(u8":/img/docking_bitmaps/tab.png"));
    group1->addButton(btn1);

    auto* btn2 = new RibbonButton();
    btn2->setText(u8"Cut");
    group1->addButton(btn2);

    // 下拉按钮：加入下拉项后自动转成 QMenu 挂到按钮上
    auto* btnInsert = new RibbonButton();
    btnInsert->setText(u8"Insert");

    auto* miPic = new RibbonAction();
    miPic->setText(u8"Picture");
    auto* miTable = new RibbonAction();
    miTable->setText(u8"Table");
    auto* miChart = new RibbonAction();
    miChart->setText(u8"Chart");
    miChart->setIcon(Icon(u8":/img/docking_bitmaps/tab.png"));

    btnInsert->addDropDownItem(miPic);
    btnInsert->addDropDownItem(miTable);
    btnInsert->addDropDownItem(miChart);

    group1->addButton(btnInsert);

    auto* group2 = new RibbonGroup();
    group2->setTitle(u8"View");
    tab1->addGroup(group2);

    auto* btn3 = new RibbonButton();
    btn3->setText(u8"Zoom In");
    group2->addButton(btn3);

    // Second tab
    auto* tab2 = new RibbonTab();
    tab2->setTitle(u8"Tools");
    bar->addTab(tab2);

    auto* group3 = new RibbonGroup();
    group3->setTitle(u8"Options");
    tab2->addGroup(group3);

    auto* btn4 = new RibbonButton();
    btn4->setText(u8"Settings");
    group3->addButton(btn4);

    // ---- Application menu ----
    auto* miOpen = new RibbonAction();
    miOpen->setText(u8"Open");
    bar->appendApplicationMenu(miOpen);

    auto* miSave = new RibbonAction();
    miSave->setText(u8"Save");
    bar->appendApplicationMenu(miSave);

    // ---- Dock panels ----
    auto* mgr = wnd.dockPanelManager();

    // ---- Settings panel demo: register config items, ConfigWindow renders them ----
    auto* config    = app.configManager();
    auto* configs   = app.configRegistry();
    auto* editorCat = configs->addCategory(u8"编辑器");
    editorCat->description(u8"编辑器相关设置");
    auto* fontGrp = editorCat->addGroup(u8"字体");
    fontGrp->description(u8"字体相关");
    fontGrp->addItem(vine::appfw::ConfigItem(u8"editor.font.size", u8"字号", vine::appfw::ConfigItemType::Int)
                         .description(u8"编辑器字号（8-72）")
                         .range(8, 72)
                         .defaultValue(14));
    fontGrp->addItem(vine::appfw::ConfigItem(u8"editor.font.family", u8"字体", vine::appfw::ConfigItemType::String)
                         .description(u8"编辑器的等宽字体")
                         .defaultValue(u8"Consolas"));
    fontGrp->addItem(vine::appfw::ConfigItem(u8"editor.font.lineSpacing", u8"行距", vine::appfw::ConfigItemType::Double)
                         .description(u8"编辑器行距（1.0-3.0）")
                         .range(1.0, 3.0)
                         .step(0.1)
                         .defaultValue(1.5));
    fontGrp->addItem(vine::appfw::ConfigItem(u8"editor.font.zoom", u8"缩放", vine::appfw::ConfigItemType::Choice)
                         .description(u8"编辑器整体缩放比例")
                         .choices({ { 0.8, u8"80%" }, { 1.0, u8"100%" }, { 1.2, u8"120%" } })
                         .defaultValue(1.0));
    auto* editorGenGrp = editorCat->addGroup(u8"常规");
    editorGenGrp->addItem(vine::appfw::ConfigItem(u8"editor.wordWrap", u8"自动换行", vine::appfw::ConfigItemType::Bool).defaultValue(true));
    editorGenGrp->addItem(vine::appfw::ConfigItem(u8"editor.showLineNumbers", u8"显示行号", vine::appfw::ConfigItemType::Bool).defaultValue(true));
    editorGenGrp->addItem(vine::appfw::ConfigItem(u8"editor.showWhitespace", u8"显示空白字符", vine::appfw::ConfigItemType::Bool).defaultValue(false));
    editorGenGrp->addItem(vine::appfw::ConfigItem(u8"editor.indentMode", u8"缩进模式", vine::appfw::ConfigItemType::Choice)
                              .choices({ { u8"space", u8"空格" }, { u8"tab", u8"制表符" } })
                              .defaultValue(u8"space"));
    auto* appearanceCat = configs->addCategory(u8"外观");
    appearanceCat->description(u8"界面外观相关设置");
    auto* themeGrp = appearanceCat->addGroup(u8"主题");
    themeGrp->addItem(vine::appfw::ConfigItem(u8"app.theme", u8"主题", vine::appfw::ConfigItemType::Choice)
                          .choices({ u8"浅色", u8"深色", u8"跟随系统" })
                          .defaultValue(u8"浅色"));
    themeGrp->addItem(vine::appfw::ConfigItem(u8"app.accentColor", u8"强调色", vine::appfw::ConfigItemType::Choice)
                          .choices({ { 0, u8"蓝色" }, { 1, u8"绿色" }, { 2, u8"红色" } })
                          .defaultValue(1));
    auto* uiGrp = appearanceCat->addGroup(u8"界面");
    uiGrp->addItem(vine::appfw::ConfigItem(u8"app.animations", u8"启用动画", vine::appfw::ConfigItemType::Bool).defaultValue(true));
    uiGrp->addItem(vine::appfw::ConfigItem(u8"app.fontScale", u8"界面缩放", vine::appfw::ConfigItemType::Int)
                       .description(u8"界面文字缩放（80-200%）")
                       .range(80, 200)
                       .step(10)
                       .defaultValue(100));
    uiGrp->addItem(vine::appfw::ConfigItem(u8"app.opacity", u8"不透明度", vine::appfw::ConfigItemType::Double)
                       .description(u8"主窗口不透明度（0.5-1.0）")
                       .range(0.5, 1.0)
                       .step(0.05)
                       .defaultValue(1.0));

    auto* settingsWin = new ConfigWindow(configs, config);
    settingsWin->setWindowTitle(u8"设置");
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
    panelLeft->setTitle(u8"Project");
    panelLeft->setId(u8"dock_project");
    panelLeft->setFeatures(DockFeatures::None); // 无 Closable
    mgr->addDockPanel(panelLeft, DockAreas::Left);

    // 右侧面板：不允许浮动（拖拽不会脱离停靠）
    auto* panelRight = new DockPanel();
    panelRight->setTitle(u8"Properties");
    panelRight->setId(u8"dock_properties");
    panelRight->setFeatures(DockFeatures::Closable); // 无 Floatable
    mgr->addDockPanel(panelRight, DockAreas::Right);

    // 底部面板：不允许拖动（固定在原位）
    auto* panelBottom = new DockPanel();
    panelBottom->setTitle(u8"Output");
    panelBottom->setId(u8"dock_output");
    panelBottom->setFeatures(DockFeatures::Closable); // 无 Movable
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
    panelBottom2->setFeatures(DockFeatures::None); // 不可关闭、不可拖动、不可浮动
    mgr->addDockPanel(panelBottom2, DockAreas::Bottom);

    // Exercise queries
    std::cout << "Dock panel count: " << mgr->count() << std::endl;
    for (auto* p : mgr->panels()) {
        auto        utf16 = p->title().toUtf16();
        std::string title(utf16.begin(), utf16.end());
        std::cout << "  - " << title << "  closable=" << vine::testFlag(p->features(), DockFeatures::Closable) << std::endl;
    }

    return app.run();
}
