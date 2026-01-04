#pragma once

#include <vine/RefObject.hpp>

#include "Widget.hpp"

VI_APPFWGUI_NS_BEGIN

class VI_APPFW_API DockPanel : public Widget {
    VI_OBJECT_META;

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

    void     setContent(Widget* content);
    Widget* getContent() const;

  public:
  private:
    VI_OBJECT_DATA;
};

VI_APPFWGUI_NS_END