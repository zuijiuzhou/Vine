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
namespace DockingPaneTheme {

inline QPalette appPalette(void)
{
    return QApplication::palette();
}

/* 1px pane border: slightly darker than the window colour in light themes,
 * slightly lighter in dark themes. */
inline QColor borderColor(void)
{
    const QColor window = appPalette().color(QPalette::Window);

    return window.lightness() < 128 ? window.lighter(135) : window.darker(115);
}

/* Floating pane border */
inline QColor floatingBorderColor(void)
{
    const QColor window = appPalette().color(QPalette::Window);

    return window.lightness() < 128 ? window.lighter(160) : window.darker(150);
}

/* Pane header background: the highlight role is the accent colour, the
 * window role is the neutral background. Both pair with the text roles
 * below in every theme. */
inline QColor activeHeaderColor(void)
{
    return appPalette().color(QPalette::Highlight);
}

inline QColor inactiveHeaderColor(void)
{
    return appPalette().color(QPalette::Window);
}

/* Title bar text, always paired with the matching header background */
inline QColor titleTextColor(bool active)
{
    return appPalette().color(active ? QPalette::HighlightedText : QPalette::WindowText);
}

/* Title bar dotted pattern */
inline QColor titlePatternColor(bool active)
{
    if (active) {
        const QColor highlight = appPalette().color(QPalette::Highlight);

        return highlight.lightness() < 128 ? highlight.lighter(150) : highlight.lighter(115);
    }

    return appPalette().color(QPalette::Mid);
}

/* Auto-hide button strip */
inline QColor autoHideStripColor(void)
{
    return borderColor();
}

inline QColor autoHideHoverColor(void)
{
    return appPalette().color(QPalette::Highlight);
}

/* Close/pin button icon tint. Active bitmaps are white, inactive ones are
 * black; recolour them with the palette text roles so they stay visible on
 * every header background. */
inline QColor iconTintColor(bool active)
{
    return appPalette().color(active ? QPalette::HighlightedText : QPalette::WindowText);
}

} // namespace DockingPaneTheme
