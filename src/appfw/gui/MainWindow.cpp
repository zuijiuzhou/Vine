#include <SARibbon.h>

#include <appfw/gui/MainWindow.h>
#include <appfw/gui/RibbonBar.h>
#include <appfw/gui/StatusBar.h>
#include <core/Ptr.h>

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
using StartupPosition = MainWindow::StartupPosition;
using WindowState     = MainWindow::WindowState;
} // namespace

MainWindow::MainWindow()
  : Control(new SARibbonMainWindow(nullptr, true))
  , d(new Data) {
    size(Size(600, 400));
    d->ribbon_bar = new RibbonBar(this);
    d->status_bar = new StatusBar(this);

    // SAFramelessHelper* helper = impl<itype>()->framelessHelper();
    // helper->setRubberBandOnResize(false);
    impl<itype>()->setWindowTitle(("Vine"));

    impl<itype>()->setStatusBar(d->status_bar->impl<QStatusBar>());

    SARibbonBar* ribbon = impl<itype>()->ribbonBar();
    // 通过setContentsMargins设置ribbon四周的间距
    ribbon->setContentsMargins(5, 0, 5, 0);
    // 设置applicationButton
    ribbon->applicationButton()->setText(("File"));
}

MainWindow::~MainWindow() {
    delete d;
}

MainWindow* MainWindow::startupPosition(StartupPosition position) {
    d->startup_posi = position;
    return this;
}

StartupPosition MainWindow::startupPosition() const {
    return d->startup_posi;
}

MainWindow* MainWindow::windowState(WindowState state) {
    d->wnd_state = state;
    Qt::WindowState qstate;
    if (state == MINIMIZED)
        qstate = Qt::WindowState::WindowMinimized;
    else if (state == MAXIMIZED)
        qstate = Qt::WindowState::WindowMinimized;
    else
        qstate = Qt::WindowState::WindowNoState;
    impl<itype>()->setWindowState(qstate);
    return this;
}

WindowState MainWindow::windowState() const {
    WindowState state;
    auto        qstate = impl<itype>()->windowState();
    if (qstate & Qt::WindowState::WindowFullScreen)
        state = MAXIMIZED;
    else if (qstate & Qt::WindowState::WindowMaximized)
        state = MAXIMIZED;
    else if (qstate & Qt::WindowState::WindowMinimized)
        state = MINIMIZED;
    else
        state = NORMAL;
    return state;
}

MainWindow* MainWindow::activate() {
    auto qstate = impl<itype>()->windowState();
    impl<itype>()->activateWindow();
    return this;
}

MainWindow* MainWindow::setEnabled() {
    impl<itype>()->setEnabled(true);
    return this;
}

MainWindow* MainWindow::setDisabled() {
    impl<itype>()->setEnabled(false);
    return this;
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

VI_APPFWGUI_NS_END