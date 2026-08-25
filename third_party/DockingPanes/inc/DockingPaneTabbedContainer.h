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

#ifndef DOCKINGPANETABBEDCONTAINER_H
#define DOCKINGPANETABBEDCONTAINER_H

#include <QList>

#include "DockingPaneContainer.h"

class QMouseEvent;
class QStackedWidget;

class DockingPaneFlyoutWidget;
class DockingPaneManager;

class DockingPaneTabbedContainer : public DockingPaneContainer
{
    Q_OBJECT
    public:
        friend class DockingPaneManager;
        explicit DockingPaneTabbedContainer(QWidget *parent = nullptr);
        ~DockingPaneTabbedContainer();

        bool addPane(DockingPaneContainer *child);
        void restoreChildWidgets(void);
        void setVisiblePane(DockingPaneContainer *pane);
        bool containsPane(DockingPaneContainer *pane);
        virtual void onCloseButtonClicked(void) override;
        virtual void onPinButtonClicked(void) override;
        // DockingPaneContainer overrides
        virtual int getPaneCount(void) override;
        virtual DockingPaneContainer *getPane(int index) override;
        virtual void saveLayout(QDomNode *parentNode, bool includeGeometry=false) override;
        virtual DockingPaneFlyoutWidget *openFlyout(bool hasFocus, QWidget *parent, FlyoutPosition pos, DockingPaneContainer *pane) override;
        virtual void setClientWidget(QWidget *widget) override;

    protected:
        virtual void paintEvent(QPaintEvent* event) override;
        virtual void mousePressEvent(QMouseEvent* event) override;
        virtual void mouseReleaseEvent(QMouseEvent* event) override;
        virtual void mouseMoveEvent(QMouseEvent*event) override;
        virtual void resizeEvent(QResizeEvent* event) override;
        virtual void onStartDragTitle(QPoint pos) override;
        virtual void onEndDragTitle(QPoint pos) override;
        virtual void onMoveDragTitle(QPoint pos) override;
        virtual void onStartDragFlyoutTitle(QPoint pos) override;
        virtual void onEndDragFlyoutTitle(QPoint pos) override;
        virtual void onMoveDragFlyoutTitle(QPoint pos) override;

        QStackedWidget *m_stackedWidget = nullptr;
        QList<DockingPaneContainer *> m_paneList;
        QList<int> m_tabWidths;
        DockingPaneContainer *m_draggedPane = nullptr;
        QPoint m_originalClickPos;
        QRect m_invalidTabRect;
        bool m_fromMousePressEvent = false;

    private:
        void calculateButtonsRectangles(void);
        QRect getButtonRect(int pos);
        void updateMargins(void);
        virtual void onUnpinContainer(void) override;
        virtual void onCloseContainer(void) override;
        virtual void onFocusChanged(QWidget *old, QWidget *now) override;
        void syncFeaturesFromCurrentPane();
};

#endif // DOCKINGPANETABBEDCONTAINER_H
