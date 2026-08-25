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
 * \brief 停靠预览高亮（半透明蓝色矩形）。
 *
 * 拖动浮动窗格命中某停靠指示时，显示目标区域（窗格的一侧或停靠区边缘）
 * 的预览框。颜色取调色板 Highlight（随主题变化），透明度 0.5。
 */
class DockingTargetWidget : public QWidget
{
    Q_OBJECT
    public:
        explicit DockingTargetWidget(QWidget* parent = nullptr);
};

#endif // DOCKINGTARGETWIDGET_H
