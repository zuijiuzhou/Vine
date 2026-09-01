#pragma once

#include <vine/raw_ptr.hpp>

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

    void         setFeatures(DockFeatures f);
    DockFeatures features() const;

    void   setTitle(const String& t);
    String title() const;

    void   setId(const String& i);
    String id() const;

    void       setContent(UIElement* c);
    raw_ptr<UIElement> content() const;

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

    struct Impl;
    Impl*       dptr();
    const Impl* dptr() const;
};

V_APPFWGUI_NS_END
