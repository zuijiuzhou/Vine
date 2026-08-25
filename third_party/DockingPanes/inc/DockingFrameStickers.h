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

#ifndef DOCKINGFRAMECENTRESTICKER_H
#define DOCKINGFRAMECENTRESTICKER_H

#include <QMap>
#include <QWidget>

class QPaintEvent;

class DockingFrameFrameSticker;

/**
 * \brief 停靠指示器组（中心 + 四边图标 + 四角帧指示）。
 *
 * 拖动浮动窗格时显示在当前候选窗格中央：中心/四边指示停靠到该窗格
 * 的哪一侧，四角帧指示停靠到整个停靠区（frame）边缘。命中检测由
 * getHit() 完成，驱动 DockingPaneManager 决定停靠位置。
 *
 * \note 实现细节：
 *  - 是 Qt::ToolTip 置顶无边框窗口，带 WA_TranslucentBackground。
 *  - m_rcCentre 为遗留死代码：中心区域实际由 m_rcTab（Tab 图标）命中，
 *    tab 不可见时中心不可停靠。
 *  - m_rcBottom / frame 角用 Top/Left 的尺寸定位，属 copy-paste 遗留；
 *    当前各图标尺寸一致所以无可见问题。
 */
class DockingFrameStickers : public QWidget
{
    Q_OBJECT

    public:
        /// 命中结果：停靠到窗格某侧 / 停靠区边缘 / Tab。
        enum DockingPosition
        {
            paneLeft,     ///< 停靠到窗格左侧。
            paneRight,    ///< 停靠到窗格右侧。
            paneTop,      ///< 停靠到窗格上侧。
            paneBottom,   ///< 停靠到窗格下侧。
            frameLeft,    ///< 停靠到停靠区左边缘。
            frameRight,   ///< 停靠到停靠区右边缘。
            frameTop,     ///< 停靠到停靠区上边缘。
            frameBottom,  ///< 停靠到停靠区下边缘。
            tab           ///< 并入标签组。
        };

    public:
        explicit DockingFrameStickers(QWidget *parent = nullptr);

        /**
         * \brief 设置停靠区（主窗口中央区）矩形，用于摆放四角帧指示。
         */
        void setFrameRect(QRect rect);

        /**
         * \brief 拖动移动时更新各指示的激活态（光标所在者高亮）。
         */
        void updateCursorPos(QPoint pos);

        /**
         * \brief 是否显示中心 Tab 图标（候选窗格是普通窗格时显示）。
         */
        void setTabVisible(bool state);

        /**
         * \brief 命中检测。
         * \param pos     全局光标位置。
         * \param dockPos 输出命中方位。
         * \return 是否命中任一指示。
         */
        bool getHit(QPoint pos, DockingPosition *dockPos);

    protected:
        virtual void paintEvent(QPaintEvent* event) override;
        virtual void hideEvent(QHideEvent *e) override;
        virtual void showEvent(QShowEvent *e) override;

    private:
        enum StickerPosition
        {
            Centre,
            Left,
            Right,
            Top,
            Bottom,
            Tab,
        };

        void initializeStickersImages(void);
        bool m_isActive;
        bool m_tabVisible;

        QRect m_rcCentre;
        QRect m_rcLeft;
        QRect m_rcRight;
        QRect m_rcTop;
        QRect m_rcBottom;
        QRect m_rcTab;

        DockingFrameFrameSticker *m_frameLeftSticker;
        DockingFrameFrameSticker *m_frameRightSticker;
        DockingFrameFrameSticker *m_frameTopSticker;
        DockingFrameFrameSticker *m_frameBottomSticker;

        QMap<StickerPosition, QImage> m_activeStickers;
        QMap<StickerPosition, QImage> m_inactiveStickers;

};

#endif // DOCKINGFRAMECENTRESTICKER_H
