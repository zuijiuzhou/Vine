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
 * @brief Small corner frame indicator window of the docking indicators (can dock
 * to the edge of the docking area).
 *
 * A standalone Qt::ToolTip topmost transparent window that switches between the
 * active/inactive bitmaps as the cursor enters/leaves. Positioned and driven by
 * DockingFrameStickers.
 */
class DockingFrameFrameSticker : public QWidget {
    Q_OBJECT

  public:
    /**
     * @brief Constructs a frame sticker.
     * @param image Bitmap prefix (e.g. "frame_left"; loads the _active/_inactive variants).
     * @param parent
     */
    explicit DockingFrameFrameSticker(const QString& image, QWidget* parent = nullptr);

    /**
     * @brief Updates the active state (whether the cursor is over this sticker).
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
