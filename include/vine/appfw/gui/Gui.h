#pragma once

#include <vine/core/core_defs.h>
#include <vine/ge/Point2i.h>
#include <vine/ge/Rect2i.h>
#include <vine/ge/Vector2i.h>

#include "gui_global.h"


VI_APPFWGUI_NS_BEGIN

using Rect  = ge::Rect2i;
using Point = ge::Point2i;
using Size  = ge::Vector2i;

enum class DockAreas
{
    None   = 0,
    Left   = 1,
    Top    = 2,
    Right  = 4,
    Bottom = 8,
    All    = Left | Top | Right | Bottom,
};
VI_ENUM_CLASS_FLAGS(DockAreas);

enum class DockFeatures
{
    None      = 0,
    Closable  = 1,
    Movable   = 2,
    Floatable = 4,
    All       = Closable | Movable | Floatable
};
VI_ENUM_CLASS_FLAGS(DockFeatures);

enum class StartupPosition
{
    Manual,
    CenterScreen
};

enum class WindowState
{
    Normal,
    Minimized,
    Maximized,
};

VI_APPFWGUI_NS_END