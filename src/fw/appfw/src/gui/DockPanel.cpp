#include <vine/appfw/gui/DockPanel.hpp>

#include <DockingPaneContainer.h>
#include <DockingPaneManager.h>

#include "Convert.hpp"
#include <vine/appfw/gui/UIElementData.hpp>

V_APPFWGUI_NS_BEGIN

V_OBJECT_META_IMPL(DockPanel, Control)

// Helper to access protected members of DockingPaneContainer.
struct DPC : DockingPaneContainer {
    static void setContainerName(DockingPaneContainer* c, const QString& n)
    {
        reinterpret_cast<DPC*>(c)->setName(n);
    }

    static void setContainerId(DockingPaneContainer* c, const QString& i)
    {
        reinterpret_cast<DPC*>(c)->setId(i);
    }
};

inline QString toQString(const String& s)
{
    return QString::fromUtf8(s.data(), static_cast<int>(s.size()));
}

using itype = DockingPaneContainer;

struct DockPanel::Impl : public UIElementData {
    DockFeatures features = DockFeatures::None;
    String       title;
    String       id;
    UIElement*   content = nullptr;
};

DockPanel::DockPanel()
  : Control(new Impl(), nullptr)
{}

DockPanel::~DockPanel()
{
    // UIElement::~UIElement() deletes d (Impl)
}

inline auto DockPanel::dptr() -> Impl*
{
    return static_cast<Impl*>(UIElement::d);
}

inline auto DockPanel::dptr() const -> const Impl*
{
    return static_cast<const Impl*>(UIElement::d);
}

bool DockPanel::onClosing()
{
    return true; // default: allow close
}

void DockPanel::setFeatures(DockFeatures f)
{
    dptr()->features = f;
    auto* c          = impl<itype>();
    if (c)
        c->setClosable(testFlag(f, DockFeatures::Closable));
}

DockFeatures DockPanel::features() const
{
    return dptr()->features;
}

void DockPanel::setTitle(const String& t)
{
    dptr()->title = t;
    auto* c       = impl<itype>();
    if (c)
        DPC::setContainerName(c, toQString(t));
}

String DockPanel::title() const
{
    return dptr()->title;
}

void DockPanel::setId(const String& i)
{
    dptr()->id = i;
    auto* c    = impl<itype>();
    if (c)
        DPC::setContainerId(c, toQString(i));
}

String DockPanel::id() const
{
    return dptr()->id;
}

void DockPanel::setContent(UIElement* c)
{
    dptr()->content = c;
    auto* p         = impl<itype>();
    if (!p)
        return;

    auto* newWidget = c ? static_cast<QWidget*>(c->impl()) : nullptr;

    // The docking library requires a non-null widget in the pane layout;
    // passing nullptr to setClientWidget() would assert/crash. Treat
    // setContent(nullptr) as "keep the current content".
    if (!newWidget)
        return;

    auto* oldClient = p->clientWidget();
    p->setClientWidget(newWidget);
    if (oldClient && oldClient != newWidget)
        oldClient->deleteLater();
}

raw_ptr<UIElement> DockPanel::content() const
{
    return dptr()->content;
}

void DockPanel::attach(UIObject* container)
{
    if (!container)
        return;

    auto* dpc          = static_cast<DockingPaneContainer*>(container);
    UIElement::d->impl = container;
    setOwnsImpl(true); // DockingPanes owns the container lifetime

    // Sync previously-set title and id
    if (!dptr()->title.empty())
        DPC::setContainerName(dpc, toQString(dptr()->title));
    if (!dptr()->id.empty())
        DPC::setContainerId(dpc, toQString(dptr()->id));

    // Install close callback via lambda — captures 'this' directly, no trampoline needed
    dpc->setCloseCallback([](DockingPaneContainer* c) -> bool {
        auto* panel = static_cast<DockPanel*>(c->userData());
        return panel ? panel->onClosing() : true;
    });
}

bool DockPanel::isFloating() const
{
    auto* c = impl<itype>();
    return c && c->state() == DockingPaneBase::Floating;
}

bool DockPanel::isPinned() const
{
    auto* c = impl<itype>();
    return c && c->state() == DockingPaneBase::Pinned;
}

bool DockPanel::isCollapsed() const
{
    auto* c = impl<itype>();
    return c && c->state() == DockingPaneBase::Hidden;
}

bool DockPanel::isTabbed() const
{
    auto* c = impl<itype>();
    return c && c->state() == DockingPaneBase::Tabbed;
}

