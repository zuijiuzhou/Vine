// test_gui.cpp
//
// GUI 层单元测试：功能区（RibbonBar/Tab/Group/Button/DropDownItem）的构造、
// 属性与事件，以及停靠面板（DockPanelManager/DockPanel）的管理与状态切换。
//
// 运行前提：
//   - 需要真实显示环境（本地运行，会短暂弹出窗口）。
//   - 整个测试进程共享一个 GuiApplication（在全局 Environment 中创建，
//     因为 Application 单例只允许一个实例）。
//   - RibbonTab/Group/Button 与 DockPanel 包装对象刻意不释放（与 demo 一致）；
//     其 Qt impl 由 SARibbon/DockingPanes 持有，进程退出时统一回收。

#include <gtest/gtest.h>

#include <QByteArray>
#include <QCheckBox>
#include <QColor>
#include <QComboBox>
#include <QCoreApplication>
#include <QDoubleSpinBox>
#include <QIcon>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QSpinBox>
#include <QTabWidget>
#include <QToolBar>
#include <QToolButton>
#include <QWidget>

#include <SARibbon.h>

#include <vine/vi_global.hpp>

#include <vine/appfw/Command.hpp>
#include <vine/appfw/CommandManager.hpp>
#include <vine/appfw/ConfigItem.hpp>
#include <vine/appfw/ConfigManager.hpp>
#include <vine/appfw/ConfigRegistry.hpp>
#include <vine/appfw/ConfigStandard.hpp>
#include <vine/appfw/PluginLoadContext.hpp>

#include <vine/appfw/gui/Control.hpp>
#include <vine/appfw/gui/DockPanel.hpp>
#include <vine/appfw/gui/DockPanelManager.hpp>
#include <vine/appfw/gui/GuiApplication.hpp>
#include <vine/appfw/gui/MainWindow.hpp>
#include <vine/appfw/gui/RibbonAction.hpp>
#include <vine/appfw/gui/RibbonBar.hpp>
#include <vine/appfw/gui/RibbonButton.hpp>
#include <vine/appfw/gui/RibbonGroup.hpp>
#include <vine/appfw/gui/RibbonTab.hpp>
#include <vine/appfw/gui/ProgressPresenter.hpp>

#include <vine/progress/ProgressHost.hpp>
#include <vine/progress/ProgressRange.hpp>
#include <vine/progress/ProgressScope.hpp>

#include <vine/async/Sleep.hpp>
#include <vine/async/SyncWait.hpp>

#include <any>
#include <chrono>
#include <memory>
#include <stdexcept>
#include <thread>

#include <QPushButton>

namespace guifw = vine::appfw::gui;

namespace
{

// 整个测试进程共享一个 GuiApplication（必须在任何 QWidget 之前创建）。
class GuiEnv : public ::testing::Environment {
  public:
    void SetUp() override
    {
        guifw::GuiApplication::desc();
        static char  arg0[] = "test_gui";
        static char* argv[] = { arg0, nullptr };
        int          argc   = 1;
        app                 = std::make_unique<guifw::GuiApplication>(argc, argv);
        app->init(); // 创建 QApplication
    }

    void TearDown() override
    {
        app.reset();
    }

    static std::unique_ptr<guifw::GuiApplication> app;
};

std::unique_ptr<guifw::GuiApplication> GuiEnv::app;

// gtest_main 没有自定义 main 的钩子，用静态初始化注册全局环境即可。
::testing::Environment* const g_gui_env = ::testing::AddGlobalTestEnvironment(new GuiEnv());

// 最小具体命令，用于验证 CommandManager 的 owner 跟踪与按插件报告。
class DummyCommand : public vine::appfw::Command {
    V_OBJECT_META_DECL;

  public:
    vine::String name() const override { return u8"dummy"; }
    vine::String group() const override { return u8"Test"; }
    vine::String description() const override { return u8"dummy command"; }
    vine::appfw::CommandFlags flags() const override { return vine::appfw::CommandFlags::None; }
    vine::async::Task<vine::appfw::CommandResult> execute(vine::appfw::CommandExecutionContext*) override
    {
        co_return vine::appfw::CommandResult(vine::appfw::CommandStatus::Success);
    }
};

V_OBJECT_META_IMPL(DummyCommand, vine::appfw::Command)

// 耗时命令：LongRunning 标志让 CommandManager 自动挂载环境进度宿主。
class LongCommand : public vine::appfw::Command {
    V_OBJECT_META_DECL;

  public:
    vine::String name() const override { return u8"long"; }
    vine::String group() const override { return u8"Test"; }
    vine::String description() const override { return u8"long running command"; }
    vine::appfw::CommandFlags flags() const override { return vine::appfw::CommandFlags::LongRunning; }
    vine::async::Task<vine::appfw::CommandResult> execute(vine::appfw::CommandExecutionContext*) override
    {
        co_await vine::async::sleepFor(std::chrono::milliseconds(120));
        co_return vine::appfw::CommandResult(vine::appfw::CommandStatus::Success);
    }
};

V_OBJECT_META_IMPL(LongCommand, vine::appfw::Command)

// 嵌套子命令：LongRunning，复用父命令的取消/进度宿主。
class NestedChildCommand : public vine::appfw::Command {
    V_OBJECT_META_DECL;

  public:
    vine::String name() const override { return u8"nested_child"; }
    vine::String group() const override { return u8"Test"; }
    vine::String description() const override { return u8"nested child command"; }
    vine::appfw::CommandFlags flags() const override { return vine::appfw::CommandFlags::LongRunning; }
    vine::async::Task<vine::appfw::CommandResult> execute(vine::appfw::CommandExecutionContext* context) override
    {
        // 子命令现在是独立宿主（前台栈顶），可上报自己的进度。
        if (auto* host = vine::progress::ProgressHost::current()) {
            host->setLabel("child");
            vine::progress::ProgressScope scope = host->scope("child", 20);
            for (int i = 0; i < 20; ++i) {
                if (context && context->isCancelled()) {
                    co_return vine::appfw::CommandResult(vine::appfw::CommandStatus::Cancelled);
                }
                scope.next(1);
                co_await vine::async::sleepFor(std::chrono::milliseconds(10));
            }
        }
        else {
            for (int i = 0; i < 20; ++i) {
                if (context && context->isCancelled()) {
                    co_return vine::appfw::CommandResult(vine::appfw::CommandStatus::Cancelled);
                }
                co_await vine::async::sleepFor(std::chrono::milliseconds(10));
            }
        }
        co_return vine::appfw::CommandResult(vine::appfw::CommandStatus::Success);
    }
};

V_OBJECT_META_IMPL(NestedChildCommand, vine::appfw::Command)

// 嵌套父命令：LongRunning，报告进度并通过 context->executeChild 调用子命令。
class NestedProgressCommand : public vine::appfw::Command {
    V_OBJECT_META_DECL;

  public:
    vine::String name() const override { return u8"nested_progress"; }
    vine::String group() const override { return u8"Test"; }
    vine::String description() const override { return u8"nested progress command"; }
    vine::appfw::CommandFlags flags() const override { return vine::appfw::CommandFlags::LongRunning; }
    vine::async::Task<vine::appfw::CommandResult> execute(vine::appfw::CommandExecutionContext* context) override
    {
        auto* host = vine::progress::ProgressHost::current();
        if (!host || !context) {
            co_return vine::appfw::CommandResult(vine::appfw::CommandStatus::Failed);
        }
        host->setLabel("parent");
        vine::progress::ProgressScope root = host->scope("parent", 20);
        for (int i = 0; i < 8; ++i) {
            if (root.isCancelled()) {
                co_return vine::appfw::CommandResult(vine::appfw::CommandStatus::Cancelled);
            }
            root.next(1);
            co_await vine::async::sleepFor(std::chrono::milliseconds(5));
        }
        const auto child_result = co_await context->executeChild(u8"nested_child");
        if (!child_result.succeeded()) {
            co_return child_result;
        }
        for (int i = 0; i < 12; ++i) {
            if (root.isCancelled()) {
                co_return vine::appfw::CommandResult(vine::appfw::CommandStatus::Cancelled);
            }
            root.next(1);
            co_await vine::async::sleepFor(std::chrono::milliseconds(5));
        }
        co_return vine::appfw::CommandResult(vine::appfw::CommandStatus::Success);
    }
};

V_OBJECT_META_IMPL(NestedProgressCommand, vine::appfw::Command)

} // namespace

