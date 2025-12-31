#pragma once

#include <vine/appfw/appfw_global.hpp>

#include <vine/math/Point2.hpp>
#include <vine/math/Rect2.hpp>
#include <vine/math/Vector2.hpp>

VI_APPFWGUI_NS_BEGIN

using Rect  = math::Rect2i;
using Point = math::Point2i;
using Size  = math::Vec2i;

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