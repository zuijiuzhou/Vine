#include <vine/appfw/gui/DockPanelManager.hpp>

#include <vine/appfw/gui/DockPanelManager_p.hpp>

VI_APPFWGUI_NS_BEGIN

VI_OBJECT_META_IMPL(DockPanelManager, RefObject)

DockPanelManagerPrivate::DockPanelManagerPrivate()
  : impl_(new DockingPaneManager())
{}

DockPanelManagerPrivate::~DockPanelManagerPrivate()
{
    delete impl_;
}

DockPanelManager::DockPanelManager()
  : vine::RefObject(new DockPanelManagerPrivate())
{}

VI_APPFWGUI_NS_END