// ---------------------------------------------------------------------------
// 每用例独立构建一个 MainWindow + 功能区 + 停靠面板，互不干扰。
// ---------------------------------------------------------------------------
class GuiTest : public ::testing::Test {
  protected:
    void SetUp() override
    {
        wnd = std::make_unique<guifw::MainWindow>();
        wnd->show();
        QCoreApplication::processEvents();
        buildRibbon();
        buildDock();
    }

    void TearDown() override
    {
        wnd.reset();
        QCoreApplication::processEvents();
    }

    // ---------- 功能区成员 ----------
    guifw::RibbonTab*    tabHome   = nullptr;
    guifw::RibbonGroup*  groupClip = nullptr;
    guifw::RibbonGroup*  groupView = nullptr;
    guifw::RibbonButton* btnPaste  = nullptr;
    guifw::RibbonButton* btnCut    = nullptr;
    guifw::RibbonButton* btnInsert = nullptr;
    guifw::RibbonButton* btnStyle  = nullptr;
    guifw::RibbonButton* btnBig    = nullptr;
    guifw::RibbonButton* btnMid    = nullptr;
    guifw::RibbonAction* miPic     = nullptr;
    guifw::RibbonAction* miTable   = nullptr;
    guifw::RibbonAction* miChart   = nullptr;

    void buildRibbon()
    {
        auto* bar = wnd->ribbonBar();

        // tab 1: Home
        tabHome = new guifw::RibbonTab();
        tabHome->setTitle(u8"Home");
        bar->addTab(tabHome);

        // 组 1: Clipboard —— 普通按钮 + 下拉按钮
        groupClip = new guifw::RibbonGroup();
        groupClip->setTitle(u8"Clipboard");
        tabHome->addGroup(groupClip);

        btnPaste = new guifw::RibbonButton();
        btnPaste->setText(u8"Paste");
        btnPaste->setIcon(guifw::Icon(u8":/img/docking_bitmaps/tab.png"));
        groupClip->addButton(btnPaste);

        btnCut = new guifw::RibbonButton();
        btnCut->setText(u8"Cut");
        groupClip->addButton(btnCut);

        btnInsert = new guifw::RibbonButton();
        btnInsert->setText(u8"Insert");

        miPic = new guifw::RibbonAction();
        miPic->setText(u8"Picture");
        miTable = new guifw::RibbonAction();
        miTable->setText(u8"Table");
        miChart = new guifw::RibbonAction();
        miChart->setText(u8"Chart");
        miChart->setIcon(guifw::Icon(u8":/img/docking_bitmaps/tab.png"));

        btnInsert->addDropDownItem(miPic);
        btnInsert->addDropDownItem(miTable);
        btnInsert->addDropDownItem(miChart);
        groupClip->addButton(btnInsert);

        // 组 2: View —— 样式/尺寸/事件测试按钮
        groupView = new guifw::RibbonGroup();
        groupView->setTitle(u8"View");
        tabHome->addGroup(groupView);

        btnStyle = new guifw::RibbonButton();
        btnStyle->setText(u8"Style");
        btnStyle->setIcon(guifw::Icon(u8":/img/docking_bitmaps/tab.png"));
        btnStyle->setStyle(guifw::RibbonButtonStyle::TextBesideIcon);
        btnStyle->setIconRightText(true);
        btnStyle->setTooltip(u8"style tooltip");
        btnStyle->setCheckable(true);
        btnStyle->setChecked(true);
        groupView->addButton(btnStyle);

        QPixmap bigPm(64, 64);
        bigPm.fill(QColor(42, 130, 218));
        btnBig = new guifw::RibbonButton();
        btnBig->setText(u8"Big");
        btnBig->setIcon(guifw::Icon(QIcon(bigPm)));
        btnBig->setButtonSize(guifw::RibbonItemSize::Large);
        btnBig->setLargeIconSize(guifw::Size(64, 64));
        groupView->addButton(btnBig);

        btnMid = new guifw::RibbonButton();
        btnMid->setText(u8"Mid");
        btnMid->setIcon(guifw::Icon(u8":/img/docking_bitmaps/tab.png"));
        btnMid->setButtonSize(guifw::RibbonItemSize::Medium);
        groupView->addButton(btnMid);

        // tab 2: Tools
        auto* tab2 = new guifw::RibbonTab();
        tab2->setTitle(u8"Tools");
        bar->addTab(tab2);

        auto* groupOpt = new guifw::RibbonGroup();
        groupOpt->setTitle(u8"Options");
        tab2->addGroup(groupOpt);

        auto* btn4 = new guifw::RibbonButton();
        btn4->setText(u8"Settings");
        groupOpt->addButton(btn4);
    }

    // ---------- 停靠面板成员 ----------
    guifw::DockPanel* panelLeft    = nullptr;
    guifw::DockPanel* panelRight   = nullptr;
    guifw::DockPanel* panelBottom  = nullptr;
    guifw::DockPanel* panelTop     = nullptr;
    guifw::DockPanel* panelBottom2 = nullptr;

    void buildDock()
    {
        auto* mgr = wnd->dockPanelManager();

        panelLeft = new guifw::DockPanel();
        panelLeft->setTitle(u8"Project");
        panelLeft->setId(u8"dock_project");
        panelLeft->setFeatures(guifw::DockFeatures::None);
        mgr->addDockPanel(panelLeft, guifw::DockAreas::Left);

        panelRight = new guifw::DockPanel();
        panelRight->setTitle(u8"Properties");
        panelRight->setId(u8"dock_properties");
        panelRight->setFeatures(guifw::DockFeatures::Closable);
        mgr->addDockPanel(panelRight, guifw::DockAreas::Right);

        panelBottom = new guifw::DockPanel();
        panelBottom->setTitle(u8"Output");
        panelBottom->setId(u8"dock_output");
        panelBottom->setFeatures(guifw::DockFeatures::Closable);
        mgr->addDockPanel(panelBottom, guifw::DockAreas::Bottom);

        panelTop = new guifw::DockPanel();
        panelTop->setTitle(u8"Toolbox");
        panelTop->setId(u8"dock_toolbox");
        panelTop->setFeatures(guifw::DockFeatures::All);
        mgr->addDockPanel(panelTop, guifw::DockAreas::Top);

        panelBottom2 = new guifw::DockPanel();
        panelBottom2->setTitle(u8"Log");
        panelBottom2->setId(u8"dock_log");
        panelBottom2->setFeatures(guifw::DockFeatures::None);
        mgr->addDockPanel(panelBottom2, guifw::DockAreas::Bottom);

        QCoreApplication::processEvents();
    }

    std::unique_ptr<guifw::MainWindow> wnd;
};

// ============================ 功能区 ============================

TEST_F(GuiTest, RibbonBar_Tabs)
{
    auto* bar = wnd->ribbonBar();
    ASSERT_EQ(bar->numTabs(), 2);
    EXPECT_TRUE(bar->tabAt(0)->title() == u8"Home");
    EXPECT_TRUE(bar->tabAt(1)->title() == u8"Tools");
    EXPECT_EQ(bar->currentIndex(), 0);
}

TEST_F(GuiTest, RibbonBar_GlobalSettings)
{
    auto* bar = wnd->ribbonBar();

    // 全局风格往返
    bar->setRibbonStyle(guifw::RibbonStyle::TwoRowCompact);
    EXPECT_EQ(bar->ribbonStyle(), guifw::RibbonStyle::TwoRowCompact);
    bar->setRibbonStyle(guifw::RibbonStyle::ThreeRowLoose);
    EXPECT_EQ(bar->ribbonStyle(), guifw::RibbonStyle::ThreeRowLoose);

    // 折叠模式
    bar->setMinimumMode(true);
    EXPECT_TRUE(bar->minimumMode());
    bar->setMinimumMode(false);
    EXPECT_FALSE(bar->minimumMode());

    // 面板标题
    bar->setPanelTitleVisible(false);
    EXPECT_FALSE(bar->panelTitleVisible());
    bar->setPanelTitleVisible(true);
    EXPECT_TRUE(bar->panelTitleVisible());

    // 全局批量换行 / 图标右侧
    bar->setWordWrap(true);
    EXPECT_TRUE(bar->wordWrap());
    bar->setIconRightText(true);
    EXPECT_TRUE(bar->iconRightText());
}

