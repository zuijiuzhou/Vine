#include <vine/appfw/gui/RibbonTab.hpp>

#include <SARibbon.h>
#include <vine/appfw/gui/RibbonGroup.hpp>

V_APPFWGUI_NS_BEGIN

V_OBJECT_META_IMPL(RibbonTab, UIElement)

struct RibbonTab::Data {
    SARibbonCategory* cat = nullptr;
    String            title;
};

RibbonTab::RibbonTab()
  : UIElement(new SARibbonCategory(QString()))
  , d(new Data)
{
    d->cat = impl<SARibbonCategory>();
}

RibbonTab::~RibbonTab()
{
    delete d;
}

void RibbonTab::title(const String& t)
{
    d->title = t;
}

String RibbonTab::title() const
{
    return d->title;
}

void RibbonTab::addGroup(RibbonGroup* g)
{
    if (!g)
        return;
    // SARibbonCategory::addPanel expects SARibbonPanel*
    auto p = g->impl<SARibbonPanel>();
    if (p)
        d->cat->addPanel(p);
}

void RibbonTab::removeGroup(RibbonGroup* g)
{
    if (!g)
        return;
    auto p = g->impl<SARibbonPanel>();
    if (p)
        d->cat->removePanel(p);
}

V_APPFWGUI_NS_END
