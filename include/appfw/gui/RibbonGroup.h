#pragma once
#include "gui_global.h"

#include "Control.h"

VI_APPFWGUI_NS_BEGIN

class RibbonTab;

class VI_APPFWGUI_API RibbonGroup : public Control {
    VI_OBJECT_META

    friend class RibbonTab;

  public:
    RibbonGroup();
    virtual ~RibbonGroup();

  public:
    String title() const;
    void   title(const String& ti);

  private:
    VI_OBJECT_DATA
};

VI_APPFWGUI_NS_END
