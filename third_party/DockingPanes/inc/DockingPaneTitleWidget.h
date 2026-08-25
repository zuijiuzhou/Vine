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

/**
 * \brief 窗格标题栏：绘制标题文字 + 右侧点状纹理，并负责拖动。
 *
 * 按下时 grabMouse() 抓取鼠标以在整个拖动期间持续收到事件（含光标压到
 * 顶层停靠指示器上时）。通过三个信号把拖动事件上抛给容器。
 *
 * \note 拖动机制注意事项：
 *  - 停靠窗格拖过阈值后 floatPane() 会重建原生窗口，抓取随之丢失，需要
 *    在重建后调用 reacquireGrab() 重新抓取。
 *  - flyout 拖出转浮动时 flyout 隐藏会释放抓取，用 takeGrab() 由本标题接管。
 *  - 鼠标释放（release）只对左键处理，且用 m_grabbing 记录抓取状态避免
 *    重复 releaseMouse()。
 */
class DockingPaneTitleWidget : public QWidget
{
    Q_OBJECT
    public:
        /**
         * \brief 构造标题栏。
         * \param text 初始标题文本。
         */
        explicit DockingPaneTitleWidget(QString text = QString(), QWidget *parent = nullptr);
        ~DockingPaneTitleWidget() override;

        /**
         * \brief 设置标题文本。
         */
        void setText(QString text);

        /**
         * \brief 设置激活态（影响前景/纹理颜色）。
         */
        void setActive(bool active);

        /**
         * \brief 重新抓取鼠标（停靠窗格转浮动重建窗口后调用）。
         * \note 仅在仍处于抓取状态（m_grabbing）时重新 grabMouse()。
         */
        void reacquireGrab(void);

        /**
         * \brief 接管拖动：抓取鼠标并进入抓取状态。
         *        用于 flyout 中途隐藏、由本标题继续拖动。
         */
        void takeGrab(void);

    Q_SIGNALS:
        void titleBarStartMove(QPoint pos);  ///< 按下标题（全局坐标）。
        void titleBarEndMove(QPoint pos);    ///< 松开标题（全局坐标）。
        void titleBarMoved(QPoint pos);      ///< 拖动移动（全局坐标）。

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
        bool m_grabbing = false;   ///< 当前是否持有鼠标抓取。
};

#endif // DOCKINGPANETITLEWIDGET_H
