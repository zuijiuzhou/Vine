#pragma once

#include "Control.h"
#include "core/Object.h"
#include "gui_global.h"

VI_APPFWGUI_NS_BEGIN

class VI_APPFWGUI_API DockPanel : public Control {
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

    void     setContent(Control* content);
    Control* getContent() const;

  public:
  private:
    VI_OBJECT_DATA;
};

VI_APPFWGUI_NS_END