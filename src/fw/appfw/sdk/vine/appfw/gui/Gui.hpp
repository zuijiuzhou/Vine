#pragma once

#include <vine/appfw/appfw_global.hpp>

#include <vine/math/Point2.hpp>
#include <vine/math/Rect2.hpp>
#include <vine/math/Vector2.hpp>

V_APPFWGUI_NS_BEGIN

using Rect  = math::Rect2i;
using Point = math::Point2i;
using Size  = math::Vec2i;

enum class DockAreas
{
    None   = 0,
    Left   = 1,
    Top    = 2,
    Right  = 4,
    Bottom = 8
};
V_ENABLE_ENUM_FLAGS(DockAreas);

enum class DockFeatures
{
    None      = 0,
    Closable  = 1,
    All       = Closable
};
V_ENABLE_ENUM_FLAGS(DockFeatures);

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

/**
 * \brief 功能区按钮显示样式（图标与文字的排布）。
 */
enum class RibbonButtonStyle
{
    IconOnly,        ///< 只显示图标
    TextOnly,        ///< 只显示文字
    TextBesideIcon,  ///< 文字在图标右侧
    TextUnderIcon,   ///< 文字在图标下方
};

/**
 * \brief 功能区条目尺寸（对应 SARibbon 面板行占比 RowProportion）。
 *
 * 供 RibbonButton 的尺寸，以及 RibbonGroup::addControl 添加任意控件的行占比使用。
 * \note SARibbon 按钮类型只有 Large/Small 两档；Medium 仅作为面板行占比
 * 生效（三行模式下两个 Medium 并排占两行），按钮本身按小按钮渲染。
 */
enum class RibbonItemSize
{
    Large,   ///< 大：占整行，图标在上、文字在下
    Medium,  ///< 中等：三行模式下并排占两行（面板需为三行模式）
    Small,   ///< 小：占一行（普通工具条按钮）
};

/**
 * \brief 功能区面板布局模式（对应 SARibbonPanel::PanelLayoutMode）。
 */
enum class RibbonPanelLayoutMode
{
    ThreeRow,  ///< 三行模式（Office 风格，默认）
    TwoRow,    ///< 两行模式（WPS 紧凑风格）
    SingleRow, ///< 单行模式：所有按钮等效 Small，标题默认隐藏
};

/**
 * \brief 功能区全局风格：行数 × 宽松/紧凑（对应 SARibbonBar::RibbonStyles）。
 */
enum class RibbonStyle
{
    ThreeRowLoose,    ///< 三行、宽松
    ThreeRowCompact,  ///< 三行、紧凑
    TwoRowLoose,      ///< 两行、宽松
    TwoRowCompact,    ///< 两行、紧凑（WPS 风格）
    SingleRowLoose,   ///< 单行、宽松
    SingleRowCompact, ///< 单行、紧凑
};

V_APPFWGUI_NS_END
