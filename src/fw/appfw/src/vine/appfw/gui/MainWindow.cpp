#include <vine/appfw/gui/MainWindow.hpp>

#include <QDockWidget>
#include <QTabWidget>
#include <SARibbon.h>

#include <vine/appfw/gui/RibbonBar.hpp>
#include <vine/appfw/gui/StatusBar.hpp>
#include <vine/appfw/gui/Gui.hpp>
#include <vine/appfw/gui/DockPanel.hpp>
#include <vine/Ptr.hpp>

#include "Convert.hpp"

VI_APPFWGUI_NS_BEGIN

VI_OBJECT_META_IMPL(MainWindow, Control)

struct MainWindow::Data {
    RefPtr<RibbonBar> ribbon_bar;
    RefPtr<StatusBar> status_bar;
    StartupPosition   startup_posi;
    WindowState       wnd_state;
    bool              is_first_time_displayed = true;
};

namespace {
using itype           = SARibbonMainWindow;
} // namespace

MainWindow::MainWindow()
  : Control(new SARibbonMainWindow(nullptr))
  , d(new Data) {
    size(Size(600, 400));
    d->ribbon_bar = new RibbonBar(this);
    d->status_bar = new StatusBar(this);

    // SAFramelessHelper* helper = impl<itype>()->framelessHelper();
    // helper->setRubberBandOnResize(false);
    impl<itype>()->setWindowTitle(("Vine"));

    impl<itype>()->setStatusBar(d->status_bar->impl<QStatusBar>());

    auto ribbon = impl<itype>()->ribbonBar();
    // 通过setContentsMargins设置ribbon四周的间距
    ribbon->setContentsMargins(5, 0, 5, 0);
    // 设置applicationButton
    ribbon->applicationButton()->setText(("File"));

    auto central_widget = new QTabWidget();
    auto wnd            = impl<itype>();
    wnd->setCentralWidget(central_widget);
    
}

MainWindow::~MainWindow() {
    delete d;
}

void MainWindow::startupPosition(StartupPosition position) {
    d->startup_posi = position;
}

StartupPosition MainWindow::startupPosition() const {
    return d->startup_posi;
}

void MainWindow::windowState(WindowState state) {
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

WindowState MainWindow::windowState() const {
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

void MainWindow::activate() {
    auto qstate = impl<itype>()->windowState();
    impl<itype>()->activateWindow();
}

void MainWindow::setEnabled() {
    impl<itype>()->setEnabled(true);
}

void MainWindow::setDisabled() {
    impl<itype>()->setEnabled(false);
}

bool MainWindow::isActive() const {
    return impl<itype>()->isActiveWindow();
}

bool MainWindow::isEnabled() const {
    return impl<itype>()->isEnabled();
}

void MainWindow::show() {
    if (d->is_first_time_displayed) {
    }
    d->is_first_time_displayed = false;
    impl<itype>()->show();
}

void MainWindow::close() {
    impl<itype>()->close();
}

RibbonBar* MainWindow::ribbonBar() const {
    return d->ribbon_bar.get();
}

StatusBar* MainWindow::statusBar() const {
    return d->status_bar.get();
}

void MainWindow::addDockPanel(DockPanel* panel, DockAreas area){
    auto qdockpanel = panel->impl<QDockWidget>();
    auto qwnd = impl<itype>();
    auto qarea = Convert::toQDockAreas(area);
    qdockpanel->setAllowedAreas(qarea);
    qwnd->addDockWidget((Qt::DockWidgetArea)qarea.toInt(), qdockpanel);

}

VI_APPFWGUI_NS_END