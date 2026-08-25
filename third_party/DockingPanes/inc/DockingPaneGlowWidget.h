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
 * \brief 浮动窗格边缘的缩放手柄（几乎透明的窄条）。
 *
 * 9px 宽/高的边缘窗口，光标进入显示相应缩放光标；按下拖动即可调整
 * 浮动窗格尺寸（上/下边还支持左右拖动实现四角缩放）。
 *
 * \note 窗口透明度 0.01，视觉上几乎不可见，仅作为命中区使用；
 * 依赖 QCursor::pos()，Wayland 下失效。
 */
class DockingPaneGlowWidget : public QWidget
{
    Q_OBJECT

    public:
        enum Position
        {
            Left,
            Right,
            Top,
            Bottom
        };

        explicit DockingPaneGlowWidget(QWidget* floatingPane, Position pos, QWidget* parent = nullptr);

        /**
         * \brief 按浮动窗格当前位置/尺寸重排本光晕窗口。
         */
        void updatePosition(void);

    Q_SIGNALS:
        void glowResized();   ///< 尺寸被拖动改变（通知 DockingPaneGlow 重排其它边）。

    protected:
        virtual void paintEvent(QPaintEvent* event) override;
        virtual void mousePressEvent(QMouseEvent* event) override;
        virtual void mouseMoveEvent(QMouseEvent* event) override;
        virtual void mouseReleaseEvent(QMouseEvent* event) override;
        virtual void enterEvent(QEnterEvent* event) override;
        virtual void leaveEvent(QEvent* event) override;

    private:
        void updateCursor();

        QWidget* m_floatingPane;   ///< 被缩放的浮动窗格。
        QRect m_paneGeometry;      ///< 按下时窗格几何（用于限制最小尺寸）。
        QPoint m_Pos;              ///< 上一次全局光标位置。
        int m_position;            ///< 本手柄的 Position。
        int m_cursorDelta;         ///< 按下时相对手柄左缘的偏移。
        int m_cornerState;         ///< 上/下边：0=中部 1=左角 2=右角。
        bool m_dragging;           ///< 是否正在拖动。
};

#endif // DOCKINGPANEGLOWWIDGET_H
