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
 * \brief Tab 标签容器：把多个窗格合并显示，底部有标签条。
 *
 * 内部用 QStackedWidget 承载各子窗格的 clientWidget，底部手绘标签条（DockingPaneTabbedContainer 重写了 paintEvent 与鼠标事件）。
 * 当标签数降到 1 时自动“折叠”回单窗格并删除自身。
 *
 * \note 关键设计：子窗格只把 clientWidget 放入 QStackedWidget，子容器对象本身不被 reparent 进本容器。因此：
 *  - 关闭/移动子窗格必须调用本类 closePane() 或 DockingPaneManager::closePane()（会路由到这里），直接删子容器会使本容器的 m_paneList 悬垂。
 *  - 标题栏关闭按钮的可关闭性跟随当前标签（syncFeaturesFromCurrentPane）。
 *  - 浮动时窗口类型为 Qt::Tool（与单窗格的 Qt::ToolTip 不一致，属历史遗留）。
 */
class DockingPaneTabbedContainer : public DockingPaneContainer {
    Q_OBJECT
  public:
    friend class DockingPaneManager;

    /**
     * \brief 构造空的标签容器。
     */
    explicit DockingPaneTabbedContainer(QWidget* parent = nullptr);
    ~DockingPaneTabbedContainer() override;

  public:
    /**
     * \brief 加入一个子窗格。
     * \param child 子窗格；若是另一个标签容器则合并其全部标签。
     * \return true 表示 child 已被并入（调用方需删除原容器）。
     */
    bool addPane(DockingPaneContainer* child);
    /**
     * \brief 子窗格数量（标签数）。
     */
    int getPaneCount() override;
    /**
     * \brief 取第 index 个子窗格。
     */
    DockingPaneContainer*    getPane(int index) override;
    void                     saveLayout(QDomNode* parentNode, bool includeGeometry = false) override;
    DockingPaneFlyoutWidget* openFlyout(bool hasFocus, QWidget* parent, FlyoutPosition pos, DockingPaneContainer* pane) override;
    void                     setClientWidget(QWidget* widget) override;

  protected:
    /**
     * \brief 把 QStackedWidget 中的各 widget 归还给对应子窗格
     *        （标签组折叠为单窗格时用）。
     */
    void restoreChildWidgets();

    /**
     * \brief 把某个子窗格设为当前显示页。
     * \note 仅供 DockingPaneManager（friend）内部调用。
     */
    void setVisiblePane(DockingPaneContainer* pane);

    /**
     * \brief 子窗格是否在本标签组内。
     * \note 仅供 DockingPaneManager（friend）内部调用。
     */
    bool containsPane(DockingPaneContainer* pane);

    /**
     * \brief 关闭（移除）一个子窗格：移除标签、更新标签条，
     *        仅剩一个时折叠为单窗格并删除自身。
     * \note 仅供 DockingPaneManager（friend）路由与本类自身使用；
     * 会调用子窗格的 close 回调（可被否决，否决则标签保留）。
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
    /// 把标题栏关闭按钮的可关闭性同步为当前标签的 isClosable()。
    void syncFeaturesFromCurrentPane();

  protected:
    QStackedWidget*              m_stackedWidget = nullptr;     // 承载各子窗格 clientWidget。
    QList<DockingPaneContainer*> m_paneList;                    // 子窗格（与 stacked 顺序一致）。
    QList<int>                   m_tabWidths;                   // 各标签计算宽度。
    DockingPaneContainer*        m_draggedPane = nullptr;       // 正在被拖出的标签。
    QPoint                       m_originalClickPos;            // 按下时全局坐标。
    QRect                        m_invalidTabRect;              // 标签重排时避免抖动。
    bool                         m_fromMousePressEvent = false; // 是否由鼠标按下开始。
};

#endif // DOCKINGPANETABBEDCONTAINER_H
