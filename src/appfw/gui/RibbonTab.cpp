#include <appfw/gui/RibbonTab.h>
#include <SARibbon.h>

VINE_APPFWGUI_NS_BEGIN

VI_OBJECT_META_IMPL(RibbonTab, UIElement)

struct RibbonTab::Data
{
    
};

RibbonTab::RibbonTab(void* impl)
    : Control(impl)
    , d(new Data())
{

}

VINE_APPFWGUI_NS_END