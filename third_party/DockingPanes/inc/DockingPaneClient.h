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

#ifndef DOCKINGPANECLIENT_H
#define DOCKINGPANECLIENT_H

#include "DockingPaneBase.h"

class QGridLayout;

class DockingPaneManager;

/**
 * @brief Central client-area container (the root/centre node of the docking tree).
 *
 * Holds the central content set by the host via setClientWidget; all docked panes
 * are arranged around it. During layout restore this node is the "contains client"
 * anchor (updateAllSplitters / applyLayout).
 *
 * @note It is a leaf of the docking tree with no title bar; it always exists and
 * should never be closed.
 */
class DockingPaneClient : public DockingPaneBase {
    Q_OBJECT

    friend class DockingPaneManager;

  public:
    explicit DockingPaneClient(QWidget* parent = nullptr);

  public:
    void saveLayout(QDomNode* parentNode, bool includeGeometry = false) override;

  private:
    void setWidget(QWidget* widget);
};

#endif // DOCKINGPANECLIENT_H
