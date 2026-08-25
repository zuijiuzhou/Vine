#include <vine/appfw/gui/DockPanel.hpp>

#include <DockingPaneContainer.h>
#include <DockingPaneManager.h>

#include "Convert.hpp"
#include "UIElementData.hpp"

V_APPFWGUI_NS_BEGIN

V_OBJECT_META_IMPL(DockPanel, UIElement)

// Helper to access protected members of DockingPaneContainer.
struct DPC : DockingPaneContainer {
    static void setContainerName(DockingPaneContainer* c, const QString& n)
    { reinterpret_cast<DPC*>(c)->setName(n); }

    static void setContainerId(DockingPaneContainer* c, const QString& i)
    { reinterpret_cast<DPC*>(c)->setId(i); }
};

inline QString toQString(const String& s)
{ return QString::fromUtf8(s.data(), static_cast<int>(s.size())); }

using itype = DockingPaneContainer;

struct DockPanel::Data : public UIElementData {
    DockFeatures features = DockFeatures::None;
    String       title;
    String       id;
    UIElement*   content  = nullptr;
};

inline auto DockPanel::dptr() -> Data*
{ return static_cast<Data*>(UIElement::d); }

inline auto DockPanel::dptr() const -> const Data*
{ return static_cast<const Data*>(UIElement::d); }

DockPanel::DockPanel()
  : UIElement(new Data(), nullptr)
{}

DockPanel::~DockPanel()
{
    // UIElement::~UIElement() deletes d (Data)
}

// ---- Close interception ----

bool DockPanel::onClosing()
{
    return true; // default: allow close
}

// ---- Features ----

void DockPanel::setFeatures(DockFeatures features)
{
    dptr()->features = features;
    auto* c = impl<itype>();
    if (c)
        c->setClosable(testFlag(features, DockFeatures::Closable));
}

DockFeatures DockPanel::getFeatures() const
{ return dptr()->features; }

// ---- Title / Id ----

void DockPanel::setTitle(const String& title)
{
    dptr()->title = title;
    auto* c = impl<itype>();
    if (c)
        DPC::setContainerName(c, toQString(title));
}

String DockPanel::getTitle() const
{ return dptr()->title; }

void DockPanel::setId(const String& id)
{
    dptr()->id = id;
    auto* c = impl<itype>();
    if (c)
        DPC::setContainerId(c, toQString(id));
}

String DockPanel::getId() const
{ return dptr()->id; }

// ---- Content ----

void DockPanel::setContent(UIElement* content)
{
    dptr()->content = content;
    auto* c = impl<itype>();
    if (!c) return;

    auto* newWidget = content ? static_cast<QWidget*>(content->impl()) : nullptr;

    // The docking library requires a non-null widget in the pane layout;
    // passing nullptr to setClientWidget() would assert/crash. Treat
    // setContent(nullptr) as "keep the current content".
    if (!newWidget)
        return;

    auto* oldClient = c->clientWidget();
    c->setClientWidget(newWidget);
    if (oldClient && oldClient != newWidget)
        oldClient->deleteLater();
}

UIElement* DockPanel::getContent() const
{ return dptr()->content; }

// ---- Attach ----

void DockPanel::attach(UIObject* container)
{
    if (!container) return;

    auto* dpc = static_cast<DockingPaneContainer*>(container);
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

// ---- State queries ----

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
    if (!c) return DockAreas::None;
    auto state = c->state();
    switch (state) {
    case DockingPaneBase::Docked:
    case DockingPaneBase::Tabbed: {
        auto var = c->property("_vine_dockarea");
        if (var.isValid())
            return static_cast<DockAreas>(var.toInt());
        return DockAreas::Left;
    }
    case DockingPaneBase::Floating: return DockAreas::None;
    default: return DockAreas::None;
    }
}

// ---- State control ----

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
            case DockAreas::Top:    pos = DockingPaneManager::dockTop;    break;
            case DockAreas::Bottom: pos = DockingPaneManager::dockBottom; break;
            case DockAreas::Left:   pos = DockingPaneManager::dockLeft;   break;
            default:                pos = DockingPaneManager::dockRight;  break;
            }
        }

        mgr->dockPane(c, pos, nullptr);
    }
}

void DockPanel::pin()
{
    auto* c = impl<itype>();
    if (!c) return;

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
    if (!c) return;

    if (c->state() == DockingPaneBase::Pinned) {
        if (auto* mgr = c->dockingManager())
            mgr->unpinPane(c);
    }
}

void DockPanel::collapse()
{
    auto* c = impl<itype>();
    if (!c) return;

    if (c->state() == DockingPaneBase::Docked || c->state() == DockingPaneBase::Tabbed) {
        if (auto* mgr = c->dockingManager())
            mgr->closePane(c);
    }
}

void DockPanel::restore()
{
    auto* c = impl<itype>();
    if (!c) return;

    if (c->state() == DockingPaneBase::Hidden) {
        if (auto* mgr = c->dockingManager())
            mgr->showPane(c);
    }
}

V_APPFWGUI_NS_END
