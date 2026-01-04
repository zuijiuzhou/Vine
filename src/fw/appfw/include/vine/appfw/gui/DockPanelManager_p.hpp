#include <vine/appfw/gui/DockPanelManager.hpp>

#include <DockingPaneManager.h>

VI_APPFWGUI_NS_BEGIN

class DockPanelManagerPrivate : public vine::RefObjectPrivate {
    VI_DECLARE_PUBLIC(DockPanelManager)

  protected:
    DockPanelManagerPrivate();
    ~DockPanelManagerPrivate();

  protected:
    DockingPaneManager* impl_;
};

VI_APPFWGUI_NS_END