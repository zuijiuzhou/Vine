#include <vine/appfw/gui/MainWindow.hpp>

#include <QDockWidget>
#include <QTabWidget>
#include <SARibbon.h>
#include "../../../../third_party/DockingPanes/src/DockingPaneManager.h"
#include <QUuid>
#include <QSize>

#include <vine/Ptr.hpp>
#include <vine/appfw/gui/DockPanel.hpp>
#include <vine/appfw/gui/Gui.hpp>
#include <vine/appfw/gui/RibbonBar.hpp>
#include <vine/appfw/gui/StatusBar.hpp>

#include "vine/appfw/gui/Convert.hpp"

V_APPFWGUI_NS_BEGIN

V_OBJECT_META_IMPL(MainWindow, UIElement)

struct MainWindow::Data {
    RibbonBar* ribbon_bar = nullptr;
    StatusBar* status_bar = nullptr;
    StartupPosition   startup_posi;
    WindowState       wnd_state;
    bool              is_first_time_displayed = true;
    DockingPaneManager* docking_manager = nullptr;
};

namespace
{

using itype = SARibbonMainWindow;

} // namespace

MainWindow::MainWindow()
    : UIElement(new SARibbonMainWindow(nullptr))
    , d(new Data)
{
    // size(Size(600, 400));
    d->ribbon_bar = new RibbonBar(this);
    d->status_bar = new StatusBar(this);

    // SAFramelessHelper* helper = impl<itype>()->framelessHelper();
    // helper->setRubberBandOnResize(false);
    impl<itype>()->setWindowTitle("Vine");

    impl<itype>()->setStatusBar(d->status_bar->impl<QStatusBar>());

    auto ribbon = impl<itype>()->ribbonBar();
    // 通过setContentsMargins设置ribbon四周的间距
    ribbon->setContentsMargins(5, 0, 5, 0);
    // 设置applicationButton
    ribbon->applicationButton()->setText("File");

    auto central_widget = new QTabWidget();
    auto wnd            = impl<itype>();
    wnd->setCentralWidget(central_widget);

    // initialize docking panes manager and attach to main window
    d->docking_manager = new DockingPaneManager();
    d->docking_manager->setMainWindow(impl<itype>());
}

MainWindow::~MainWindow()
{
    if (d->docking_manager) {
        delete d->docking_manager;
        d->docking_manager = nullptr;
    }
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

void MainWindow::addDockPanel(DockPanel* panel, DockAreas area)
{
    // if panel wraps DockingPanes, use DockingPaneManager to dock it
    if (d->docking_manager) {
        auto dp = panel->impl<DockingPaneBase>();
        if (!dp) {
            // create a pane via DockingPaneManager and attach it to the wrapper
            QString id = QUuid::createUuid().toString();
            auto title = panel->getTitle();
            auto utf16 = title.toUtf16();
            QString qtitle = QString::fromUtf16(reinterpret_cast<const ushort*>(utf16.data()), (int)utf16.size());
            QWidget* client = nullptr;
            if (auto content = panel->getContent())
                client = static_cast<QWidget*>(content->impl());
            auto newPane = d->docking_manager->createPane(id, qtitle, client, QSize(200, 200), DockingPaneManager::dockLeft, nullptr);
            panel->attach(newPane);
            dp = newPane;
        }
        if (dp) {
            // map DockAreas to DockPosition
            DockingPaneManager::DockPosition pos = DockingPaneManager::dockLeft;
            if (testFlag(area, DockAreas::Left))
                pos = DockingPaneManager::dockLeft;
            else if (testFlag(area, DockAreas::Right))
                pos = DockingPaneManager::dockRight;
            else if (testFlag(area, DockAreas::Top))
                pos = DockingPaneManager::dockTop;
            else if (testFlag(area, DockAreas::Bottom))
                pos = DockingPaneManager::dockBottom;
            d->docking_manager->dockPane(dp, pos, nullptr);
            return;
        }
    }

    // fallback to QDockWidget approach if present
    auto qdockpanel = panel->impl<QDockWidget>();
    if (qdockpanel) {
        auto qwnd  = impl<itype>();
        auto qarea = Convert::toQDockAreas(area);
        qdockpanel->setAllowedAreas(qarea);
        qwnd->addDockWidget((Qt::DockWidgetArea)qarea.toInt(), qdockpanel);
    }
}

V_APPFWGUI_NS_END
