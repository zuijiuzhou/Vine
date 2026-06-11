#include <vine/appfw/gui/RibbonGroup.hpp>

#include <SARibbon.h>
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
    if (!b)
        return;
    auto w = b->impl<QWidget>();
    if (w) {
        // Detach the widget from its parent panel
        w->setParent(nullptr);
    }
}

V_APPFWGUI_NS_END
