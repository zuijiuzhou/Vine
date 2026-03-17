#pragma once

#include <vine/RefObject.hpp>
#include <vine/appfw/appfw_global.hpp>

V_APPFWGUI_NS_BEGIN

V_DECLARE_PIMPL(DockPanelManager)

class V_APPFW_API DockPanelManager : public RefObject {
    V_OBJECT_META_DECL
    V_DECLARE_PRIVATE(DockPanelManager)

  public:
    DockPanelManager();
};

V_APPFWGUI_NS_END
