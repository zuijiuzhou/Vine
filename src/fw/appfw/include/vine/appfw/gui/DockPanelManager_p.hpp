#include <vine/appfw/gui/DockPanelManager.hpp>

#include <DockingPaneManager.h>

V_APPFWGUI_NS_BEGIN

class DockPanelManagerPrivate : public vine::RefObjectPrivate {
    V_DECLARE_PUBLIC(DockPanelManager)

  protected:
    DockPanelManagerPrivate();
    ~DockPanelManagerPrivate();

  protected:
    DockingPaneManager* impl_;
};

V_APPFWGUI_NS_END
