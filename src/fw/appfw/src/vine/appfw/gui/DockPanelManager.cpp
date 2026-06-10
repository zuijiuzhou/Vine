#include <vine/appfw/gui/DockPanelManager.hpp>
#include <vine/appfw/gui/DockPanel.hpp>

#include <algorithm>

V_APPFWGUI_NS_BEGIN

struct DockPanelManager::Data {
  std::vector<DockPanel*> panels;
};

DockPanelManager::DockPanelManager()
  : d(new Data)
{
}

DockPanelManager::~DockPanelManager()
{
  for (auto p : d->panels) {
    delete p;
  }
  delete d;
}

DockPanel* DockPanelManager::createDockPanel()
{
  auto p = new DockPanel();
  d->panels.push_back(p);
  return p;
}

void DockPanelManager::addDockPanel(DockPanel* panel)
{
  if (!panel)
    return;
  if (std::find(d->panels.begin(), d->panels.end(), panel) == d->panels.end())
    d->panels.push_back(panel);
}

void DockPanelManager::removeDockPanel(DockPanel* panel)
{
  if (!panel)
    return;
  auto it = std::find(d->panels.begin(), d->panels.end(), panel);
  if (it != d->panels.end()) {
    d->panels.erase(it);
    delete panel;
  }
}

DockPanel* DockPanelManager::findByTitle(const String& title) const
{
  for (auto p : d->panels) {
    if (p && p->getTitle() == title)
      return p;
  }
  return nullptr;
}

std::vector<DockPanel*> DockPanelManager::panels() const
{
  return d->panels;
}

V_APPFWGUI_NS_END
