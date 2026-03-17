#pragma once

#include <vine/RefObject.hpp>

#include "Widget.hpp"

V_APPFWGUI_NS_BEGIN

class V_APPFW_API DockPanel : public Widget {
    V_OBJECT_META_DECL;

  public:
  public:
    DockPanel();
    virtual ~DockPanel();

  public:
    void      setAllowedAreas(DockAreas areas);
    DockAreas getAllowedAreas() const;

    void         setFeatures(DockFeatures features);
    DockFeatures getFeatures() const;

    void   setTitle(const String& title);
    String getTitle() const;

    void    setContent(Widget* content);
    Widget* getContent() const;

  public:
  private:
    struct Data;
    Data* const d;
};

V_APPFWGUI_NS_END
