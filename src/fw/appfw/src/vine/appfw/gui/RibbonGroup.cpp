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
    (void)w; // TODO: integrate with SARibbonPanel API
}

void RibbonGroup::removeButton(RibbonButton* b)
{
    // SARibbonPanel doesn't expose removeWidget in public API reliably; keep no-op
}

V_APPFWGUI_NS_END
