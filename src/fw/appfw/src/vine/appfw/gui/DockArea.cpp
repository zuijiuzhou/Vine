#include <vine/appfw/gui/DockArea.hpp>

#include <vine/appfw/gui/DockPanel.hpp>
#include <DockingPaneContainer.h>
#include <DockingPaneTabbedContainer.h>
#include <DockingPaneManager.h>

V_APPFWGUI_NS_BEGIN

V_OBJECT_META_IMPL(DockArea, UIElement)

struct DockArea::Data {
    DockAreas                           pos = DockAreas::Left;
    DockingPaneTabbedContainer*         container = nullptr;
};

DockArea::DockArea(DockAreas position)
    : UIElement(new DockingPaneTabbedContainer(nullptr))
    , d(new Data)
{
    d->pos       = position;
    d->container = impl<DockingPaneTabbedContainer>();
}

DockArea::~DockArea()
{
    delete d;
}

DockAreas DockArea::position() const
{
    return d->pos;
}

int DockArea::dockPanelCount() const
{
    if (!d->container)
        return 0;
    return d->container->getPaneCount();
}

DockPanel* DockArea::dockPanelAt(int index) const
{
    if (!d->container)
        return nullptr;
    auto* dpc = d->container->getPane(index);
    if (!dpc)
        return nullptr;
    // Recover the DockPanel wrapper from the DockingPaneContainer
    return static_cast<DockPanel*>(dpc->userData());
}

void DockArea::addDockPanel(DockPanel* panel)
{
    if (!panel)
        return;
    auto* dpc = panel->impl<DockingPaneContainer>();
    if (dpc && d->container) {
        dpc->setUserData(static_cast<void*>(panel));
        d->container->addPane(dpc);
    }
}

void DockArea::removeDockPanel(DockPanel* panel)
{
    if (!panel)
        return;
    if (d->container) {
        auto* mgr = d->container->dockingManager();
        if (mgr) {
            auto* dpc = panel->impl<DockingPaneContainer>();
            if (dpc)
                mgr->closePane(dpc);
        }
    }
}

bool DockArea::containsDockPanel(DockPanel* panel) const
{
    if (!panel || !d->container)
        return false;
    auto* dpc = panel->impl<DockingPaneContainer>();
    if (!dpc)
        return false;
    // Query the container directly
    for (int i = 0; i < d->container->getPaneCount(); ++i) {
        if (d->container->getPane(i) == dpc)
            return true;
    }
    return false;
}

bool DockArea::isEmpty() const
{
    return dockPanelCount() == 0;
}

V_APPFWGUI_NS_END
