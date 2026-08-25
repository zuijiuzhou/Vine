/*
 * This file is part of DockingPanes. (https://github.com/KestrelRadarSensors/dockingpanes)
 *
 * (C) 2020 Kestrel Radar Sensors (https://www.kestrelradarsensors.com)
 *
 * DockingPanes is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DockingPanes is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with DockingPanes.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <QApplication>
#include <QColor>
#include <QPalette>

/*
 * Palette-driven colour helpers.
 *
 * All colours are derived from the application palette, so the panes follow
 * any palette change (Windows 11 dark mode, Fusion dark palette, custom
 * palettes set by the host application, ...) automatically. No hardcoded
 * light/dark colour pairs.
 */

/**
 * \brief 调色板驱动的主题颜色辅助（命名空间内联函数集合）。
 *
 * 所有颜色从 QApplication::palette() 派生，因此窗格颜色自动跟随主题/
 * 深色模式变化，无需硬编码亮/暗色。相关控件在收到
 * QEvent::PaletteChange / ApplicationPaletteChange 后 update() 重绘。
 *
 * \note 颜色语义约定：
 *  - 边框（border/floatingBorder）由 Window 亮度派生（亮主题加深、暗主题加亮）。
 *  - 标题栏背景：激活用 Base（与内容融合）、非激活用 Window。
 *  - 标题文字/图标用 WindowText；悬停/指示用 Highlight。
 */
namespace DockingPaneTheme {

/**
 * \brief 当前应用调色板。
 */
inline QPalette appPalette(void)
{
    return QApplication::palette();
}

/**
 * \brief 1px 窗格边框色（亮主题略深、暗主题略亮的 Window）。
 */
inline QColor borderColor(void)
{
    const QColor window = appPalette().color(QPalette::Window);

    return window.lightness() < 128 ? window.lighter(135) : window.darker(115);
}

/**
 * \brief 浮动窗格边框色。
 */
inline QColor floatingBorderColor(void)
{
    const QColor window = appPalette().color(QPalette::Window);

    return window.lightness() < 128 ? window.lighter(160) : window.darker(150);
}

/**
 * \brief 激活标题栏背景（用 Base 与面板内容融合）。
 */
inline QColor activeHeaderColor(void)
{
    return appPalette().color(QPalette::Base);
}

/**
 * \brief 非激活标题栏背景（用 Window）。
 */
inline QColor inactiveHeaderColor(void)
{
    return appPalette().color(QPalette::Window);
}

/**
 * \brief 标题文字色（与对应标题栏背景配对使用）。
 */
inline QColor titleTextColor(bool)
{
    return appPalette().color(QPalette::WindowText);
}

/**
 * \brief 标题栏点状纹理色。
 */
inline QColor titlePatternColor(bool)
{
    return appPalette().color(QPalette::Mid);
}

/**
 * \brief 自动隐藏按钮条底色（同边框色）。
 */
inline QColor autoHideStripColor(void)
{
    return borderColor();
}

/**
 * \brief 自动隐藏按钮悬停色（Highlight）。
 */
inline QColor autoHideHoverColor(void)
{
    return appPalette().color(QPalette::Highlight);
}

/**
 * \brief 关闭/固定图标着色（与对应标题栏背景配对）。
 */
inline QColor iconTintColor(bool)
{
    return appPalette().color(QPalette::WindowText);
}

} // namespace DockingPaneTheme