TEST_F(GuiTest, RibbonBar_ApplicationButton)
{
    auto* bar = wnd->ribbonBar();

    // 可见性往返
    bar->setApplicationButtonVisible(false);
    EXPECT_FALSE(bar->applicationButtonVisible());
    bar->setApplicationButtonVisible(true);
    EXPECT_TRUE(bar->applicationButtonVisible());

    // 图标 / 文字
    bar->setApplicationIcon(guifw::Icon(u8":/img/docking_bitmaps/tab.png"));
    EXPECT_FALSE(bar->applicationIcon().isNull());
    bar->setApplicationText(u8"File");
    EXPECT_TRUE(bar->applicationText() == u8"File");
}

TEST_F(GuiTest, RibbonBar_QuickAccess)
{
    auto* bar = wnd->ribbonBar();

    auto* act = new guifw::RibbonAction();
    act->setText(u8"Save");
    bar->addQuickAccessItem(act);
    bar->addQuickAccessSeparator();

    // 快捷栏里应能找到 Save 动作与一条分隔线（经 Qt API 定位 QToolBar 子控件）
    auto* root = bar->impl<QWidget>();
    ASSERT_NE(root, nullptr);
    bool foundSave = false, foundSep = false;
    for (QToolBar* tb : root->findChildren<QToolBar*>()) {
        for (QAction* a : tb->actions()) {
            if (a->text() == QStringLiteral("Save")) {
                foundSave = true;
            }
            if (a->isSeparator()) {
                foundSep = true;
            }
        }
    }
    EXPECT_TRUE(foundSave);
    EXPECT_TRUE(foundSep);

    // 可见性往返
    bar->setQuickAccessVisible(false);
    EXPECT_FALSE(bar->quickAccessVisible());
    bar->setQuickAccessVisible(true);
    EXPECT_TRUE(bar->quickAccessVisible());
}

TEST_F(GuiTest, RibbonGroups_InCategory)
{
    // 注意：这里不直接链接 tp::SARibbon，因此不能用 SARibbonCategory::panelCount()
    // /panelByName()（内部 qobject_cast 在跨 DLL 边界会因静态库双份元对象而失败）。
    // 改用纯 Qt API（QLayout/QWidget/QLabel）断言分组的存在与标题。
    auto* cat = tabHome->impl<SARibbonCategory>();
    ASSERT_NE(cat, nullptr);
    auto* lay = cat->layout();
    ASSERT_NE(lay, nullptr);

    // Home 页有两个组
    ASSERT_EQ(lay->count(), 2);

    QStringList titles;
    for (int i = 0; i < lay->count(); ++i) {
        if (auto* w = lay->itemAt(i)->widget()) {
            if (auto* label = w->findChild<QLabel*>()) {
                titles << label->text();
            }
        }
    }
    EXPECT_TRUE(titles.contains(QStringLiteral("Clipboard")));
    EXPECT_TRUE(titles.contains(QStringLiteral("View")));
    EXPECT_EQ(titles.size(), 2);
}

TEST_F(GuiTest, RibbonTab_Groups)
{
    EXPECT_EQ(tabHome->numGroups(), 2);
    ASSERT_NE(tabHome->groupAt(0), nullptr);
    EXPECT_TRUE(tabHome->groupAt(0)->title() == u8"Clipboard");
    EXPECT_TRUE(tabHome->groupAt(1)->title() == u8"View");
    EXPECT_EQ(tabHome->groupAt(99), nullptr);
}

TEST_F(GuiTest, RibbonTab_PanelSettings)
{
    // 该页所有面板的布局模式往返
    tabHome->setPanelLayoutMode(guifw::RibbonPanelLayoutMode::TwoRow);
    EXPECT_EQ(tabHome->panelLayoutMode(), guifw::RibbonPanelLayoutMode::TwoRow);
    tabHome->setPanelLayoutMode(guifw::RibbonPanelLayoutMode::ThreeRow);
    EXPECT_EQ(tabHome->panelLayoutMode(), guifw::RibbonPanelLayoutMode::ThreeRow);

    // 面板标题 / 间距
    tabHome->setPanelTitleVisible(false);
    EXPECT_FALSE(tabHome->panelTitleVisible());
    tabHome->setPanelTitleVisible(true);
    EXPECT_TRUE(tabHome->panelTitleVisible());

    tabHome->setPanelSpacing(4);
    EXPECT_EQ(tabHome->panelSpacing(), 4);
}

TEST_F(GuiTest, RibbonButton_Properties)
{
    EXPECT_TRUE(btnPaste->text() == u8"Paste");
    EXPECT_FALSE(btnPaste->icon().isNull());
    EXPECT_EQ(btnPaste->buttonSize(), guifw::RibbonItemSize::Small);
    EXPECT_TRUE(btnPaste->enabled());
    EXPECT_FALSE(btnPaste->checkable());
    EXPECT_FALSE(btnPaste->checked());
    EXPECT_TRUE(btnPaste->tooltip().empty());
}

TEST_F(GuiTest, RibbonButton_StyleAndState)
{
    EXPECT_EQ(btnStyle->buttonSize(), guifw::RibbonItemSize::Small);
    EXPECT_EQ(btnStyle->style(), guifw::RibbonButtonStyle::TextBesideIcon);
    EXPECT_TRUE(btnStyle->iconRightText());
    EXPECT_TRUE(btnStyle->tooltip() == u8"style tooltip");
    EXPECT_TRUE(btnStyle->checkable());
    EXPECT_TRUE(btnStyle->checked());
    EXPECT_TRUE(btnStyle->enabled());
}

TEST_F(GuiTest, RibbonButton_ButtonSizes)
{
    EXPECT_EQ(btnBig->buttonSize(), guifw::RibbonItemSize::Large);
    EXPECT_EQ(btnMid->buttonSize(), guifw::RibbonItemSize::Medium);
    EXPECT_EQ(btnPaste->buttonSize(), guifw::RibbonItemSize::Small);
    EXPECT_FALSE(btnBig->icon().isNull());
    // largeIconSize 可往返
    auto ls = btnBig->largeIconSize();
    EXPECT_EQ(ls.x, 64);
    EXPECT_EQ(ls.y, 64);
}

TEST_F(GuiTest, RibbonButton_DropDown)
{
    EXPECT_EQ(btnInsert->dropDownItemCount(), 3u);
    EXPECT_TRUE(btnInsert->dropDownItemAt(0)->text() == u8"Picture");
    EXPECT_TRUE(btnInsert->dropDownItemAt(1)->text() == u8"Table");
    EXPECT_TRUE(btnInsert->dropDownItemAt(2)->text() == u8"Chart");
    EXPECT_FALSE(btnInsert->dropDownItemAt(2)->icon().isNull());
    EXPECT_EQ(btnInsert->dropDownItemAt(99), nullptr);

    // 加入下拉项后按钮应挂上 QMenu，且包含全部项对应的 QAction
    auto* tb = btnInsert->impl<SARibbonToolButton>();
    ASSERT_NE(tb, nullptr);
    auto* menu = tb->menu();
    ASSERT_NE(menu, nullptr);
    EXPECT_EQ(menu->actions().size(), 3);
}

