#include <vine/appfw/gui/DockPanel.hpp>

#include <DockingPaneContainer.h>

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
    auto* oldClient = c->clientWidget();
    if (!content) { c->setClientWidget(nullptr); }
    else { c->setClientWidget(static_cast<QWidget*>(content->impl())); }
    if (oldClient && oldClient != static_cast<QWidget*>(content ? content->impl() : nullptr))
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
    if (c)
        c->setState(floating ? DockingPaneBase::Floating : DockingPaneBase::Docked);
}

void DockPanel::pin()
{
    auto* c = impl<itype>();
    if (c) c->setState(DockingPaneBase::Pinned);
}

void DockPanel::unpin()
{
    auto* c = impl<itype>();
    if (c) c->setState(DockingPaneBase::Docked);
}

void DockPanel::collapse()
{
    auto* c = impl<itype>();
    if (c) c->setState(DockingPaneBase::Hidden);
}

void DockPanel::restore()
{
    auto* c = impl<itype>();
    if (c) c->setState(DockingPaneBase::Docked);
}

V_APPFWGUI_NS_END
