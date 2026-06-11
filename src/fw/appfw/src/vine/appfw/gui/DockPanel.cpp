#include <vine/appfw/gui/DockPanel.hpp>

#include <vine/appfw/gui/Convert.hpp>
#include <DockingPaneContainer.h>
#include <DockingPaneManager.h>

V_APPFWGUI_NS_BEGIN

V_OBJECT_META_IMPL(DockPanel, UIElement)

struct DockPanel::Data {
    DockingPaneContainer* container    = nullptr;
    DockAreas             allowed      = DockAreas::None;
    DockFeatures          features     = DockFeatures::None;
    String                title;
    String                id;
    UIElement*            content      = nullptr;
};

DockPanel::DockPanel()
  : UIElement(static_cast<QObject*>(nullptr))
  , d(new Data)
{
    d->container = nullptr;
}

DockPanel::DockPanel(UIObject* container)
    : UIElement(container)
    , d(new Data)
{
        d->container = static_cast<DockingPaneContainer*>(container);
}

DockPanel::~DockPanel()
{
    delete d;
}

void DockPanel::setAllowedAreas(DockAreas areas)
{
    d->allowed = areas;
}

DockAreas DockPanel::getAllowedAreas() const
{
    return d->allowed;
}

void DockPanel::setFeatures(DockFeatures features)
{
    d->features = features;
    if (!d->container)
        return;
    // Map features to DockingPaneContainer state:
    // Closable -> allow close button
    // Floatable -> allow floating (Float is a state)
    // Movable -> allow drag to move (default in DockingPanes)
    if (!testFlag(features, DockFeatures::Closable)) {
        // DockingPanes doesn't directly support hiding close button
        // without subclassing; feature stored for reference
    }
    if (!testFlag(features, DockFeatures::Floatable)) {
        // Prevent floating if not allowed:
        // only allow Docked/Hidden/Pinned states if floating disabled
        if (d->container->state() == DockingPaneBase::Floating)
            d->container->setState(DockingPaneBase::Docked);
    }
}

DockFeatures DockPanel::getFeatures() const
{
    return d->features;
}

void DockPanel::setTitle(const String& title)
{
    d->title = title;
}

String DockPanel::getTitle() const
{
    return d->title;
}

void DockPanel::setId(const String& id)
{
    d->id = id;
}

String DockPanel::getId() const
{
    return d->id;
}

void DockPanel::setContent(UIElement* content)
{
    d->content = content;
    if (!content) {
        d->container->setClientWidget(nullptr);
    } else {
        d->container->setClientWidget(static_cast<QWidget*>(content->impl()));
    }
}

UIElement* DockPanel::getContent() const
{
    return d->content;
}

void DockPanel::attach(UIObject* container)
{
    if (!container)
        return;
    d->container = static_cast<DockingPaneContainer*>(container);
    d->allowed = DockAreas::None;
}

bool DockPanel::isFloating() const
{
    return d->container->state() == DockingPaneBase::Floating;
}

bool DockPanel::isPinned() const
{
    return d->container->state() == DockingPaneBase::Pinned;
}

bool DockPanel::isCollapsed() const
{
    return d->container->state() == DockingPaneBase::Hidden;
}

bool DockPanel::isTabbed() const
{
    return d->container->state() == DockingPaneBase::Tabbed;
}

DockAreas DockPanel::dockArea() const
{
    if (!d->container)
        return DockAreas::None;
    auto state = d->container->state();
    switch (state) {
        case DockingPaneBase::Docked:
        case DockingPaneBase::Tabbed:
            // Docked/Tabbed — report the last known allowed area
            return d->allowed != DockAreas::None ? d->allowed : DockAreas::Left;
        case DockingPaneBase::Floating:
            return DockAreas::None;
        default:
            return DockAreas::None;
    }
}

void DockPanel::setFloating(bool floating)
{
    if (floating)
        d->container->setState(DockingPaneBase::Floating);
    else
        d->container->setState(DockingPaneBase::Docked);
}

void DockPanel::pin()
{
    d->container->setState(DockingPaneBase::Pinned);
}

void DockPanel::unpin()
{
    d->container->setState(DockingPaneBase::Docked);
}

void DockPanel::collapse()
{
    d->container->setState(DockingPaneBase::Hidden);
}

void DockPanel::restore()
{
    d->container->setState(DockingPaneBase::Docked);
}

V_APPFWGUI_NS_END
