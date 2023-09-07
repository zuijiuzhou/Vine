#include <appfw/gui/RibbonTab.h>
#include <SARibbon.h>

VI_APPFWGUI_NS_BEGIN

VI_OBJECT_META_IMPL(RibbonTab, UIElement)

struct RibbonTab::Data
{
    
};

RibbonTab::RibbonTab(void* impl)
    : Control(impl)
    , d(new Data())
{

}

VI_APPFWGUI_NS_END