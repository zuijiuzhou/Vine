#pragma once

#include <vine/RefObject.hpp>
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

  private:
    struct Data;
    Data* const d;
};

using GuiApplicationPtr = RefPtr<GuiApplication>;

V_APPFWGUI_NS_END
