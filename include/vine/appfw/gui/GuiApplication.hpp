#pragma once
#include "gui_global.hpp"

#include <vine/appfw/Application.hpp>
#include <vine/core/Object.hpp>


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