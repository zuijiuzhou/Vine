#pragma once

#include "UIElement.hpp"
#include "Gui.hpp"

V_APPFWGUI_NS_BEGIN

class DockPanel;

/**
 * @brief Represents a docking area zone in the main window.
 *
 * A DockArea is a container that holds dock panels arranged in a specific
 * dock position (Left, Right, Top, Bottom, or Center/Client).
 */
class V_APPFW_API DockArea : public UIElement {
    V_OBJECT_META_DECL

  public:
    explicit DockArea(DockAreas position = DockAreas::Left);
    virtual ~DockArea();

    /** Get the position of this dock area */
    DockAreas position() const;

    /** Number of dock panels in this area */
    int dockPanelCount() const;

    /** Get dock panel at index */
    DockPanel* dockPanelAt(int index) const;

    /** Add a dock panel to this area */
    void addDockPanel(DockPanel* panel);

    /** Remove a dock panel from this area */
    void removeDockPanel(DockPanel* panel);

    /** Check if a dock panel is in this area */
    bool containsDockPanel(DockPanel* panel) const;

    /** Check if this area is empty */
    bool isEmpty() const;

  private:
    struct Data;
    Data*       dptr();
    const Data* dptr() const;
};

V_APPFWGUI_NS_END
