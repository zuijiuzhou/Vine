#include <vine/appfw/gui/DockPanelManager.hpp>

#include <algorithm>

#include <DockingPaneBase.h>
#include <DockingPaneContainer.h>
#include <DockingPaneManager.h>
#include <QDockWidget>
#include <QMainWindow>
#include <QSize>
#include <QUuid>

#include <vine/appfw/gui/MainWindow.hpp>
#include <vine/appfw/gui/UIElement.hpp>
#include <vine/appfw/gui/DockPanel.hpp>

#include "Convert.hpp"


V_APPFWGUI_NS_BEGIN

namespace
{

/// Minimal UIElement wrapper around the docking manager's root QWidget.
class DockRootWidget final : public UIElement {
  public:
    explicit DockRootWidget(UIObject* impl)
      : UIElement(impl)
    {}
};

} // namespace

struct DockPanelManager::Data {
    DockingPaneManager* dockingMgr  = nullptr;
    UIElement*          rootElement = nullptr;
};

DockPanelManager::DockPanelManager()
  : d(new Data)
{ d->dockingMgr = new DockingPaneManager(); }

DockPanelManager::~DockPanelManager()
{
    delete d->dockingMgr;
    delete d->rootElement;
    delete d;
}

void DockPanelManager::setWindow(UIElement* wnd)
{
    // NOTE: d->dockingMgr is created in the ctor, so the old condition
    // "if (!wnd || d->dockingMgr) return;" always returned early and the
    // main window was never registered on the DockingPaneManager.
    if (!wnd || !d->dockingMgr)
        return;
    d->dockingMgr->setMainWindow(static_cast<QWidget*>(wnd->impl()));
}

void DockPanelManager::setCentralWidget(UIElement* widget)
{
    if (!d->dockingMgr || !widget)
        return;
    d->dockingMgr->setClientWidget(static_cast<QWidget*>(widget->impl()));
}

UIElement* DockPanelManager::root() const
{
    if (!d->dockingMgr)
        return nullptr;
    if (!d->rootElement)
        d->rootElement = new DockRootWidget(d->dockingMgr->widget());
    return d->rootElement;
}

DockPanel* DockPanelManager::createDockPanel(const String& title, DockAreas area)
{
    auto* panel = new DockPanel();
    if (!title.empty())
        panel->title(title);
    addDockPanel(panel, area);
    return panel;
}

DockPanel* DockPanelManager::createDockPanel(const String& title, UIElement* content, DockAreas area)
{
    auto* panel = new DockPanel();
    if (!title.empty())
        panel->title(title);
    if (content)
        panel->content(content);
    addDockPanel(panel, area);
    return panel;
}

