#pragma once

#include "core/Inherit.h"
#include "appfw/Application.h"

#include "gui_global.h"

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
VINE_APPFWGUI_NS_END