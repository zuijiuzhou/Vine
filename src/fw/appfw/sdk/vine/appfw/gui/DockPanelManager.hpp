#pragma once

#include <vine/appfw/appfw_global.hpp>
#include <vine/Ptr.hpp>

#include <vector>

#include "Gui.hpp"

V_APPFWGUI_NS_BEGIN

class DockPanel;
class MainWindow;

/**
 * @brief Manages the lifecycle and organization of DockPanel instances.
 *
 * Owns an internal DockingPaneManager and provides factory methods to create
 * dock panels, tracks all managed panels, and supports lookup by id or title.
 */
class V_APPFW_API DockPanelManager {

  public:
    DockPanelManager();
    ~DockPanelManager();

    /** Attach this manager to a MainWindow (must be called once before docking) */
    void attachToWindow(MainWindow* wnd);

    // ---- Factory ----

    /** Create a new DockPanel and add it to the managed list */
    DockPanel* createDockPanel();

    /** Create a new DockPanel and dock it at the given area */
    DockPanel* createDockPanel(DockAreas area);

    // ---- Registration ----

    /** Add an externally-created DockPanel to the managed list */
    void addDockPanel(DockPanel* panel);

    /** Add and dock a DockPanel at the given area */
    void addDockPanel(DockPanel* panel, DockAreas area);

    /** Remove and delete a DockPanel from the managed list */
    void removeDockPanel(DockPanel* panel);

    // ---- Queries ----

    /** Find a DockPanel by its unique id */
    DockPanel* findById(const String& id) const;

    /** Find a DockPanel by its display title */
    DockPanel* findByTitle(const String& title) const;

    /** Get the number of managed dock panels */
    int count() const;

    /** Get all managed dock panels */
    std::vector<DockPanel*> panels() const;

  private:
    struct Data;
    Data* const d;
};

V_APPFWGUI_NS_END