TEST_F(GuiTest, RibbonButton_DropDownSeparator)
{
    // 分隔线不计入下拉项
    size_t before = btnInsert->dropDownItemCount();
    btnInsert->addSeparator();
    EXPECT_EQ(btnInsert->dropDownItemCount(), before);

    auto* tb = btnInsert->impl<SARibbonToolButton>();
    ASSERT_NE(tb, nullptr);
    auto* menu = tb->menu();
    ASSERT_NE(menu, nullptr);
    EXPECT_EQ(menu->actions().size(), static_cast<int>(before) + 1);
    EXPECT_TRUE(menu->actions().last()->isSeparator());

    // 移除一项后重建，顺序与分隔线仍正确
    btnInsert->removeDropDownItem(miTable);
    EXPECT_EQ(btnInsert->dropDownItemCount(), before - 1);
    EXPECT_EQ(menu->actions().size(), static_cast<int>(before)); // 2 项 + 1 分隔线

    // 清理后可重新加入，分隔线被释放、菜单只剩真实项
    btnInsert->clearDropDownItems();
    EXPECT_EQ(btnInsert->dropDownItemCount(), 0u);
    btnInsert->addDropDownItem(miPic);
    btnInsert->addDropDownItem(miTable);
    btnInsert->addDropDownItem(miChart);
    EXPECT_EQ(btnInsert->dropDownItemCount(), 3u);
    EXPECT_EQ(menu->actions().size(), 3);
    EXPECT_FALSE(menu->actions().last()->isSeparator());

    // ---- 全条目索引接口（项与分隔线统一计数）----
    btnInsert->clearDropDownItems();
    btnInsert->addDropDownItem(miPic);   // entry 0
    btnInsert->addSeparator();           // entry 1
    btnInsert->addDropDownItem(miTable); // entry 2
    btnInsert->addDropDownItem(miChart); // entry 3
    EXPECT_EQ(btnInsert->dropDownEntryCount(), 4u);
    EXPECT_EQ(btnInsert->dropDownItemCount(), 3u);

    // 按全条目索引移除分隔线（entry 1）
    btnInsert->removeDropDownEntryAt(1);
    EXPECT_EQ(btnInsert->dropDownEntryCount(), 3u);
    EXPECT_EQ(btnInsert->dropDownItemCount(), 3u);
    EXPECT_EQ(menu->actions().size(), 3);
    EXPECT_FALSE(menu->actions().at(1)->isSeparator());

    // 移除真实项（entry 0 = miPic）
    btnInsert->removeDropDownEntryAt(0);
    EXPECT_EQ(btnInsert->dropDownEntryCount(), 2u);
    EXPECT_EQ(btnInsert->dropDownItemCount(), 2u);

    // 越界安全
    btnInsert->removeDropDownEntryAt(99);
    EXPECT_EQ(btnInsert->dropDownEntryCount(), 2u);
}

TEST_F(GuiTest, RibbonButton_ClickedEvent)
{
    int  clicks = 0;
    auto id     = btnStyle->clicked.addHandler([&clicks](guifw::RibbonButton&, vine::EventArgs&) { ++clicks; });

    auto* tb = btnStyle->impl<SARibbonToolButton>();
    ASSERT_NE(tb, nullptr);
    tb->click();
    EXPECT_EQ(clicks, 1);
    tb->click();
    tb->click();
    EXPECT_EQ(clicks, 3);

    btnStyle->clicked.removeHandler(id);
    tb->click();
    EXPECT_EQ(clicks, 3); // 移除后不再触发
}

TEST_F(GuiTest, RibbonGroup_Methods)
{
    // ---- 布局模式（默认值取决于 SARibbon 全局风格，显式设置后再读回）----
    groupClip->setLayoutMode(guifw::RibbonPanelLayoutMode::TwoRow);
    EXPECT_EQ(groupClip->layoutMode(), guifw::RibbonPanelLayoutMode::TwoRow);
    groupClip->setLayoutMode(guifw::RibbonPanelLayoutMode::ThreeRow);
    EXPECT_EQ(groupClip->layoutMode(), guifw::RibbonPanelLayoutMode::ThreeRow);

    // ---- 扩展 / 自定义 ----
    groupClip->setExpanding(true);
    EXPECT_TRUE(groupClip->expanding());
    groupClip->setExpanding(false);

    groupClip->setCanCustomize(false);
    EXPECT_FALSE(groupClip->canCustomize());
    groupClip->setCanCustomize(true);
    EXPECT_TRUE(groupClip->canCustomize());

    // ---- 面板级图标尺寸往返 ----
    groupClip->setLargeIconSize(guifw::Size(40, 40));
    groupClip->setSmallIconSize(guifw::Size(16, 16));
    auto ls = groupClip->largeIconSize();
    auto ss = groupClip->smallIconSize();
    EXPECT_EQ(ls.x, 40);
    EXPECT_EQ(ls.y, 40);
    EXPECT_EQ(ss.x, 16);
    EXPECT_EQ(ss.y, 16);

    // ---- 批量样式 ----
    groupClip->setIconRightText(true);
    EXPECT_TRUE(groupClip->iconRightText());
    groupClip->setWordWrap(true);
    EXPECT_TRUE(groupClip->wordWrap());

    // ---- 分隔线 ----
    auto* pnl = groupClip->impl<SARibbonPanel>();
    ASSERT_NE(pnl, nullptr);
    int before = pnl->layout()->count();
    groupClip->addSeparator();
    EXPECT_EQ(pnl->layout()->count(), before + 1);

    // ---- 选项按钮 ----
    auto* mi = new guifw::RibbonAction();
    mi->setText(u8"More");
    groupClip->setOptionAction(mi);
    EXPECT_EQ(groupClip->optionAction(), mi);
    groupClip->setOptionAction(nullptr);
    EXPECT_EQ(groupClip->optionAction(), nullptr);
}

TEST_F(GuiTest, RibbonGroup_AddWidget)
{
    // 通用控件容器：包一个 QComboBox 放进组
    auto* combo = new QComboBox();
    combo->addItem(u8"10");
    combo->addItem(u8"12");
    combo->addItem(u8"14");
    auto* we = new guifw::Control(combo);
    groupClip->addControl(we, guifw::RibbonItemSize::Medium);

    // 控件已被面板接管（父变为面板）
    ASSERT_NE(combo->parentWidget(), nullptr);

    // 移除后控件被置空父并延迟释放
    groupClip->removeControl(we);
    EXPECT_EQ(combo->parentWidget(), nullptr);
}

TEST_F(GuiTest, Control_CommonProps)
{
    // 通用控件容器暴露 QWidget 级属性（enabled/tooltip/size 等）
    auto* combo = new QComboBox();
    auto* ctrl  = new guifw::Control(combo);

    ctrl->setEnabled(false);
    EXPECT_FALSE(ctrl->enabled());
    ctrl->setEnabled(true);
    EXPECT_TRUE(ctrl->enabled());

    ctrl->setVisible(false);
    EXPECT_FALSE(ctrl->visible());

    ctrl->setTooltip(u8"hi");
    EXPECT_TRUE(ctrl->tooltip() == u8"hi");

    ctrl->setSize(guifw::Size(400, 40));
    EXPECT_EQ(ctrl->width(), 400);
    EXPECT_EQ(ctrl->height(), 40);
    EXPECT_EQ(ctrl->size().x, 400);
    EXPECT_EQ(ctrl->size().y, 40);

    delete ctrl;
}

