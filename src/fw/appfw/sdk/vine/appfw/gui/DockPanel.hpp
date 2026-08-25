#pragma once

#include "Control.hpp"
#include "Gui.hpp"

V_APPFWGUI_NS_BEGIN

class DockPanelManager; // forward-declare for friend access

class V_APPFW_API DockPanel : public Control {
    V_OBJECT_META_DECL

    friend class DockPanelManager;

  public:
    DockPanel();
    virtual ~DockPanel();

    void         features(DockFeatures f);
    DockFeatures features() const;

    void   title(const String& t);
    String title() const;

    void   id(const String& i);
    String id() const;

    void       content(UIElement* c);
    UIElement* content() const;

    // State queries
    bool      isFloating() const;
    bool      isPinned() const;
    bool      isCollapsed() const;
    bool      isTabbed() const;
    DockAreas dockArea() const;

    // State control
    void setFloating(bool floating);
    void pin();
    void unpin();
    void collapse();
    void restore();

  protected:
    /// Override to intercept close. Return false to veto.
    virtual bool onClosing();

  private:
    void attach(UIObject* container); // only DockPanelManager may call

    struct Data;
    Data*       dptr();
    const Data* dptr() const;
};

V_APPFWGUI_NS_END
