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
 * @brief Palette-driven theme colour helpers (a set of inline functions in a namespace).
 *
 * All colours are derived from QApplication::palette(), so the pane colours automatically
 * follow the theme/dark-mode changes without hardcoding light/dark colours.
 * The affected widgets repaint on receiving QEvent::PaletteChange / ApplicationPaletteChange.
 *
 * @note Colour semantics:
 *  - Borders (border/floatingBorder) are derived from the Window lightness (darker on light
 *    themes, lighter on dark themes).
 *  - Title-bar background: Base when active (blends with the content), Window when inactive.
 *  - Title text/icons use WindowText; hover/indicators use Highlight.
 */
namespace DockingPaneTheme
{

/**
 * @brief The current application palette.
 */
inline QPalette appPalette()
{
    return QApplication::palette();
}

/**
 * @brief 1px pane border colour (Window slightly darker on light themes, slightly lighter on dark themes).
 */
inline QColor borderColor()
{
    const QColor window = appPalette().color(QPalette::Window);

    return window.lightness() < 128 ? window.lighter(135) : window.darker(115);
}

/**
 * @brief Floating pane border colour.
 */
inline QColor floatingBorderColor()
{
    const QColor window = appPalette().color(QPalette::Window);

    return window.lightness() < 128 ? window.lighter(160) : window.darker(150);
}

/**
 * @brief Active title-bar background (uses Base to blend with the panel content).
 */
inline QColor activeHeaderColor()
{
    return appPalette().color(QPalette::Base);
}

/**
 * @brief Inactive title-bar background (uses Window).
 */
inline QColor inactiveHeaderColor()
{
    return appPalette().color(QPalette::Window);
}

/**
 * @brief Title text colour (used with the matching title-bar background).
 */
inline QColor titleTextColor(bool)
{
    return appPalette().color(QPalette::WindowText);
}

/**
 * @brief Title-bar dotted pattern colour.
 */
inline QColor titlePatternColor(bool)
{
    return appPalette().color(QPalette::Mid);
}

/**
 * @brief Auto-hide button strip background colour (same as the border colour).
 */
inline QColor autoHideStripColor()
{
    return borderColor();
}

/**
 * @brief Auto-hide button hover colour (Highlight).
 */
inline QColor autoHideHoverColor()
{
    return appPalette().color(QPalette::Highlight);
}

/**
 * @brief Close/pin icon tint (paired with the matching title-bar background).
 */
inline QColor iconTintColor(bool)
{
    return appPalette().color(QPalette::WindowText);
}

} // namespace DockingPaneTheme