TEST_F(GuiTest, ConfigManager_Basic)
{
    auto* cfg = new vine::appfw::ConfigManager();

    // 标量
    cfg->setString(u8"name", u8"Vine");
    cfg->setInt(u8"max", 100);
    cfg->setBool(u8"flag", true);
    cfg->setDouble(u8"ratio", 1.5);
    EXPECT_TRUE(cfg->contains(u8"name"));
    EXPECT_TRUE(cfg->getString(u8"name") == u8"Vine");
    EXPECT_EQ(cfg->getInt(u8"max"), 100);
    EXPECT_TRUE(cfg->getBool(u8"flag"));
    EXPECT_DOUBLE_EQ(cfg->getDouble(u8"ratio"), 1.5);
    EXPECT_EQ(cfg->getInt(u8"missing"), 0); // 缺省返回默认值
    EXPECT_FALSE(cfg->contains(u8"missing"));

    // 数组
    cfg->setIntArray(u8"nums", { 1, 2, 3 });
    cfg->setStringArray(u8"names", { u8"a", u8"b" });
    auto nums = cfg->getIntArray(u8"nums");
    ASSERT_EQ(nums.size(), 3u);
    EXPECT_EQ(nums[0], 1);
    EXPECT_EQ(nums[2], 3);
    auto names = cfg->getStringArray(u8"names");
    ASSERT_EQ(names.size(), 2u);
    EXPECT_TRUE(names[1] == u8"b");

    // JSON 往返（类型无损）
    auto* cfg2 = new vine::appfw::ConfigManager();
    EXPECT_TRUE(cfg2->loadJson(cfg->toJson()));
    EXPECT_TRUE(cfg2->getString(u8"name") == u8"Vine");
    EXPECT_EQ(cfg2->getInt(u8"max"), 100);
    EXPECT_TRUE(cfg2->getBool(u8"flag"));
    EXPECT_DOUBLE_EQ(cfg2->getDouble(u8"ratio"), 1.5);
    auto nums2 = cfg2->getIntArray(u8"nums");
    ASSERT_EQ(nums2.size(), 3u);
    EXPECT_EQ(nums2[2], 3);
    EXPECT_EQ(cfg2->getInt(u8"max"), 100);

    // 层级 key（A.B.C）
    cfg->setInt(u8"window.x", 100);
    cfg->setInt(u8"window.y", 200);
    cfg->setBool(u8"window.state.maximized", true);
    cfg->setString(u8"editor.font.name", u8"Consolas");
    EXPECT_TRUE(cfg->contains(u8"window.x"));
    EXPECT_EQ(cfg->getInt(u8"window.x"), 100);
    EXPECT_TRUE(cfg->getBool(u8"window.state.maximized"));
    EXPECT_TRUE(cfg->getString(u8"editor.font.name") == u8"Consolas");

    // 序列化为嵌套 JSON 对象（非点分扁平 key）
    const auto          json = cfg->toJson();
    QJsonParseError     pe;
    const QJsonDocument nested = QJsonDocument::fromJson(QByteArray(reinterpret_cast<const char*>(json.data()), static_cast<int>(json.size())), &pe);
    ASSERT_EQ(pe.error, QJsonParseError::NoError);
    const QJsonObject root = nested.object();
    EXPECT_TRUE(root.contains(QStringLiteral("window")));
    EXPECT_TRUE(root.value(QStringLiteral("window")).toObject().contains(QStringLiteral("x")));
    EXPECT_TRUE(root.value(QStringLiteral("window")).toObject().value(QStringLiteral("state")).toObject().contains(QStringLiteral("maximized")));
    EXPECT_FALSE(root.contains(QStringLiteral("window.x"))); // 不出现扁平点分 key

    // 层级 JSON 往返
    auto* cfg3 = new vine::appfw::ConfigManager();
    EXPECT_TRUE(cfg3->loadJson(json));
    EXPECT_EQ(cfg3->getInt(u8"window.x"), 100);
    EXPECT_EQ(cfg3->getInt(u8"window.y"), 200);
    EXPECT_TRUE(cfg3->getBool(u8"window.state.maximized"));
    EXPECT_TRUE(cfg3->getString(u8"editor.font.name") == u8"Consolas");
    delete cfg3;

    // 非法 JSON
    EXPECT_FALSE(cfg2->loadJson(u8"not json"));

    delete cfg;
    delete cfg2;

    // Application 持有引用
    auto* app = GuiEnv::app.get();
    ASSERT_NE(app, nullptr);
    ASSERT_NE(app->configManager(), nullptr);
    app->configManager()->setInt(u8"appmax", 42);
    EXPECT_EQ(app->configManager()->getInt(u8"appmax"), 42);
}

// ============================ 可显示配置 ============================

TEST_F(GuiTest, ConfigItem_Descriptor)
{
    using vine::appfw::ConfigItem;
    using vine::appfw::ConfigItemType;

    ConfigItem item(u8"editor.font.size", u8"字号", ConfigItemType::Int);
    item.description(u8"编辑器字号").range(8, 72).defaultValue(14).step(2);
    EXPECT_TRUE(item.key() == u8"editor.font.size");
    EXPECT_TRUE(item.label() == u8"字号");
    EXPECT_TRUE(item.description() == u8"编辑器字号");
    EXPECT_EQ(item.type(), ConfigItemType::Int);
    EXPECT_TRUE(item.hasRange());
    EXPECT_EQ(item.minInt(), 8);
    EXPECT_EQ(item.maxInt(), 72);
    EXPECT_EQ(item.step(), 2.0);
    EXPECT_TRUE(item.hasDefault());
    EXPECT_EQ(item.defaultInt(), 14);

    // 默认值重载：char8_t* 必须走 String 而非 bool
    ConfigItem s(u8"name", u8"名称", ConfigItemType::String);
    s.defaultValue(u8"Vine");
    EXPECT_TRUE(s.hasDefault());
    EXPECT_TRUE(s.defaultString() == u8"Vine");

    ConfigItem c(u8"theme", u8"主题", ConfigItemType::Choice);
    c.choices({ u8"浅色", u8"深色" });
    EXPECT_EQ(c.choices().size(), 2u);
    EXPECT_TRUE(c.choices()[1].description == u8"深色");
    EXPECT_FALSE(c.hasDefault());
}

TEST_F(GuiTest, ConfigItem_DefaultTypeCheck)
{
    using vine::appfw::ConfigItem;
    using vine::appfw::ConfigItemType;

    // No default set -> hasDefault() false; wrong-type getter throws.
    ConfigItem none(u8"k", u8"K", ConfigItemType::Int);
    EXPECT_FALSE(none.hasDefault());
    EXPECT_THROW(none.defaultInt(), std::bad_any_cast);

    // Correct type accepted.
    ConfigItem i(u8"size", u8"大小", ConfigItemType::Int);
    i.defaultValue(10);
    EXPECT_TRUE(i.hasDefault());
    EXPECT_EQ(i.defaultInt(), 10);

    // String default is valid for Choice too.
    ConfigItem c(u8"theme", u8"主题", ConfigItemType::Choice);
    c.defaultValue(u8"浅色");
    EXPECT_TRUE(c.hasDefault());
    EXPECT_TRUE(c.defaultString() == u8"浅色");

    // Wrong-type setter throws std::invalid_argument.
    ConfigItem b(u8"flag", u8"开关", ConfigItemType::Bool);
    EXPECT_THROW(b.defaultValue(3), std::invalid_argument);
    EXPECT_THROW(ConfigItem(u8"x", u8"X", ConfigItemType::Double).defaultValue(true), std::invalid_argument);
}

TEST_F(GuiTest, ConfigItem_RangeAny)
{
    using vine::appfw::ConfigItem;
    using vine::appfw::ConfigItemType;

    // No range -> hasRange() false; min/max getters throw; step() falls back to 1.0.
    ConfigItem none(u8"k", u8"K", ConfigItemType::Int);
    EXPECT_FALSE(none.hasRange());
    EXPECT_THROW(none.minInt(), std::bad_any_cast);
    EXPECT_EQ(none.step(), 1.0);

    // Int range with default step, then override.
    ConfigItem i(u8"size", u8"大小", ConfigItemType::Int);
    i.range(8, 72);
    EXPECT_TRUE(i.hasRange());
    EXPECT_EQ(i.minInt(), 8);
    EXPECT_EQ(i.maxInt(), 72);
    EXPECT_EQ(i.step(), 1.0);
    i.step(2);
    EXPECT_EQ(i.step(), 2.0);

    // Double range.
    ConfigItem d(u8"ratio", u8"比例", ConfigItemType::Double);
    d.range(0.0, 10.0);
    EXPECT_DOUBLE_EQ(d.minDouble(), 0.0);
    EXPECT_DOUBLE_EQ(d.maxDouble(), 10.0);

    // Wrong usage throws.
    ConfigItem s(u8"name", u8"名称", ConfigItemType::String);
    EXPECT_THROW(s.range(1, 2), std::invalid_argument);
    EXPECT_THROW(ConfigItem(u8"x", u8"X", ConfigItemType::Int).step(0.5), std::invalid_argument);
}

