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

#ifndef DOCKINGPANESPLITTERCONTAINER_H
#define DOCKINGPANESPLITTERCONTAINER_H

#include "DockingPaneBase.h"

class QSplitter;

class DockingPaneManager;

/**
 * \brief 停靠树中的分割器节点（包装 QSplitter）。
 *
 * 停靠时按方向创建分割器，把邻居与停靠窗格并排。含两个子节点时是二叉树形态的停靠树中间节点；不含客户端分支的子分割器会优先收缩（updateAllSplitters）。
 *
 * \note 分割方向与停靠方位对应：splitVertical=上下堆叠，splitHorizontal=左右并排（命名与直觉相反，注意区分）。
 */
class DockingPaneSplitterContainer : public DockingPaneBase {
    Q_OBJECT

    friend class DockingPaneManager;

  public:
    enum SplitterDirection
    {
        splitVertical,  // QSplitter 垂直方向（子窗格上下堆叠）。
        splitHorizontal // QSplitter 水平方向（子窗格左右并排）。
    };

  public:
    explicit DockingPaneSplitterContainer(QWidget* parent = nullptr, SplitterDirection direction = splitVertical);
    ~DockingPaneSplitterContainer() override;

  public:
    /**
     * \brief 当前分割方向。
     */
    SplitterDirection direction();

    void saveLayout(QDomNode* parentNode, bool includeGeometry = false) override;

  private:
    QSplitter* m_splitterWidget; ///< 实际的分割控件。
};

#endif // DOCKINGPANESPLITTERCONTAINER_H
