#include <vine/appfw/gui/MainWindow.hpp>

#include <QDockWidget>
#include <SARibbon.h>
#include <QSize>

#include <vine/Ptr.hpp>
#include <vine/appfw/gui/DockPanel.hpp>
#include <vine/appfw/gui/DockPanelManager.hpp>
#include <vine/appfw/gui/Gui.hpp>
#include <vine/appfw/gui/RibbonBar.hpp>
#include <vine/appfw/gui/StatusBar.hpp>

#include "vine/appfw/gui/Convert.hpp"

V_APPFWGUI_NS_BEGIN

V_OBJECT_META_IMPL(MainWindow, UIElement)

struct MainWindow::Data {
    RibbonBar*           ribbon_bar               = nullptr;
    StatusBar*           status_bar               = nullptr;
    StartupPosition      startup_posi;
    WindowState          wnd_state;
    bool                 is_first_time_displayed  = true;
    DockPanelManager*    dock_panel_mgr           = nullptr;
};

namespace
{

using itype = SARibbonMainWindow;

} // namespace

MainWindow::MainWindow()
    : UIElement(new SARibbonMainWindow(nullptr))
    , d(new Data)
{
    d->ribbon_bar     = new RibbonBar(this);
    d->status_bar     = new StatusBar(this);
    d->dock_panel_mgr = new DockPanelManager();
    d->dock_panel_mgr->attachToWindow(this);

    // SAFramelessHelper* helper = impl<itype>()->framelessHelper();
    // helper->setRubberBandOnResize(false);
    impl<itype>()->setWindowTitle("Vine");

    impl<itype>()->setStatusBar(d->status_bar->impl<QStatusBar>());
    d->status_bar->setOwnsImpl(false);  // QMainWindow takes ownership of status bar

    auto ribbon = impl<itype>()->ribbonBar();
    // 通过setContentsMargins设置ribbon四周的间距
    ribbon->setContentsMargins(5, 0, 5, 0);
    // 设置applicationButton
    ribbon->applicationButton()->setText("File");
}

MainWindow::~MainWindow()
{
    delete d->dock_panel_mgr;
    delete d->ribbon_bar;
    delete d->status_bar;
    delete d;
}

void MainWindow::startupPosition(StartupPosition position)
{
    d->startup_posi = position;
}

StartupPosition MainWindow::startupPosition() const
{
    return d->startup_posi;
}

void MainWindow::windowState(WindowState state)
{
    d->wnd_state = state;
    Qt::WindowState qstate;
    if (state == WindowState::Minimized)
        qstate = Qt::WindowState::WindowMinimized;
    else if (state == WindowState::Maximized)
        qstate = Qt::WindowState::WindowMaximized;
    else
        qstate = Qt::WindowState::WindowNoState;
    impl<itype>()->setWindowState(qstate);
}

WindowState MainWindow::windowState() const
{
    WindowState state;
    auto        qstate = impl<itype>()->windowState();
    if (qstate & Qt::WindowState::WindowFullScreen)
        state = WindowState::Maximized;
    else if (qstate & Qt::WindowState::WindowMaximized)
        state = WindowState::Maximized;
    else if (qstate & Qt::WindowState::WindowMinimized)
        state = WindowState::Minimized;
    else
        state = WindowState::Normal;
    return state;
}

void MainWindow::activate()
{
    auto qstate = impl<itype>()->windowState();
    impl<itype>()->activateWindow();
}

void MainWindow::setEnabled()
{
    impl<itype>()->setEnabled(true);
}

void MainWindow::setDisabled()
{
    impl<itype>()->setEnabled(false);
}

bool MainWindow::isActive() const
{
    return impl<itype>()->isActiveWindow();
}

bool MainWindow::isEnabled() const
{
    return impl<itype>()->isEnabled();
}

void MainWindow::show()
{
    if (d->is_first_time_displayed) {
    }
    d->is_first_time_displayed = false;
    impl<itype>()->show();
}

void MainWindow::close()
{
    impl<itype>()->close();
}

RibbonBar* MainWindow::ribbonBar() const
{
    return d->ribbon_bar;
}

StatusBar* MainWindow::statusBar() const
{
    return d->status_bar;
}

DockPanelManager* MainWindow::dockPanelManager() const
{
    return d->dock_panel_mgr;
}

V_APPFWGUI_NS_END
