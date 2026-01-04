#include <vine/appfw/gui/DockPanelManager.hpp>

#include <vine/appfw/gui/DockPanelManager_p.hpp>

VI_APPFWGUI_NS_BEGIN

VI_OBJECT_META_IMPL(DockPanelManager, RefObject)

DockPanelManager::DockPanelManager()
  : vine::RefObject(new DockPanelManagerPrivate())
{
	
}

VI_APPFWGUI_NS_END