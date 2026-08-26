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

#ifndef DOCKINGFRAMEFRAMESTICKER_H
#define DOCKINGFRAMEFRAMESTICKER_H

#include <QWidget>

/**
 * \brief 停靠指示器的四角帧指示小窗口（可停靠到停靠区边缘）。
 *
 * 独立的 Qt::ToolTip 置顶透明窗口，随光标进入/离开切换激活/非激活位图。
 * 由 DockingFrameStickers 统一摆放与驱动。
 */
class DockingFrameFrameSticker : public QWidget {
    Q_OBJECT

  public:
    /**
     * \brief 构造。
     * \param image 位图前缀（如 "frame_left"，实际加载 _active/_inactive 两张）。
     * \param parent
     */
    explicit DockingFrameFrameSticker(const QString& image, QWidget* parent = nullptr);

    /**
     * \brief 更新激活态（光标是否落在本指示上）。
     */
    void updateCursorPos(QPoint pos);

  protected:
    void paintEvent(QPaintEvent* event) override;

  private:
    QImage m_activeImage;
    QImage m_inactiveImage;

    bool m_isActive;
};

#endif // DOCKINGFRAMEFRAMESTICKER_H