TEST_F(GuiTest, ConfigItem_TypedChoices)
{
    using vine::String;
    using vine::appfw::ConfigItem;
    using vine::appfw::ConfigItemType;

    // Int-valued choices with descriptions.
    ConfigItem i(u8"theme", u8"主题", ConfigItemType::Choice);
    i.choices({
                { 0, u8"浅色"     },
                { 1, u8"深色"     },
                { 2, u8"跟随系统" }
    })
        .defaultValue(1);
    ASSERT_EQ(i.choices().size(), 3u);
    EXPECT_TRUE(i.choices()[0].description == u8"浅色");
    EXPECT_EQ(*std::any_cast<int>(&i.choices()[1].value), 1);
    EXPECT_TRUE(i.hasDefault());
    EXPECT_EQ(i.defaultType(), ConfigItemType::Int);
    EXPECT_EQ(i.defaultInt(), 1);

    // Double-valued choices.
    ConfigItem d(u8"size", u8"大小", ConfigItemType::Choice);
    d.choices({
      { 12.0, u8"小" },
      { 14.0, u8"中" }
    });
    ASSERT_EQ(d.choices().size(), 2u);
    EXPECT_EQ(*std::any_cast<double>(&d.choices()[1].value), 14.0);

    // String-valued choices (value + description).
    ConfigItem s(u8"enc", u8"编码", ConfigItemType::Choice);
    s.choices({
      { u8"utf8", u8"UTF-8" },
      { u8"gbk",  u8"GBK"   }
    });
    EXPECT_TRUE(*std::any_cast<String>(&s.choices()[0].value) == u8"utf8");
    EXPECT_TRUE(s.choices()[0].description == u8"UTF-8");
}

TEST_F(GuiTest, ConfigRegistry_Register)
{
    using vine::appfw::ConfigItem;
    using vine::appfw::ConfigItemType;
    using vine::appfw::ConfigRegistry;

    ConfigRegistry reg;
    auto*          cat1 = reg.addCategory(u8"C1");
    ASSERT_NE(cat1, nullptr);
    auto* g1 = cat1->addGroup(u8"G1");
    ASSERT_NE(g1, nullptr);
    EXPECT_TRUE(g1->addItem(ConfigItem(u8"a", u8"A", ConfigItemType::String)));
    EXPECT_TRUE(g1->addItem(ConfigItem(u8"c", u8"C", ConfigItemType::Int)));
    auto* g2 = cat1->addGroup(u8"G2");
    ASSERT_NE(g2, nullptr);
    EXPECT_TRUE(g2->addItem(ConfigItem(u8"b", u8"B", ConfigItemType::Bool)));

    // Duplicate name rejected
    EXPECT_EQ(cat1->addGroup(u8"G1"), nullptr);
    EXPECT_EQ(reg.addCategory(u8"C1"), nullptr);

    // Duplicate key rejected (whole tree)
    EXPECT_FALSE(g2->addItem(ConfigItem(u8"a", u8"A2", ConfigItemType::String)));

    EXPECT_EQ(reg.itemCount(), 3);

    // Whole-tree lookup
    EXPECT_TRUE(reg.item(u8"b")->label() == u8"B");
    EXPECT_EQ(reg.item(u8"nope"), nullptr);
    EXPECT_TRUE(reg.item(u8"c")->key() == u8"c");
    // In-group lookup
    EXPECT_TRUE(g1->item(u8"a") != nullptr);
    EXPECT_EQ(g1->item(u8"b"), nullptr);

    // Category/group order preserved
    ASSERT_EQ(reg.categories().size(), 1u);
    auto groups = cat1->groups();
    ASSERT_EQ(groups.size(), 2u);
    EXPECT_TRUE(groups[0]->name() == u8"G1");
    EXPECT_TRUE(groups[1]->name() == u8"G2");

    // Removal / clear
    EXPECT_TRUE(reg.removeItem(u8"b"));
    EXPECT_FALSE(reg.removeItem(u8"b"));
    EXPECT_EQ(reg.itemCount(), 2);
    EXPECT_EQ(reg.item(u8"b"), nullptr);
    EXPECT_TRUE(cat1->removeGroup(u8"G2"));
    EXPECT_FALSE(cat1->removeGroup(u8"G2"));
    EXPECT_TRUE(reg.removeCategory(u8"C1"));
    EXPECT_FALSE(reg.removeCategory(u8"C1"));
    EXPECT_EQ(reg.itemCount(), 0);
    reg.clear();
    EXPECT_EQ(reg.categories().size(), 0u);
}

TEST_F(GuiTest, ConfigRegistry_MetaAndOrder)
{
    using vine::appfw::ConfigRegistry;

    ConfigRegistry reg;
    auto*          a = reg.addCategory(u8"a");
    ASSERT_NE(a, nullptr);
    a->label(u8"AA").description(u8"desc").order(3);
    EXPECT_TRUE(a->label() == u8"AA");
    EXPECT_TRUE(a->description() == u8"desc");
    EXPECT_EQ(a->order(), 3);

    auto* g = a->addGroup(u8"g");
    ASSERT_NE(g, nullptr);
    g->label(u8"GG").order(1);
    EXPECT_TRUE(g->label() == u8"GG");
    EXPECT_EQ(g->order(), 1);

    // Display order: smaller order first (ties keep insertion order)
    auto* b = reg.addCategory(u8"b");
    ASSERT_NE(b, nullptr);
    b->order(0);
    auto cats = reg.categories();
    ASSERT_EQ(cats.size(), 2u);
    EXPECT_TRUE(cats[0]->name() == u8"b");
    EXPECT_TRUE(cats[1]->name() == u8"a");
}

TEST_F(GuiTest, ConfigManager_ChangedEvent)
{
    auto*        cfg   = new vine::appfw::ConfigManager();
    int          fired = 0;
    vine::String lastKey;
    auto         id = cfg->changed.addHandler([&](vine::appfw::ConfigManager&, vine::appfw::ConfigChangedEventArgs& args) {
        ++fired;
        lastKey = args.key();
    });

    cfg->setInt(u8"window.x", 1);
    cfg->setBool(u8"flag", true);
    cfg->setString(u8"name", u8"a");
    cfg->setDouble(u8"ratio", 0.5);
    cfg->setIntArray(u8"nums", { 1, 2 });
    EXPECT_EQ(fired, 5);
    EXPECT_TRUE(lastKey == u8"nums");

    cfg->remove(u8"window.x");
    EXPECT_EQ(fired, 6);
    EXPECT_TRUE(lastKey == u8"window.x");

    cfg->changed.removeHandler(id);
    cfg->setInt(u8"after", 9);
    EXPECT_EQ(fired, 6); // 已移除 handler

    delete cfg;
}

TEST_F(GuiTest, PluginLoadContext_Configs)
{
    auto* app = GuiEnv::app.get();
    ASSERT_NE(app, nullptr);

    vine::appfw::PluginLoadContext ctx(app);
    EXPECT_EQ(ctx.application(), app);
    ASSERT_NE(ctx.configs(), nullptr);
    EXPECT_EQ(ctx.configs(), app->configRegistry());
    EXPECT_NE(ctx.eventBus(), nullptr);
    EXPECT_EQ(ctx.eventBus(), app->eventBus());

    // Registering through the context targets the same registry as Application
    auto* pluginCat = ctx.configs()->addCategory(u8"插件");
    ASSERT_NE(pluginCat, nullptr);
    pluginCat->addGroup(u8"常规")->addItem(vine::appfw::ConfigItem(u8"plugin.opt", u8"插件选项", vine::appfw::ConfigItemType::Bool));
    EXPECT_NE(app->configRegistry()->item(u8"plugin.opt"), nullptr);
    EXPECT_TRUE(ctx.configs()->removeItem(u8"plugin.opt"));
    EXPECT_TRUE(ctx.configs()->removeCategory(u8"插件"));
}