DockAreas DockPanel::dockArea() const
{
    auto* c = impl<itype>();
    if (!c)
        return DockAreas::None;
    auto state = c->state();
    switch (state) {
    case DockingPaneBase::Docked:
    case DockingPaneBase::Tabbed:
    {
        // Derive the current area dynamically so it stays correct even after
        // the pane was moved by dragging (the stored _vine_dockarea property
        // is only refreshed by addDockPanel()).
        auto* mgr = c->dockingManager();
        if (mgr) {
            switch (mgr->dockPositionOf(c)) {
            case DockingPaneManager::dockLeft: return DockAreas::Left;
            case DockingPaneManager::dockRight: return DockAreas::Right;
            case DockingPaneManager::dockTop: return DockAreas::Top;
            case DockingPaneManager::dockBottom: return DockAreas::Bottom;
            default: break;
            }
        }
        // Fall back to the remembered area if the position cannot be derived.
        auto var = c->property("_vine_dockarea");
        if (var.isValid())
            return static_cast<DockAreas>(var.toInt());
        return DockAreas::None;
    }
    case DockingPaneBase::Floating: return DockAreas::None;
    default: return DockAreas::None;
    }
}

void DockPanel::setFloating(bool floating)
{
    auto* c = impl<itype>();
    if (!c)
        return;

    if (floating) {
        if (c->state() != DockingPaneBase::Floating) {
            // Really float: detach from the dock tree (which frees the space
            // for the remaining panes) and keep the current position.
            // floatPane(QPoint) records the global position before detaching
            // and restores it afterwards; a manual closePane() would leave the
            // pane at its old parent-relative coordinates.
            c->floatPane(QPoint(0, 0));
        }
    }
    else if (c->state() == DockingPaneBase::Floating) {
        auto* mgr = c->dockingManager();
        if (!mgr)
            return;

        // Dock back to the frame, using the remembered dock area.
        DockingPaneManager::DockPosition pos = DockingPaneManager::dockRight;

        const QVariant areaVar = c->property("_vine_dockarea");
        if (areaVar.isValid()) {
            switch (static_cast<DockAreas>(areaVar.toInt())) {
            case DockAreas::Top: pos = DockingPaneManager::dockTop; break;
            case DockAreas::Bottom: pos = DockingPaneManager::dockBottom; break;
            case DockAreas::Left: pos = DockingPaneManager::dockLeft; break;
            default: pos = DockingPaneManager::dockRight; break;
            }
        }

        mgr->dockPane(c, pos, nullptr);
    }
}

void DockPanel::pin()
{
    auto* c = impl<itype>();
    if (!c)
        return;

    if (c->state() != DockingPaneBase::Pinned) {
        // Real auto-hide: pull the pane out of the dock tree and create the
        // strip button (same path as the title-bar pin button).
        if (auto* mgr = c->dockingManager())
            mgr->hidePane(c);
    }
}

void DockPanel::unpin()
{
    auto* c = impl<itype>();
    if (!c)
        return;

    if (c->state() == DockingPaneBase::Pinned) {
        if (auto* mgr = c->dockingManager())
            mgr->unpinPane(c);
    }
}

void DockPanel::collapse()
{
    auto* c = impl<itype>();
    if (!c)
        return;

    if (c->state() == DockingPaneBase::Docked || c->state() == DockingPaneBase::Tabbed) {
        if (auto* mgr = c->dockingManager()) {
            mgr->closePane(c);
            // closePane() detaches a docked pane but only hides floating
            // panes; make sure a collapsed pane is invisible until restore().
            c->hide();
        }
    }
}

void DockPanel::restore()
{
    auto* c = impl<itype>();
    if (!c)
        return;

    if (c->state() != DockingPaneBase::Hidden)
        return;

    auto* mgr = c->dockingManager();
    if (!mgr)
        return;

    // Dock back to the remembered area instead of floating the pane at an
    // arbitrary position (the library's showPane(Hidden) would float it).
    DockingPaneManager::DockPosition pos = DockingPaneManager::dockLeft;

    const QVariant areaVar = c->property("_vine_dockarea");
    if (areaVar.isValid()) {
        switch (static_cast<DockAreas>(areaVar.toInt())) {
        case DockAreas::Right: pos = DockingPaneManager::dockRight; break;
        case DockAreas::Top: pos = DockingPaneManager::dockTop; break;
        case DockAreas::Bottom: pos = DockingPaneManager::dockBottom; break;
        default: pos = DockingPaneManager::dockLeft; break;
        }
    }

    mgr->dockPane(c, pos, nullptr);
    c->show();
}

V_APPFWGUI_NS_END
