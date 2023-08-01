#pragma once

#include "core/Inherit.h"
#include "appfw/Application.h"

#include "appfw_gui_export.h"

ETD_APPFW_GUI_BEGIN

class ETD_APPFW_GUI_API GuiApplication : public Inherit<Application, GuiApplication>
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
ETD_APPFW_GUI_NS_END