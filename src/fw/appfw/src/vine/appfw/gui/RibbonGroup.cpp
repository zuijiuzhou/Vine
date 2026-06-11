#include <vine/appfw/gui/RibbonGroup.hpp>

#include <SARibbon.h>
#include <QToolButton>
#include <vine/appfw/gui/RibbonButton.hpp>

V_APPFWGUI_NS_BEGIN

V_OBJECT_META_IMPL(RibbonGroup, UIElement)

struct RibbonGroup::Data {
        SARibbonPanel* pannel = nullptr;
    String         title;
};

RibbonGroup::RibbonGroup()
    : UIElement(new SARibbonPanel())
  , d(new Data)
{
        d->pannel = impl<SARibbonPanel>();
}

RibbonGroup::~RibbonGroup()
{
    delete d;
}

void RibbonGroup::title(const String& t)
{
    d->title = t;
    if (d->pannel) {
        auto utf16 = t.toUtf16();
        d->pannel->setPanelName(QString::fromUtf16(reinterpret_cast<const ushort*>(utf16.data()), (int)utf16.size()));
    }
}

String RibbonGroup::title() const
{
    return d->title;
}

void RibbonGroup::addButton(RibbonButton* b)
{
    if (!b)
        return;
    auto w = b->impl<QWidget>();
    if (w && d->pannel)
        d->pannel->addWidget(w, SARibbonPanelItem::Small);
}

void RibbonGroup::removeButton(RibbonButton* b)
{
    if (!b || !d->pannel)
        return;
    auto* w = b->impl<QWidget>();
    if (w) {
        // Remove the action from the panel's layout
        // SARibbonPanel manages buttons through QActions; find and remove
        auto* action = w->findChild<QAction*>(QString(), Qt::FindDirectChildrenOnly);
        if (!action) {
            // Try to get the default action if it's a tool button
            auto* tb = qobject_cast<QToolButton*>(w);
            if (tb)
                action = tb->defaultAction();
        }
        if (action) {
            d->pannel->removeAction(action);
        }
        w->setParent(nullptr);
        w->deleteLater();
    }
}

V_APPFWGUI_NS_END
