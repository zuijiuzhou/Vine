#include <vine/appfw/gui/DockArea.hpp>

#include <vine/appfw/gui/DockPanel.hpp>
#include <vine/appfw/gui/UIElementData.hpp>
#include <DockingPaneContainer.h>
#include <DockingPaneTabbedContainer.h>
#include <DockingPaneManager.h>

V_APPFWGUI_NS_BEGIN

V_OBJECT_META_IMPL(DockArea, UIElement)

struct DockArea::Data : public UIElementData {
    DockAreas   pos = DockAreas::Left;
};

inline auto DockArea::dptr() -> Data* { return static_cast<Data*>(UIElement::d); }
inline auto DockArea::dptr() const -> const Data* { return static_cast<const Data*>(UIElement::d); }

DockArea::DockArea(DockAreas position)
    : UIElement(new Data(), new DockingPaneTabbedContainer(nullptr))
{
    dptr()->pos = position;
}

DockArea::~DockArea()
{
    // d is deleted by UIElement
}

DockAreas DockArea::position() const
{
    return dptr()->pos;
}

int DockArea::dockPanelCount() const
{
    auto* c = impl<DockingPaneTabbedContainer>();
    return c ? c->getPaneCount() : 0;
}

DockPanel* DockArea::dockPanelAt(int index) const
{
    auto* c = impl<DockingPaneTabbedContainer>();
    if (!c) return nullptr;
    auto* dpc = c->getPane(index);
    if (!dpc) return nullptr;
    return static_cast<DockPanel*>(dpc->userData());
}

void DockArea::addDockPanel(DockPanel* panel)
{
    if (!panel) return;
    auto* dpc = panel->impl<DockingPaneContainer>();
    auto* c   = impl<DockingPaneTabbedContainer>();
    if (dpc && c) {
        dpc->setUserData(static_cast<void*>(panel));
        c->addPane(dpc);
    }
}

void DockArea::removeDockPanel(DockPanel* panel)
{
    if (!panel) return;
    auto* c = impl<DockingPaneTabbedContainer>();
    if (c) {
        auto* mgr = c->dockingManager();
        if (mgr) {
            auto* dpc = panel->impl<DockingPaneContainer>();
            if (dpc) mgr->closePane(dpc);
        }
    }
}

bool DockArea::containsDockPanel(DockPanel* panel) const
{
    if (!panel) return false;
    auto* dpc = panel->impl<DockingPaneContainer>();
    if (!dpc) return false;
    auto* c = impl<DockingPaneTabbedContainer>();
    if (!c) return false;
    for (int i = 0; i < c->getPaneCount(); ++i) {
        if (c->getPane(i) == dpc) return true;
    }
    return false;
}

bool DockArea::isEmpty() const
{
    return dockPanelCount() == 0;
}

V_APPFWGUI_NS_END
