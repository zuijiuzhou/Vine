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

#ifndef DOCKINGTARGETWIDGET_H
#define DOCKINGTARGETWIDGET_H

#include <QWidget>

/**
 * @brief Docking preview highlight (a semi-transparent rectangle).
 *
 * When a dragged floating pane hits a docking indicator, shows a preview box of the target
 * area (a side of the pane or the docking-area edge). The colour comes from the palette's
 * Highlight (follows the theme) with an opacity of 0.5.
 */
class DockingTargetWidget : public QWidget {
    Q_OBJECT
  public:
    explicit DockingTargetWidget(QWidget* parent = nullptr);
};

#endif // DOCKINGTARGETWIDGET_H
