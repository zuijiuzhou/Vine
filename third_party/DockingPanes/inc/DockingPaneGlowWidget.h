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

#ifndef DOCKINGPANEGLOWWIDGET_H
#define DOCKINGPANEGLOWWIDGET_H

#include <QWidget>

/**
 * @brief Resize handle on the edge of a floating pane (a nearly transparent thin strip).
 *
 * A 9px wide/tall edge window that shows the appropriate resize cursor on hover; pressing and
 * dragging resizes the floating pane (the top/bottom edges also support horizontal dragging
 * for corner resizing).
 *
 * @note Window opacity is 0.01, so it is nearly invisible and serves purely as a hit area;
 * relies on QCursor::pos(), which fails under Wayland.
 */
class DockingPaneGlowWidget : public QWidget {
    Q_OBJECT

  public:
    enum Position
    {
        Left,
        Right,
        Top,
        Bottom
    };

  public:
    explicit DockingPaneGlowWidget(QWidget* floatingPane, Position pos, QWidget* parent = nullptr);

  public:
    /**
     * @brief Repositions this glow window according to the floating pane's current position/size.
     */
    void updatePosition();

  Q_SIGNALS:
    // The size was changed by dragging (notifies DockingPaneGlow to rearrange the other edges).
    void glowResized();

  protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

  private:
    void updateCursor();

  private:
    QWidget* m_floatingPane; // The floating pane being resized.
    QRect    m_paneGeometry; // Pane geometry at press time (used to enforce the minimum size).
    QPoint   m_Pos;          // Last global cursor position.
    int      m_position;     // This handle's Position.
    int      m_cursorDelta;  // Offset relative to the handle's left edge at press time.
    int      m_cornerState;  // Top/bottom edge: 0=centre 1=left corner 2=right corner.
    bool     m_dragging;     // Whether a drag is in progress.
};

#endif // DOCKINGPANEGLOWWIDGET_H
