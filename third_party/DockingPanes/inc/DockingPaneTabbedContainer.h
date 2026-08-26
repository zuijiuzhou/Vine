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

/**
 * @brief Tab container: shows multiple panes merged together with a tab strip at the bottom.
 *
 * Internally a QStackedWidget holds each child pane's clientWidget, and the tab strip is
 * drawn manually (DockingPaneTabbedContainer overrides paintEvent and the mouse events).
 * When the tab count drops to 1 it automatically "collapses" back to a single pane and deletes itself.
 *
 * @note Key design: child panes only put their clientWidget into the QStackedWidget; the child
 * container objects are not reparented into this container. Therefore:
 *  - Closing/moving a child pane must call this class's closePane() or DockingPaneManager::closePane()
 *    (which routes here); deleting a child container directly leaves this container's m_paneList dangling.
 *  - The title-bar close button's closability follows the current tab (syncFeaturesFromCurrentPane).
 *  - When floating the window type is Qt::Tool (inconsistent with the single pane's Qt::ToolTip; a historical leftover).
 */
class DockingPaneTabbedContainer : public DockingPaneContainer {
    Q_OBJECT
  public:
    friend class DockingPaneManager;

    /**
     * @brief Constructs an empty tab container.
     */
    explicit DockingPaneTabbedContainer(QWidget* parent = nullptr);
    ~DockingPaneTabbedContainer() override;

  public:
    /**
     * @brief Adds a child pane.
     * @param child The child pane; if it is another tab container, all its tabs are merged.
     * @return true if child was merged in (the caller must delete the original container).
     */
    bool addPane(DockingPaneContainer* child);
    /**
     * @brief Number of child panes (tab count).
     */
    int getPaneCount() override;
    /**
     * @brief Returns the child pane at index.
     */
    DockingPaneContainer*    getPane(int index) override;
    void                     saveLayout(QDomNode* parentNode, bool includeGeometry = false) override;
    DockingPaneFlyoutWidget* openFlyout(bool hasFocus, QWidget* parent, FlyoutPosition pos, DockingPaneContainer* pane) override;
    void                     setClientWidget(QWidget* widget) override;

  protected:
    /**
     * @brief Returns each widget in the QStackedWidget to its corresponding child pane
     *        (used when the tab group collapses to a single pane).
     */
    void restoreChildWidgets();

    /**
     * @brief Sets a child pane as the current visible page.
     * @note For DockingPaneManager (friend) internal use only.
     */
    void setVisiblePane(DockingPaneContainer* pane);

    /**
     * @brief Whether a child pane is in this tab group.
     * @note For DockingPaneManager (friend) internal use only.
     */
    bool containsPane(DockingPaneContainer* pane);

    /**
     * @brief Closes (removes) a child pane: removes its tab and updates the tab strip;
     *        when only one remains, collapses to a single pane and deletes itself.
     * @note For DockingPaneManager (friend) routing and this class's own use;
     * invokes the child pane's close callback (which may veto; then the tab is kept).
     */
    void closePane(DockingPaneContainer* pane);

    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void onStartDragTitle(QPoint pos) override;
    void onEndDragTitle(QPoint pos) override;
    void onMoveDragTitle(QPoint pos) override;
    void onStartDragFlyoutTitle(QPoint pos) override;
    void onEndDragFlyoutTitle(QPoint pos) override;
    void onMoveDragFlyoutTitle(QPoint pos) override;
    void onCloseButtonClicked() override;
    void onPinButtonClicked() override;
    void onUnpinContainer() override;
    void onCloseContainer() override;
    void onFocusChanged(QWidget* old, QWidget* now) override;

  private:
    void  calculateButtonsRectangles();
    QRect getButtonRect(int pos);
    void  updateMargins();
    /// Syncs the title-bar close button's closability to the current tab's isClosable().
    void syncFeaturesFromCurrentPane();

  protected:
    QStackedWidget*              m_stackedWidget = nullptr;     // Holds each child pane's clientWidget.
    QList<DockingPaneContainer*> m_paneList;                    // Child panes (same order as the stacked widget).
    QList<int>                   m_tabWidths;                   // Computed width of each tab.
    DockingPaneContainer*        m_draggedPane = nullptr;       // The tab currently being dragged out.
    QPoint                       m_originalClickPos;            // Global position at press time.
    QRect                        m_invalidTabRect;              // Avoids jitter when reordering tabs.
    bool                         m_fromMousePressEvent = false; // Whether the interaction started with a mouse press.
};

#endif // DOCKINGPANETABBEDCONTAINER_H
