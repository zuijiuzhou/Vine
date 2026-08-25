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

#ifndef DOCKINGPANECONTAINER_H
#define DOCKINGPANECONTAINER_H

#include "DockingPaneBase.h"

class QGridLayout;

class DockingPaneFlyoutWidget;
class DockingPaneGlow;
class DockingPaneManager;
class DockingPaneTitleWidget;
class DockingToolButton;

class DockingPaneContainer : public DockingPaneBase
{
    Q_OBJECT

    public:
        enum FlyoutPosition
        {
            Left,
            Right,
            Top,
            Bottom
        };

        explicit DockingPaneContainer(QString title, QString id, QWidget *parent = nullptr, QWidget *clientWidget = nullptr);
        explicit DockingPaneContainer(QWidget *parent = nullptr);
        virtual ~DockingPaneContainer();

        void floatPane(QRect rect);
        void floatPane(QPoint pos);
        virtual int getPaneCount(void);
        virtual DockingPaneContainer *getPane(int index);
        virtual DockingPaneFlyoutWidget *openFlyout(bool hasFocus, QWidget *parent, FlyoutPosition pos, DockingPaneContainer *pane);
        friend class DockingPaneManager;

        QWidget *clientWidget();
        virtual void setClientWidget(QWidget *widget);

        virtual void saveLayout(QDomNode *parentNode, bool includeGeometry=false) override;

        QSize flyoutSize(void);
        void setFlyoutSize(QSize flyoutSize);
        DockingPaneGlow *floatingGlow(void);
        virtual void setState(DockingPaneBase::State state) override;

        // --- Feature toggles ---
        void setClosable(bool closable);
        bool isClosable() const;

        // --- Close callback ---
        using CloseCallback = bool (*)(DockingPaneContainer*);
        void setCloseCallback(CloseCallback cb) { m_closeCallback = cb; }
        bool invokeCloseCallback() { return m_closeCallback ? m_closeCallback(this) : true; }

        // flyout 拖出转浮动后 flyout 隐藏导致鼠标抓取释放, 由目标 pane 标题接管以继续拖动
        void continueDrag(QPoint pos);

    protected:
        virtual void setName(QString name) override;
        void setActivePane(bool active);
        virtual void paintEvent(QPaintEvent* event) override;
        virtual void changeEvent(QEvent* event) override;

        virtual void onStartDragTitle(QPoint pos);
        virtual void onEndDragTitle(QPoint pos);
        virtual void onMoveDragTitle(QPoint pos);

        virtual void onStartDragFlyoutTitle(QPoint pos);
        virtual void onEndDragFlyoutTitle(QPoint pos);
        virtual void onMoveDragFlyoutTitle(QPoint pos);

        virtual void onCloseButtonClicked(void);
        virtual void onCloseContainer(void);
        virtual void onFocusChanged(QWidget *old, QWidget *now);
        virtual void onPinButtonClicked(void);
        virtual void onUnpinContainer(void);

        QWidget *m_headerWidget = nullptr;
        QWidget *m_clientWidget = nullptr;
        QGridLayout *m_clientLayout = nullptr;

        DockingToolButton *m_closeButton = nullptr;
        DockingToolButton *m_pinButton = nullptr;

        bool m_isActive = false;
        QPoint m_initialPos;

        DockingPaneTitleWidget *m_titleWidget = nullptr;
        DockingPaneFlyoutWidget *m_flyoutWidget = nullptr;

        QSize m_flyoutSize;

        DockingPaneGlow *m_floatingGlow = nullptr;

        bool m_draggingFlyout = false;

        bool m_closable = true;

        CloseCallback m_closeCallback = nullptr;

     private:
        void onFClicked(void);
};

#endif // DOCKINGPANECONTAINER_H
