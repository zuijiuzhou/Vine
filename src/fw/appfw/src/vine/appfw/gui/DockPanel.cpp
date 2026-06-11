#include <vine/appfw/gui/DockPanel.hpp>

#include <vine/appfw/gui/Convert.hpp>
#include <DockingPaneContainer.h>
#include <DockingPaneManager.h>

V_APPFWGUI_NS_BEGIN

V_OBJECT_META_IMPL(DockPanel, UIElement)

namespace
{

// Helper to access protected setName / setId on DockingPaneContainer.
// DockingPaneContainer exposes no public API to change the title after creation,
// but we need to sync title/id set after attach. This is a well-known C++ idiom.
struct DPC : DockingPaneContainer {
    static void setContainerName(DockingPaneContainer* c, const QString& n) {
        reinterpret_cast<DPC*>(c)->setName(n);
    }
    static void setContainerId(DockingPaneContainer* c, const QString& i) {
        reinterpret_cast<DPC*>(c)->setId(i);
    }
};

inline QString toQString(const String& s)
{
    return QString::fromUtf8(s.data(), static_cast<int>(s.size()));
}

} // namespace

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

    d->container->setClosable(testFlag(features, DockFeatures::Closable));
    d->container->setMovable(testFlag(features, DockFeatures::Movable));
    d->container->setFloatable(testFlag(features, DockFeatures::Floatable));
}

DockFeatures DockPanel::getFeatures() const
{
    return d->features;
}

void DockPanel::setTitle(const String& title)
{
    d->title = title;
    if (d->container) {
        DPC::setContainerName(d->container, toQString(title));
    }
}

String DockPanel::getTitle() const
{
    return d->title;
}

void DockPanel::setId(const String& id)
{
    d->id = id;
    if (d->container) {
        DPC::setContainerId(d->container, toQString(id));
    }
}

String DockPanel::getId() const
{
    return d->id;
}

void DockPanel::setContent(UIElement* content)
{
    d->content = content;
    if (!d->container) return;
    // Delete the old client widget if it was a placeholder (not managed by user)
    auto* oldClient = d->container->clientWidget();
    if (!content) {
        d->container->setClientWidget(nullptr);
    } else {
        d->container->setClientWidget(static_cast<QWidget*>(content->impl()));
    }
    if (oldClient && oldClient != static_cast<QWidget*>(content ? content->impl() : nullptr)) {
        oldClient->deleteLater();
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

    // Sync previously-set title and id to the container
    if (!d->title.empty()) {
        DPC::setContainerName(d->container, toQString(d->title));
    }
    if (!d->id.empty()) {
        DPC::setContainerId(d->container, toQString(d->id));
    }
}

bool DockPanel::isFloating() const
{
    return d->container && d->container->state() == DockingPaneBase::Floating;
}

bool DockPanel::isPinned() const
{
    return d->container && d->container->state() == DockingPaneBase::Pinned;
}

bool DockPanel::isCollapsed() const
{
    return d->container && d->container->state() == DockingPaneBase::Hidden;
}

bool DockPanel::isTabbed() const
{
    return d->container && d->container->state() == DockingPaneBase::Tabbed;
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
    if (!d->container) return;
    if (floating)
        d->container->setState(DockingPaneBase::Floating);
    else
        d->container->setState(DockingPaneBase::Docked);
}

void DockPanel::pin()
{
    if (!d->container) return;
    d->container->setState(DockingPaneBase::Pinned);
}

void DockPanel::unpin()
{
    if (!d->container) return;
    d->container->setState(DockingPaneBase::Docked);
}

void DockPanel::collapse()
{
    if (!d->container) return;
    d->container->setState(DockingPaneBase::Hidden);
}

void DockPanel::restore()
{
    if (!d->container) return;
    d->container->setState(DockingPaneBase::Docked);
}

V_APPFWGUI_NS_END
