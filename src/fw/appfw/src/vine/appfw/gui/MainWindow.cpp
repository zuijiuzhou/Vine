#include <vine/appfw/gui/MainWindow.hpp>

#include <QDockWidget>
#include <QSize>
#include <SARibbon.h>


#include <vine/Ptr.hpp>
#include <vine/appfw/gui/DockPanel.hpp>
#include <vine/appfw/gui/DockPanelManager.hpp>
#include <vine/appfw/gui/Gui.hpp>
#include <vine/appfw/gui/RibbonBar.hpp>
#include <vine/appfw/gui/StatusBar.hpp>

#include "vine/appfw/gui/Convert.hpp"
#include "vine/appfw/gui/UIElementData.hpp"

V_APPFWGUI_NS_BEGIN

V_OBJECT_META_IMPL(MainWindow, UIElement)

struct MainWindow::Data : public UIElementData {
    RibbonBar*        ribbon_bar = nullptr;
    StatusBar*        status_bar = nullptr;
    StartupPosition   startup_posi;
    WindowState       wnd_state;
    DockPanelManager* dock_panel_mgr = nullptr;
};

namespace
{

using itype = SARibbonMainWindow;

} // namespace

inline auto MainWindow::dptr() -> Data*
{ return static_cast<Data*>(UIElement::d); }

inline auto MainWindow::dptr() const -> const Data*
{ return static_cast<const Data*>(UIElement::d); }

MainWindow::MainWindow()
  : UIElement(new Data(), new SARibbonMainWindow(nullptr))
{
    dptr()->ribbon_bar     = new RibbonBar(this);
    dptr()->status_bar     = new StatusBar(this);
    dptr()->dock_panel_mgr = new DockPanelManager();
    dptr()->dock_panel_mgr->setWindow(this);

    impl<itype>()->setWindowTitle("Vine");
    impl<itype>()->setMinimumSize(QSize(800, 600));
    impl<itype>()->setCentralWidget(
        static_cast<QWidget*>(dptr()->dock_panel_mgr->root()->impl()));
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

void MainWindow::startupPosition(StartupPosition position)
{ dptr()->startup_posi = position; }

StartupPosition MainWindow::startupPosition() const
{ return dptr()->startup_posi; }

void MainWindow::windowState(WindowState state)
{
    dptr()->wnd_state = state;
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
{ impl<itype>()->activateWindow(); }

void MainWindow::setEnabled()
{ impl<itype>()->setEnabled(true); }

void MainWindow::setDisabled()
{ impl<itype>()->setEnabled(false); }

bool MainWindow::isActive() const
{ return impl<itype>()->isActiveWindow(); }

bool MainWindow::isEnabled() const
{ return impl<itype>()->isEnabled(); }

void MainWindow::show()
{ impl<itype>()->show(); }

void MainWindow::close()
{ impl<itype>()->close(); }

RibbonBar* MainWindow::ribbonBar() const
{ return dptr()->ribbon_bar; }

StatusBar* MainWindow::statusBar() const
{ return dptr()->status_bar; }

DockPanelManager* MainWindow::dockPanelManager() const
{ return dptr()->dock_panel_mgr; }

V_APPFWGUI_NS_END
