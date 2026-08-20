#pragma once

#include "../ApplicationData.hpp"

#include <vine/appfw/gui/GuiApplication.hpp>

class QApplication;

V_APPFWGUI_NS_BEGIN

struct GuiApplicationData : public ApplicationData {
    QApplication* app = nullptr;

    Theme theme         = Theme::Light; // 当前生效主题
    bool  follow_system = true;         // 是否跟随系统主题
};

V_APPFWGUI_NS_END
