#pragma once
#include "gui_global.h"

#include "core/Object.h"
#include "appfw/Application.h"


VINE_APPFWGUI_NS_BEGIN

class VINE_APPFWGUI_API GuiApplication : public Application
{
    VI_OBJECT_META
public:
    GuiApplication(int argc, char** argv);
    virtual ~GuiApplication();

public:
    virtual void init() override;

public:
    virtual int run() override;

private:
    struct Data;
    Data *d;
};

using GuiApplicationPtr = RefPtr<GuiApplication>;

VINE_APPFWGUI_NS_END