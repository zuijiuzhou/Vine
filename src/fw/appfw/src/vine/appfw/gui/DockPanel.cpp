#include <vine/appfw/gui/DockPanel.hpp>

#include <vine/appfw/gui/Convert.hpp>
#include "../../../../third_party/DockingPanes/src/DockingPaneContainer.h"

V_APPFWGUI_NS_BEGIN

V_OBJECT_META_IMPL(DockPanel, UIElement)

struct DockPanel::Data {
    DockingPaneContainer* container = nullptr;
    DockAreas             allowed   = DockAreas::None;
    DockFeatures          features  = DockFeatures::None;
    String                title;
};

DockPanel::DockPanel()
  : UIElement(new DockingPaneContainer())
  , d(new Data)
{
    d->container = impl<DockingPaneContainer>();
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
    // DockingPanes controls behaviour via state; feature mapping omitted
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

void DockPanel::setContent(UIElement* content)
{
    if (!content) {
        d->container->setClientWidget(nullptr);
    } else {
        d->container->setClientWidget(static_cast<QWidget*>(content->impl()));
    }
}

UIElement* DockPanel::getContent() const
{
    QWidget* w = d->container->clientWidget();
    if (!w)
        return nullptr;
    // cannot recover wrapper for client widget here
    return nullptr;
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

DockAreas DockPanel::dockArea() const
{
    return d->allowed;
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
