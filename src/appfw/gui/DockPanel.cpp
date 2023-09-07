#include <appfw/gui/DockPanel.h>

VI_APPFWGUI_NS_BEGIN
VI_OBJECT_META_IMPL(DockPanel, UIElement)

struct DockPanel::Data
{
};

DockPanel::DockPanel()
    : d(new Data())
{
}

VI_APPFWGUI_NS_END