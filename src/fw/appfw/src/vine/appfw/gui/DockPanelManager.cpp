#include <vine/appfw/gui/DockPanelManager.hpp>

#include <vine/appfw/gui/DockPanelManager_p.hpp>

V_APPFWGUI_NS_BEGIN

V_OBJECT_META_IMPL(DockPanelManager, RefObject)

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

V_APPFWGUI_NS_END
