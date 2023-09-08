#include <appfw/gui/DockPanel.h>

VI_APPFWGUI_NS_BEGIN
VI_OBJECT_META_IMPL(DockPanel, Control)

struct DockPanel::Data
{
};

DockPanel::DockPanel()
    : Control(nullptr)
    , d(new Data())
{
    
}

DockPanel::~DockPanel(){
    delete d;
}

VI_APPFWGUI_NS_END