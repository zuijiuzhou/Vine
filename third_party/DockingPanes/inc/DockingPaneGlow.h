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

#ifndef DOCKINGPANEGLOW_H
#define DOCKINGPANEGLOW_H

#include <QObject>

class DockingPaneGlowWidget;

/**
 * \brief 浮动窗格边缘缩放光晕的管理者。
 *
 * 为某个浮动窗格创建四个 DockingPaneGlowWidget（左/右/上/下），
 * 用于拖动窗格边缘调整大小。窗格移动/尺寸变化时调用 update() 重排。
 *
 * \note 生命周期：归所属窗格管理（DockingPaneContainer::m_floatingGlow），
 * 由 setState()/floatPane() 释放；窗格被销毁而未走这些路径时会泄漏。
 */
class DockingPaneGlow : public QObject
{
    Q_OBJECT

    public:
        /**
         * \brief 构造。
         * \param floatingPane 要加光晕的浮动窗格。
         * \param parent       光晕窗口的父窗口（通常为主窗口）。
         */
        explicit DockingPaneGlow(QWidget* floatingPane, QObject* parent = nullptr);
        ~DockingPaneGlow();

        /**
         * \brief 把四个光晕窗口置顶。
         */
        void raise(void);

        /**
         * \brief 按浮动窗格当前位置/尺寸重排四个光晕窗口。
         */
        void update(void);

    private:
        void onGlowResized(void);

        DockingPaneGlowWidget* m_leftGlow;
        DockingPaneGlowWidget* m_rightGlow;
        DockingPaneGlowWidget* m_topGlow;
        DockingPaneGlowWidget* m_bottomGlow;
};

#endif // DOCKINGPANEGLOW_H
