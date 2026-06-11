#include <vine/appfw/gui/DockPanelManager.hpp>
#include <vine/appfw/gui/DockPanel.hpp>
#include <vine/appfw/gui/MainWindow.hpp>

#include <algorithm>

#include <QDockWidget>
#include <QMainWindow>
#include <QUuid>
#include <QSize>
#include <DockingPaneManager.h>
#include <DockingPaneBase.h>
#include <DockingPaneContainer.h>
#include <vine/appfw/gui/Convert.hpp>

V_APPFWGUI_NS_BEGIN

struct DockPanelManager::Data {
    DockingPaneManager*     dockingMgr = nullptr;
};

DockPanelManager::DockPanelManager()
  : d(new Data)
{
}

DockPanelManager::~DockPanelManager()
{
    delete d->dockingMgr;
    delete d;
}

void DockPanelManager::attachToWindow(MainWindow* wnd)
{
    if (!wnd || d->dockingMgr)
        return;
    d->dockingMgr = new DockingPaneManager();
    d->dockingMgr->setMainWindow(static_cast<QWidget*>(wnd->impl()));

    // Set up a default client widget so the docking framework has a
    // central area to anchor dock panels around.
    auto* clientWidget = new QWidget();
    d->dockingMgr->setClientWidget(clientWidget);

    // Replace the main window's central widget with the docking manager's
    // widget, which contains the client area and all docked panels.
    auto* mainWindow = static_cast<QMainWindow*>(wnd->impl());
    mainWindow->setCentralWidget(d->dockingMgr->widget());
}

void DockPanelManager::setCentralWidget(UIElement* widget)
{
    if (!d->dockingMgr || !widget)
        return;
    d->dockingMgr->setClientWidget(static_cast<QWidget*>(widget->impl()));
}

DockPanel* DockPanelManager::createDockPanel()
{
    auto p = new DockPanel();
    return p;
}

DockPanel* DockPanelManager::createDockPanel(DockAreas area)
{
    auto* p = createDockPanel();
    addDockPanel(p, area);
    return p;
}

void DockPanelManager::addDockPanel(DockPanel* panel)
{
    if (!panel)
        return;
    // No local list — DockingPaneManager tracks panes
}

void DockPanelManager::addDockPanel(DockPanel* panel, DockAreas area)
{
    if (!panel)
        return;

    if (!d->dockingMgr)
        return;

    auto* mgr = d->dockingMgr;

    auto* dp = panel->impl<DockingPaneBase>();
    if (!dp) {
        QString panelId = QUuid::createUuid().toString();
        auto    q8      = panelId.toUtf8();
        panel->setId(String(reinterpret_cast<const String::value_type*>(q8.constData()), q8.size()));
        auto  title  = panel->getTitle();
        QString qtitle = QString::fromUtf8(title.data(), static_cast<int>(title.size()));
        // Qt Debug asserts on addWidget(nullptr), so pass a temporary placeholder
        QWidget* placeholder = new QWidget();
        auto* newPane = mgr->createPane(panelId, qtitle, placeholder, QSize(200, 200), DockingPaneManager::dockFloat, nullptr);
        panel->attach(reinterpret_cast<UIObject*>(newPane));
        // Tag for later lookup via DockPanelManager::panels()
        auto* dpc = qobject_cast<DockingPaneContainer*>(newPane);
        if (dpc) {
            dpc->setProperty("_vine_dockpanel", QVariant::fromValue(static_cast<void*>(panel)));
            // Replace placeholder with actual content if set
            if (auto* content = panel->getContent()) {
                auto* w = static_cast<QWidget*>(content->impl());
                if (w) {
                    w->setParent(nullptr);
                    dpc->setClientWidget(w);
                }
            }
        }
        dp = newPane;
    }
    if (dp) {
        DockingPaneManager::DockPosition pos;
        if (testFlag(area, DockAreas::Right))
            pos = DockingPaneManager::dockRight;
        else if (testFlag(area, DockAreas::Top))
            pos = DockingPaneManager::dockTop;
        else if (testFlag(area, DockAreas::Bottom))
            pos = DockingPaneManager::dockBottom;
        else
            pos = DockingPaneManager::dockLeft;
        mgr->dockPane(dp, pos, nullptr);
    }
}

void DockPanelManager::removeDockPanel(DockPanel* panel)
{
    if (!panel)
        return;
    if (d->dockingMgr) {
        auto* dpc = panel->impl<DockingPaneBase>();
        if (dpc)
            d->dockingMgr->closePane(dpc);
    }
    delete panel;
}

DockPanel* DockPanelManager::findById(const String& id) const
{
    if (!d->dockingMgr)
        return nullptr;
    for (int i = 0; i < d->dockingMgr->paneCount(); ++i) {
        auto* pane = d->dockingMgr->pane(i);
        auto* dpc  = qobject_cast<DockingPaneContainer*>(pane);
        if (!dpc)
            continue;
        auto* wrapper = static_cast<DockPanel*>(dpc->property("_vine_dockpanel").value<void*>());
        if (wrapper && wrapper->getId() == id)
            return wrapper;
    }
    return nullptr;
}

DockPanel* DockPanelManager::findByTitle(const String& title) const
{
    if (!d->dockingMgr)
        return nullptr;
    for (int i = 0; i < d->dockingMgr->paneCount(); ++i) {
        auto* pane = d->dockingMgr->pane(i);
        auto* dpc  = qobject_cast<DockingPaneContainer*>(pane);
        if (!dpc)
            continue;
        auto* wrapper = static_cast<DockPanel*>(dpc->property("_vine_dockpanel").value<void*>());
        if (wrapper && wrapper->getTitle() == title)
            return wrapper;
    }
    return nullptr;
}

int DockPanelManager::count() const
{
    if (!d->dockingMgr)
        return 0;
    return d->dockingMgr->paneCount();
}

std::vector<DockPanel*> DockPanelManager::panels() const
{
    std::vector<DockPanel*> result;
    if (!d->dockingMgr)
        return result;
    for (int i = 0; i < d->dockingMgr->paneCount(); ++i) {
        auto* pane = d->dockingMgr->pane(i);
        auto* dpc  = qobject_cast<DockingPaneContainer*>(pane);
        if (!dpc)
            continue;
        auto* wrapper = static_cast<DockPanel*>(dpc->property("_vine_dockpanel").value<void*>());
        if (wrapper)
            result.push_back(wrapper);
    }
    return result;
}

V_APPFWGUI_NS_END
