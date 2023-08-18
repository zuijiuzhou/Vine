#include <appfw/gui/StatusBar.h>

VINE_APPFWGUI_NS_BEGIN

VI_OBJECT_META_IMPL(StatusBar, UIElement)

struct StatusBar::Data
{

};

StatusBar::StatusBar()
    : d(new Data())
{
}

VINE_APPFWGUI_NS_END