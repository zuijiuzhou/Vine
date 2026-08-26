#pragma once

#include "../ApplicationData.hpp"

#include <vine/appfw/gui/GuiApplication.hpp>

class QApplication;

V_APPFWGUI_NS_BEGIN

struct GuiApplicationData : public ApplicationData {
    QApplication* app = nullptr;

    Theme theme         = Theme::Light; // currently active theme
    bool  follow_system = true;         // whether to follow the system theme
};

V_APPFWGUI_NS_END
