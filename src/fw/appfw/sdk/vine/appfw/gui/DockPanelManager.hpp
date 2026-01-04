#pragma once

#include <vine/RefObject.hpp>
#include <vine/appfw/appfw_global.hpp>

VI_APPFWGUI_NS_BEGIN

VI_DECLARE_PIMPL_CLASS(DockPanelManager)

class VI_APPFW_API DockPanelManager : public RefObject {
    VI_OBJECT_META
    VI_DECLARE_PRIVATE(DockPanelManager)

  public:
    DockPanelManager();
};

VI_APPFWGUI_NS_END