void DockPanelManager::addDockPanel(DockPanel* panel, DockAreas area)
{
    if (!panel)
        return;

    if (!d->dockingMgr)
        return;

    auto* mgr = d->dockingMgr;

    auto* dp = panel->impl<DockingPaneContainer>();
    if (!dp) {
        // 保留调用方设置的 id；仅当未设置时才生成随机 id 保证唯一。
        QString panelId;
        if (panel->id().empty()) {
            panelId = QUuid::createUuid().toString();
            auto q8 = panelId.toUtf8();
            panel->id(String(reinterpret_cast<const String::value_type*>(q8.constData()), q8.size()));
        }
        else {
            panelId = QString::fromUtf8(panel->id().data(), static_cast<int>(panel->id().size()));
        }
        auto    title        = panel->title();
        QString qtitle       = QString::fromUtf8(title.data(), static_cast<int>(title.size()));
        // Qt Debug asserts on addWidget(nullptr), so pass a temporary placeholder
        QWidget* placeholder = new QWidget();
        auto*    newPane     = mgr->createPane(panelId, qtitle, placeholder, QSize(200, 200), DockingPaneManager::dockFloat, nullptr);
        panel->attach(reinterpret_cast<UIObject*>(newPane));
        // Tag for later lookup via DockPanelManager::panels()
        auto* dpc = qobject_cast<DockingPaneContainer*>(newPane);
        if (dpc) {
            dpc->setUserData(static_cast<void*>(panel));
            // Replace placeholder with actual content if set
            if (auto* content = panel->content()) {
                auto* w = static_cast<QWidget*>(content->impl());
                if (w) {
                    w->setParent(nullptr);
                    dpc->setClientWidget(w);
                    // The temporary placeholder widget was removed from the
                    // pane layout by setClientWidget(); drop it so we do not
                    // leak one QWidget per dock panel.
                    placeholder->deleteLater();
                }
            }
        }
        dp = dpc;
    }
    if (!dp)
        return;

    // ---- auto-tab: merge into tab group when same area already has panels ----
    DockingPaneManager::DockPosition pos;
    DockingPaneBase*                 neighbor = nullptr;

    // Convert DockAreas → DockingPaneManager::DockPosition
    if (testFlag(area, DockAreas::Right))
        pos = DockingPaneManager::dockRight;
    else if (testFlag(area, DockAreas::Top))
        pos = DockingPaneManager::dockTop;
    else if (testFlag(area, DockAreas::Bottom))
        pos = DockingPaneManager::dockBottom;
    else
        pos = DockingPaneManager::dockLeft;

    // Count existing panels in the same area
    int sameAreaCount = 0;
    for (int i = 0; i < mgr->paneCount(); ++i) {
        auto* other = mgr->pane(i);
        if (other == dp)
            continue;
        auto* oc = qobject_cast<DockingPaneContainer*>(other);
        if (!oc)
            continue;
        auto st = oc->state();
        if (st == DockingPaneBase::Docked || st == DockingPaneBase::Tabbed) {
            // Check if this panel is in the same target area via stored property
            auto areaVar = oc->property("_vine_dockarea");
            if (areaVar.isValid() && areaVar.toInt() == static_cast<int>(area)) {
                ++sameAreaCount;
                if (!neighbor)
                    neighbor = other; // first match = tab neighbour
            }
        }
    }

    if (neighbor)
        pos = DockingPaneManager::dockTab;

    mgr->dockPane(dp, pos, neighbor);

    // Remember which area this panel belongs to
    if (auto* dpc = qobject_cast<DockingPaneContainer*>(dp))
        dpc->setProperty("_vine_dockarea", static_cast<int>(area));
}

void DockPanelManager::removeDockPanel(DockPanel* panel)
{
    if (!panel)
        return;

    if (d->dockingMgr) {
        auto* dpc = panel->impl<DockingPaneContainer>();
        if (dpc) {
            d->dockingMgr->closePane(dpc);

            // If the close actually went through, the pane is now hidden and
            // no longer referenced by any tabbed container. Drop it from the
            // manager's bookkeeping: the wrapper below owns the container and
            // deletes it, so leaving it in the list would make panels()/
            // count()/findById() iterate a dangling pointer.
            //
            // If the close was vetoed (onClosing() returned false), the pane
            // is still alive and visible — keep the wrapper (and the pane)
            // untouched so nothing dangles.
            if (dpc->state() == DockingPaneBase::Hidden)
                d->dockingMgr->deletePane(dpc);
            else
                return;
        }
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
        auto* wrapper = static_cast<DockPanel*>(dpc->userData());
        if (wrapper && wrapper->id() == id)
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
        auto* wrapper = static_cast<DockPanel*>(dpc->userData());
        if (wrapper && wrapper->title() == title)
            return wrapper;
    }
    return nullptr;
}

int DockPanelManager::count() const
{
    // 只统计真正对应 DockPanel 包装对象的窗格；内部的 tabbed 容器（以及
    // 任何无 userData 的窗格）不参与计数。
    return static_cast<int>(panels().size());
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
        auto* wrapper = static_cast<DockPanel*>(dpc->userData());
        if (wrapper)
            result.push_back(wrapper);
    }
    return result;
}

V_APPFWGUI_NS_END
