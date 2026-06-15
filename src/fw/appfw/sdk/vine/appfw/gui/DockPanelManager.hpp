#pragma once

#include <vine/appfw/appfw_global.hpp>
#include <vine/Ptr.hpp>

#include <vector>

#include "Gui.hpp"

V_APPFWGUI_NS_BEGIN

class DockPanel;
class MainWindow;
class UIElement;

/**
 * @brief Manages the lifecycle and organization of DockPanel instances.
 *
 * Owns an internal DockingPaneManager and provides factory methods to create
 * dock panels, tracks all managed panels, and supports lookup by id or title.
 */
class V_APPFW_API DockPanelManager final {

  public:
    DockPanelManager();
    ~DockPanelManager();

    /** Set the target window and initialize the docking manager.
     *  Must be called once before any docking operation. */
    void setWindow(UIElement* wnd);

    /** Set the central working-area widget (replaces the default placeholder) */
    void setCentralWidget(UIElement* widget);

    /** Get the root widget to embed in a window's central area */
    UIElement* root() const;

    // ---- Factory ----

    /** Create and dock a DockPanel with title and content at the given area. */
    DockPanel* createDockPanel(const String& title, DockAreas area);

    /** Create and dock a DockPanel with title, content widget, and area. */
    DockPanel* createDockPanel(const String& title, UIElement* content, DockAreas area);

    // ---- Registration ----

    /** Add and dock a DockPanel at the given area (area is always required). */
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
