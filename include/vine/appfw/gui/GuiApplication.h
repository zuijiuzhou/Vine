#pragma once
#include "gui_global.h"

#include <vine/appfw/Application.h>
#include <vine/core/Object.h>


VI_APPFWGUI_NS_BEGIN

class VI_APPFWGUI_API GuiApplication : public Application {
    VI_OBJECT_META
  public:
    GuiApplication(int argc, char** argv);
    virtual ~GuiApplication();

  public:
    virtual void init() override;

  public:
    virtual int run() override;

  private:
    VI_OBJECT_DATA
};

using GuiApplicationPtr = RefPtr<GuiApplication>;

VI_APPFWGUI_NS_END