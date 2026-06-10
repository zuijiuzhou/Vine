#pragma once

#include <vine/appfw/appfw_global.hpp>
#include <vector>
#include <vine/Ptr.hpp>

V_APPFWGUI_NS_BEGIN

class DockPanel;

class V_APPFW_API DockPanelManager {

  public:
    DockPanelManager();
    ~DockPanelManager();

    DockPanel* createDockPanel();
    void addDockPanel(DockPanel* panel);
    void removeDockPanel(DockPanel* panel);
    DockPanel* findByTitle(const String& title) const;
    std::vector<DockPanel*> panels() const;

  private:
    struct Data;
    Data* const d;
};

V_APPFWGUI_NS_END