TEST_F(GuiTest, CommandManager_RegistrationOwner)
{
    auto* app = GuiEnv::app.get();
    ASSERT_NE(app, nullptr);
    auto* cm = app->commandManager();
    ASSERT_NE(cm, nullptr);

    const auto pluginName = vine::String(u8"testPlugin");
    const auto otherName  = vine::String(u8"myCommand");
    const auto ownedName  = vine::String(u8"pluginCommand");

    // 清理可能残留的注册，保证用例可重复运行。
    cm->unregisterCommand(otherName);
    cm->unregisterCommand(ownedName);

    // 主机命令：无 owner。
    cm->setRegistrationOwner({});
    ASSERT_TRUE(cm->registerCommand<DummyCommand>(otherName));

    // 插件命令：注册期间打上 owner。
    cm->setRegistrationOwner(pluginName);
    ASSERT_TRUE(cm->registerCommand<DummyCommand>(ownedName));
    cm->setRegistrationOwner({});

    // 报告接口：只返回该插件注册的命令，且 owner 正确。
    const auto owned = cm->commandInfosForPlugin(pluginName);
    ASSERT_EQ(owned.size(), 1u);
    EXPECT_TRUE(owned[0].name == ownedName);
    EXPECT_TRUE(owned[0].owner == pluginName);

    const auto others = cm->commandInfosForPlugin(otherName);
    EXPECT_TRUE(others.empty());

    // 全量枚举中 owner 字段按注册来源标记。
    for (const auto& info : cm->commandInfos()) {
        if (info.name == ownedName) {
            EXPECT_TRUE(info.owner == pluginName);
        } else if (info.name == otherName) {
            EXPECT_TRUE(info.owner.empty());
        }
    }

    // 清理注册，避免影响其它用例。
    cm->unregisterCommand(otherName);
    cm->unregisterCommand(ownedName);
}

TEST_F(GuiTest, ConfigRegistry_StandardCategories)
{
    using vine::appfw::ConfigItem;
    using vine::appfw::ConfigItemType;
    using vine::appfw::ConfigRegistry;
    using vine::appfw::StandardCategory;
    using vine::appfw::StandardGroup;

    ConfigRegistry reg;

    // standardCategory 是 create-or-get：重复调用返回同一节点，规范名 + label + order。
    auto* log1 = reg.standardCategory(StandardCategory::Logging);
    auto* log2 = reg.standardCategory(StandardCategory::Logging);
    ASSERT_NE(log1, nullptr);
    EXPECT_EQ(log1, log2);
    EXPECT_TRUE(log1->name() == u8"logging");
    EXPECT_TRUE(log1->label() == u8"日志");

    // standardGroup 是 create-or-get：规范名 + label。
    auto* console = reg.standardGroup(StandardCategory::Logging, StandardGroup::Console);
    ASSERT_NE(console, nullptr);
    EXPECT_TRUE(console->name() == u8"console");
    EXPECT_TRUE(console->label() == u8"控制台");
    EXPECT_EQ(reg.standardGroup(StandardCategory::Logging, StandardGroup::Console), console);

    // addItem 便捷接口
    EXPECT_TRUE(reg.addItem(StandardCategory::Logging, StandardGroup::Console,
                            ConfigItem(u8"logging.console_enabled", u8"日志输出到控制台", ConfigItemType::Bool), u8"app_shell"));
    EXPECT_NE(reg.item(u8"logging.console_enabled"), nullptr);

    // 重复 key 拒绝
    EXPECT_FALSE(reg.addItem(StandardCategory::Logging, StandardGroup::Console,
                             ConfigItem(u8"logging.console_enabled", u8"x", ConfigItemType::Bool), u8"other"));
}

TEST_F(GuiTest, ConfigRegistry_Ownership)
{
    using vine::appfw::ConfigItem;
    using vine::appfw::ConfigItemType;
    using vine::appfw::ConfigRegistry;
    using vine::appfw::StandardCategory;
    using vine::appfw::StandardGroup;

    ConfigRegistry reg;
    reg.addItem(StandardCategory::Logging, StandardGroup::Console,
                ConfigItem(u8"logging.console_enabled", u8"日志输出到控制台", ConfigItemType::Bool), u8"app_shell");
    reg.addItem(StandardCategory::Appearance, StandardGroup::Theme,
                ConfigItem(u8"appearance.theme", u8"主题", ConfigItemType::String), u8"app_shell");
    reg.addItem(StandardCategory::General, StandardGroup::Startup,
                ConfigItem(u8"general.maximize", u8"最大化启动", ConfigItemType::Bool), u8"other_plugin");

    auto app_items = reg.itemsForPlugin(u8"app_shell");
    ASSERT_EQ(app_items.size(), 2u);
    auto other_items = reg.itemsForPlugin(u8"other_plugin");
    ASSERT_EQ(other_items.size(), 1u);
    EXPECT_TRUE(other_items[0]->key() == u8"general.maximize");

    // removeItemsForPlugin 只删该插件的项
    EXPECT_TRUE(reg.removeItemsForPlugin(u8"app_shell"));
    EXPECT_TRUE(reg.itemsForPlugin(u8"app_shell").empty());
    EXPECT_EQ(reg.item(u8"logging.console_enabled"), nullptr);
    EXPECT_EQ(reg.item(u8"appearance.theme"), nullptr);
    EXPECT_NE(reg.item(u8"general.maximize"), nullptr); // 其它插件不受影响

    EXPECT_FALSE(reg.removeItemsForPlugin(u8"app_shell")); // 已无项可删
}

TEST_F(GuiTest, PluginLoadContext_RegisterConfigItem)
{
    auto* app = GuiEnv::app.get();
    ASSERT_NE(app, nullptr);

    vine::appfw::PluginLoadContext ctx(app, u8"my_plugin");
    EXPECT_TRUE(ctx.pluginName() == u8"my_plugin");

    using vine::appfw::ConfigItem;
    using vine::appfw::ConfigItemType;
    using vine::appfw::StandardCategory;
    using vine::appfw::StandardGroup;

    EXPECT_TRUE(ctx.registerConfigItem(StandardCategory::Logging, StandardGroup::Console,
                                       ConfigItem(u8"logging.file_path", u8"日志文件", ConfigItemType::String)));

    auto items = ctx.registeredConfigs();
    ASSERT_EQ(items.size(), 1u);
    EXPECT_TRUE(items[0]->key() == u8"logging.file_path");

    // 清理共享 registry，避免污染其它测试。
    app->configRegistry()->removeItemsForPlugin(u8"my_plugin");
}

// ============================ 停靠面板 ============================

TEST_F(GuiTest, DockPanelManager_CountAndLookup)
{
    auto* mgr = wnd->dockPanelManager();
    EXPECT_EQ(mgr->count(), 5);
    EXPECT_EQ(mgr->panels().size(), 5u);

    auto* p = mgr->findById(u8"dock_project");
    ASSERT_NE(p, nullptr);
    EXPECT_TRUE(p->title() == u8"Project");
    EXPECT_EQ(p->features(), guifw::DockFeatures::None);

    EXPECT_NE(mgr->findByTitle(u8"Properties"), nullptr);
    EXPECT_EQ(mgr->findByTitle(u8"NotThere"), nullptr);
}

TEST_F(GuiTest, DockPanel_Features)
{
    EXPECT_FALSE(vine::testFlag(panelLeft->features(), guifw::DockFeatures::Closable));
    EXPECT_TRUE(vine::testFlag(panelRight->features(), guifw::DockFeatures::Closable));
    EXPECT_TRUE(vine::testFlag(panelTop->features(), guifw::DockFeatures::Closable));
}

TEST_F(GuiTest, DockPanel_Area)
{
    EXPECT_EQ(panelLeft->dockArea(), guifw::DockAreas::Left);
    EXPECT_EQ(panelRight->dockArea(), guifw::DockAreas::Right);
    EXPECT_EQ(panelTop->dockArea(), guifw::DockAreas::Top);
    EXPECT_EQ(panelBottom->dockArea(), guifw::DockAreas::Bottom);
    EXPECT_EQ(panelBottom2->dockArea(), guifw::DockAreas::Bottom);
}

TEST_F(GuiTest, DockPanel_CollapseRestore)
{
    auto* p = panelLeft;
    EXPECT_FALSE(p->isCollapsed());
    EXPECT_FALSE(p->isPinned());
    EXPECT_FALSE(p->isFloating());
    EXPECT_FALSE(p->isTabbed());

    p->collapse();
    QCoreApplication::processEvents();
    EXPECT_TRUE(p->isCollapsed());

    p->restore();
    QCoreApplication::processEvents();
    EXPECT_FALSE(p->isCollapsed());
    EXPECT_EQ(p->dockArea(), guifw::DockAreas::Left); // 恢复到原区域
}

