#pragma once
#include "gui_global.h"

#include "core/Inherit.h"
#include "appfw/Application.h"


VINE_APPFWGUI_BEGIN

class VINE_APPFWGUI_API GuiApplication : public Inherit<Application, GuiApplication>
{

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