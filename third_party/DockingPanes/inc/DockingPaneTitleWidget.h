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

#ifndef DOCKINGPANETITLEWIDGET_H
#define DOCKINGPANETITLEWIDGET_H

#include <QWidget>

class QPoint;

class DockingPaneTitleWidget : public QWidget
{
    Q_OBJECT
    public:
        explicit DockingPaneTitleWidget(QString text = QString(), QWidget *parent = nullptr);
        ~DockingPaneTitleWidget() override;

        void setText(QString text);
        void setActive(bool active);

        // Re-acquire the mouse grab after the owning window was recreated
        // (e.g. when a docked pane is floated mid-drag).
        void reacquireGrab(void);

        // Take over the drag: grab the mouse and enter dragging state, used
        // when the flyout is hidden mid-drag and this title keeps the drag.
        void takeGrab(void);

    Q_SIGNALS:
        void titleBarStartMove(QPoint pos);
        void titleBarEndMove(QPoint pos);
        void titleBarMoved(QPoint pos);

    protected:
        virtual void paintEvent(QPaintEvent* event) override;
        virtual void changeEvent(QEvent* event) override;
        virtual void resizeEvent(QResizeEvent* event) override;
        virtual void mousePressEvent(QMouseEvent* event) override;
        virtual void mouseMoveEvent(QMouseEvent* event) override;
        virtual void mouseReleaseEvent(QMouseEvent* event) override;

    private:
        void drawPattern(QPainter *p, int x, int y, int w, int h);
        void onFocusChanged(QWidget *old,QWidget *now);
        QString m_text;
        bool m_active;
        bool m_grabbing = false;
};

#endif // DOCKINGPANETITLEWIDGET_H
