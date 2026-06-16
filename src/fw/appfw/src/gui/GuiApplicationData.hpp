#pragma once

#include "../ApplicationData.hpp"

class QApplication;

V_APPFWGUI_NS_BEGIN

struct GuiApplicationData : public ApplicationData {
    QApplication* app = nullptr;
};

V_APPFWGUI_NS_END
