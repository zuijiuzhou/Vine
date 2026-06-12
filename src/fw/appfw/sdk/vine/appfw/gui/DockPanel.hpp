#pragma once

#include "UIElement.hpp"
#include "Gui.hpp"

V_APPFWGUI_NS_BEGIN

class V_APPFW_API DockPanel : public UIElement {
    V_OBJECT_META_DECL

  public:
    DockPanel();
    virtual ~DockPanel();

    void      setAllowedAreas(DockAreas areas);
    DockAreas getAllowedAreas() const;

    void         setFeatures(DockFeatures features);
    DockFeatures getFeatures() const;

    void   setTitle(const String& title);
    String getTitle() const;

    void   setId(const String& id);
    String getId() const;

    void    setContent(UIElement* content);
    UIElement* getContent() const;

    // Attach an existing DockingPanes container (opaque QObject pointer)
    void attach(UIObject* container);

    // State queries
    bool isFloating() const;
    bool isPinned() const;
    bool isCollapsed() const;
    bool isTabbed() const;
    DockAreas dockArea() const;

    // State control
    void setFloating(bool floating);
    void pin();
    void unpin();
    void collapse();
    void restore();

  private:
    struct Data;
    Data*       dptr();
    const Data* dptr() const;
};

V_APPFWGUI_NS_END
