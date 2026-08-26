#include <vine/appfw/gui/MainWindow.hpp>

#include <QApplication>
#include <QDockWidget>
#include <QPalette>
#include <QSize>
#include <QTimer>
#include <SARibbon.h>

#include <vine/Ptr.hpp>
#include <vine/appfw/Application.hpp>
#include <vine/appfw/gui/DockPanel.hpp>
#include <vine/appfw/gui/DockPanelManager.hpp>
#include <vine/appfw/gui/Gui.hpp>
#include <vine/appfw/gui/GuiApplication.hpp>
#include <vine/appfw/gui/RibbonBar.hpp>
#include <vine/appfw/gui/StatusBar.hpp>

#include "MainWindowImpl.hpp"
#include "UIElementData.hpp"

V_APPFWGUI_NS_BEGIN

V_OBJECT_META_IMPL(MainWindow, Window)

struct MainWindow::Data : public UIElementData {
    RibbonBar*        ribbon_bar = nullptr;
    StatusBar*        status_bar = nullptr;
    DockPanelManager* dock_panel_mgr = nullptr;
};

namespace
{

using itype = MainWindowImpl;

} // namespace

MainWindowImpl::MainWindowImpl(QWidget* parent)
  : SARibbonMainWindow(parent)
{
    // 订阅应用主题：GuiApplication 是主题的唯一决策者，这里只做映射与应用
    if (auto* app = obj_cast<GuiApplication>(Application::current())) {
        theme_handler_id_ = app->theme_changed.addHandler([this](Theme) { QTimer::singleShot(0, this, [this] { applyAppTheme(); }); });
    }

    QTimer::singleShot(0, this, [this] { applyAppTheme(); });
}

MainWindowImpl::~MainWindowImpl()
{
    if (auto* app = obj_cast<GuiApplication>(Application::current())) {
        app->theme_changed.removeHandler(theme_handler_id_);
    }
}

void MainWindowImpl::applyAppTheme()
{
    const auto* app = obj_cast<GuiApplication>(Application::current());
    if (app == nullptr) {
        return;
    }
    setRibbonTheme(app->theme() == Theme::Dark ? SARibbonTheme::RibbonThemeDark : SARibbonTheme::RibbonThemeOffice2021Blue);
}

MainWindow::MainWindow()
  : Window(new Data(), new MainWindowImpl(nullptr))
{
    dptr()->ribbon_bar     = new RibbonBar(this);
    dptr()->status_bar     = new StatusBar(this);
    dptr()->dock_panel_mgr = new DockPanelManager();
    dptr()->dock_panel_mgr->setWindow(this);

    impl<itype>()->setWindowTitle("Vine");
    impl<itype>()->setMinimumSize(QSize(800, 600));
    impl<itype>()->setCentralWidget(static_cast<QWidget*>(dptr()->dock_panel_mgr->root()->impl()));
    impl<itype>()->setStatusBar(dptr()->status_bar->impl<QStatusBar>());

    dptr()->status_bar->setOwnsImpl(true); // QMainWindow takes ownership of status bar

    auto ribbon = impl<itype>()->ribbonBar();
    ribbon->setContentsMargins(5, 0, 5, 0);
    ribbon->applicationButton()->setText("File");
}

MainWindow::~MainWindow()
{
    delete dptr()->dock_panel_mgr;
    delete dptr()->ribbon_bar;
    delete dptr()->status_bar;
    // d is deleted by UIElement
}

RibbonBar* MainWindow::ribbonBar() const
{
    return dptr()->ribbon_bar;
}

StatusBar* MainWindow::statusBar() const
{
    return dptr()->status_bar;
}

DockPanelManager* MainWindow::dockPanelManager() const
{
    return dptr()->dock_panel_mgr;
}

inline auto MainWindow::dptr() -> Data*
{
    return static_cast<Data*>(UIElement::d);
}

inline auto MainWindow::dptr() const -> const Data*
{
    return static_cast<const Data*>(UIElement::d);
}

V_APPFWGUI_NS_END
