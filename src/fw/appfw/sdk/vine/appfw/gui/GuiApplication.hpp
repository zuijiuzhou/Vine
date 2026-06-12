#pragma once

#include <vine/appfw/Application.hpp>

V_APPFWGUI_NS_BEGIN

class V_APPFW_API GuiApplication : public Application {
    V_OBJECT_META_DECL
  public:
    GuiApplication(int argc, char** argv);
    virtual ~GuiApplication();

  public:
    virtual void init() override;

  public:
    virtual int run() override;
};

V_APPFWGUI_NS_END