TEST_F(GuiTest, DockPanel_PinUnpin)
{
    auto* p = panelRight;
    p->pin();
    QCoreApplication::processEvents();
    EXPECT_TRUE(p->isPinned());

    p->unpin();
    QCoreApplication::processEvents();
    EXPECT_FALSE(p->isPinned());
}

// ============================ 进度与串联 ============================

namespace
{

/// 泵送 Qt 事件循环一段时长（用于驱动 QTimer 轮询）。
void pumpEventsFor(int ms)
{
    const auto end = std::chrono::steady_clock::now() + std::chrono::milliseconds(ms);
    while (std::chrono::steady_clock::now() < end) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 20);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

} // namespace

TEST_F(GuiTest, Application_IsBusyReflectsActiveHost)
{
    auto* app = GuiEnv::app.get();
    ASSERT_NE(app, nullptr);
    EXPECT_FALSE(app->isBusy());

    {
        vine::progress::ProgressHost host;
        host.setForeground(true); // 只有前台宿主才让应用处于忙态
        EXPECT_TRUE(vine::progress::ProgressHost::isActive());
        EXPECT_TRUE(app->isBusy());
        EXPECT_TRUE(vine::progress::ProgressHost::current() == &host);
    }
    EXPECT_FALSE(vine::progress::ProgressHost::isActive());
    EXPECT_FALSE(app->isBusy());
}

TEST_F(GuiTest, CommandManager_LongRunningCreatesAmbientHost)
{
    auto* app = GuiEnv::app.get();
    ASSERT_NE(app, nullptr);
    auto* cm = app->commandManager();
    ASSERT_NE(cm, nullptr);

    const auto name = vine::String(u8"longCmd");
    cm->unregisterCommand(name);
    ASSERT_TRUE(cm->registerCommand<LongCommand>(name));

    EXPECT_FALSE(vine::progress::ProgressHost::isActive());

    // Task 是惰性的：放到工作线程上跑，主线程轮询观察宿主生命周期。
    auto                 task = cm->executeCommandAsync(name);
    std::atomic<bool>    done{ false };
    vine::appfw::CommandResult result;
    std::thread runner([&] {
        result = vine::async::syncWait(std::move(task));
        done.store(true);
    });

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(3);
    while (!vine::progress::ProgressHost::isActive() && std::chrono::steady_clock::now() < deadline) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    // 长命令运行期间，环境宿主已挂载、应用处于忙态。
    EXPECT_TRUE(vine::progress::ProgressHost::isActive());
    EXPECT_TRUE(app->isBusy());

    while (!done.load() && std::chrono::steady_clock::now() < deadline) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    runner.join();
    EXPECT_EQ(result.status(), vine::appfw::CommandStatus::Success);
    // 链结束后宿主释放、应用恢复空闲。
    EXPECT_FALSE(vine::progress::ProgressHost::isActive());
    EXPECT_FALSE(app->isBusy());

    cm->unregisterCommand(name);
}

TEST_F(GuiTest, CommandManager_BusyGateRejectsNewCommand)
{
    auto* app = GuiEnv::app.get();
    ASSERT_NE(app, nullptr);
    auto* cm = app->commandManager();
    ASSERT_NE(cm, nullptr);

    const auto name = vine::String(u8"busyDummy");
    cm->unregisterCommand(name);
    ASSERT_TRUE(cm->registerCommand<DummyCommand>(name));

    // 空闲时可执行。
    auto ok = cm->executeCommand(name);
    EXPECT_EQ(ok.status(), vine::appfw::CommandStatus::Success);

    {
        // 外部前台长任务持有宿主（模拟用户触发的耗时操作）。
        vine::progress::ProgressHost host;
        host.setForeground(true);
        EXPECT_TRUE(app->isBusy());

        // 忙时新命令被拒并提示。
        auto busy = cm->executeCommand(name);
        EXPECT_EQ(busy.status(), vine::appfw::CommandStatus::Failed);
        EXPECT_EQ(busy.message(), vine::String(u8"Another operation is in progress"));
    }

    // 宿主释放后可再次执行。
    auto after = cm->executeCommand(name);
    EXPECT_EQ(after.status(), vine::appfw::CommandStatus::Success);

    cm->unregisterCommand(name);
}

TEST(ProgressPresenterTest, ShowsBarForActiveHostAndHidesAfter)
{
    guifw::ProgressPresenter presenter;

    // 无宿主：不忙、隐藏。
    EXPECT_FALSE(presenter.isBusy());
    EXPECT_FALSE(presenter.visible());

    {
        vine::progress::ProgressHost host;
        host.setForeground(true); // 前台操作驱动主进度条
        std::thread worker([&] {
            vine::progress::ProgressScope scope(host.range(), "Exporting", 30);
            for (int i = 0; i < 30; ++i) {
                scope.next(1);
                std::this_thread::sleep_for(std::chrono::milliseconds(25));
            }
        });

        // 超过显示阈值后应出现进度条。
        pumpEventsFor(700);
        EXPECT_TRUE(presenter.isBusy());
        EXPECT_TRUE(presenter.visible());

        worker.join();
    }

    // 宿主结束后，延迟隐藏。
    pumpEventsFor(500);
    EXPECT_FALSE(presenter.isBusy());
    EXPECT_FALSE(presenter.visible());
}

TEST(ProgressPresenterTest, CancelButtonRequestsStop)
{
    guifw::ProgressPresenter     presenter;
    vine::progress::ProgressHost host;
    host.setForeground(true); // 取消按钮只作用于前台操作

    // 包装类不是 QObject：取消按钮在原生控件上查找。
    auto* cancel = presenter.impl<QWidget>()->findChild<QPushButton*>();
    ASSERT_NE(cancel, nullptr);

    pumpEventsFor(150); // 让 presenter 观察到宿主
    EXPECT_TRUE(presenter.isBusy());

    cancel->click();
    pumpEventsFor(50);

    EXPECT_TRUE(host.cancelSource().stop_requested());
    EXPECT_TRUE(host.indicator().isCancelled());
}

TEST_F(GuiTest, CommandManager_NestedProgressRunsChild)
{
    auto* app = GuiEnv::app.get();
    ASSERT_NE(app, nullptr);
    auto* cm = app->commandManager();
    ASSERT_NE(cm, nullptr);

    const auto name = vine::String(u8"nestedProgressCmd");
    cm->unregisterCommand(name);
    cm->unregisterCommand(u8"nested_child");
    ASSERT_TRUE(cm->registerCommand<NestedChildCommand>(u8"nested_child"));
    ASSERT_TRUE(cm->registerCommand<NestedProgressCommand>(name));

    EXPECT_FALSE(vine::progress::ProgressHost::isActive());

    // 父命令为 LongRunning → 前台宿主；内部经 executeChild 运行子命令（绕过串联门）。
    auto                 task = cm->executeCommandAsync(name);
    std::atomic<bool>    done{ false };
    vine::appfw::CommandResult result;
    std::thread runner([&] {
        result = vine::async::syncWait(std::move(task));
        done.store(true);
    });

    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(3);
    while (!vine::progress::ProgressHost::isActive() && std::chrono::steady_clock::now() < deadline) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    // 父命令运行期间：前台宿主挂载、应用忙。
    EXPECT_TRUE(vine::progress::ProgressHost::isActive());
    EXPECT_TRUE(app->isBusy());

    // 方案B：子命令压栈顶替父命令 → 前台栈出现深度 2。
    bool saw_depth2 = false;
    while (std::chrono::steady_clock::now() < deadline) {
        if (vine::progress::ProgressHost::foregroundStack().size() >= 2) {
            saw_depth2 = true;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    EXPECT_TRUE(saw_depth2);

    while (!done.load() && std::chrono::steady_clock::now() < deadline) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    runner.join();

    // 父命令成功 ⇒ 嵌套子命令未被串联门拦截（executeChild 绕过）。
    EXPECT_EQ(result.status(), vine::appfw::CommandStatus::Success);

    // 链结束后宿主释放。
    EXPECT_FALSE(vine::progress::ProgressHost::isActive());
    EXPECT_FALSE(app->isBusy());

    cm->unregisterCommand(name);
}
