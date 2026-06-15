#pragma once

#include "UIElement.hpp"
#include "Gui.hpp"

V_APPFWGUI_NS_BEGIN

class DockPanelManager; // forward-declare for friend access

class V_APPFW_API DockPanel : public UIElement {
    V_OBJECT_META_DECL

    friend class DockPanelManager;

  public:
    DockPanel();
    virtual ~DockPanel();

    void         setFeatures(DockFeatures features);
    DockFeatures getFeatures() const;

    void   setTitle(const String& title);
    String getTitle() const;

    void   setId(const String& id);
    String getId() const;

    void       setContent(UIElement* content);
    UIElement* getContent() const;

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
