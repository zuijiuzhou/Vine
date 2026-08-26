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

#ifndef DOCKINGPANEFLYOUTWIDGET_H
#define DOCKINGPANEFLYOUTWIDGET_H

#include <QWidget>

class QGridLayout;

class DockingPaneContainer;
class DockingToolButton;
class DockingPaneTitleWidget;

/**
 * \brief 自动隐藏（Pinned）窗格的弹出窗口（flyout）。
 *
 * 点击/悬停边缘自动隐藏按钮时从对应边弹出，带标题栏（可拖动）与 6px 宽的缩放边缘。聚焦丢失（onFocusChanged）或 beginDrag/endDrag 会通知管理器关闭自身。
 *
 * \note 注意事项：
 *  - 依赖 QCursor::pos() 计算光标位置，Wayland 下失效（宿主强制 xcb 规避）。
 *  - 无聚焦打开时 1s 后触发 autoHideFlyout 信号，但该信号当前未被连接（自动隐藏只依赖焦点丢失）。
 *  - 对象所有权归 DockingPaneManager/容器（QPointer 管理），不要自行 delete。
 */
class DockingPaneFlyoutWidget : public QWidget {
    Q_OBJECT

  public:
    enum FlyoutPosition
    {
        Left,
        Right,
        Top,
        Bottom
    };

  public:
    /**
     * \brief 构造 flyout。
     * \param hasFocus 是否立即取得焦点。
     * \param container 所属容器（用于取 flyout 尺寸 / 归还客户区）。
     * \param pane      被弹出的子窗格。
     * \param pos       弹出方向。
     * \param widget    要显示的客户区控件。
     * \param parent
     */
    explicit DockingPaneFlyoutWidget(bool                  hasFocus,
                                     DockingPaneContainer* container,
                                     DockingPaneContainer* pane,
                                     FlyoutPosition        pos,
                                     QWidget*              widget,
                                     QWidget*              parent = nullptr);
    ~DockingPaneFlyoutWidget() override;

  public:
    /**
     * \brief 把客户区控件归还给所属容器（关闭/拖动时调用）。
     */
    void restorePaneWidget();

    /**
     * \brief 当前被弹出的子窗格。
     */
    DockingPaneContainer* pane();

    /**
     * \brief 开始拖出（隐藏自身并把客户区归还容器）。
     */
    void beginDrag();

    /**
     * \brief 结束拖出（发射 flyoutFocusLost 通知管理器清理）。
     */
    void endDrag();

    /**
     * \brief 客户区控件。
     */
    QWidget* clientWidget();

    /**
     * \brief 可视内容矩形（去掉 5~6px 缩放边缘后的区域）。
     */
    QRect paneRect();

  Q_SIGNALS:
    void unpinContainer(); ///< 固定按钮被点击。
    void closeContainer(); ///< 关闭按钮被点击。
    void startDragFlyoutTitle(QPoint pos);
    void endDragFlyoutTitle(QPoint pos);
    void moveDragFlyoutTitle(QPoint pos);
    void flyoutFocusLost(); ///< 焦点丢失 / 拖出结束（管理器据此清理）。
    void autoHideFlyout();  ///< 无焦点超时自动隐藏（当前未连接）。

  public:
    bool eventFilter(QObject* obj, QEvent* event) override;

  protected:
    void paintEvent(QPaintEvent* event) override;
    void changeEvent(QEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

  private Q_SLOTS:
    void onFocusChanged(QWidget* old, QWidget* now);

  private:
    void setActivePane(bool active);
    void setPositionAndSize();
    void updateCursor();
    void autoHideTimeout();

  private:
    DockingPaneContainer*   m_pane;
    DockingPaneContainer*   m_container;
    QWidget*                m_clientWidget;
    DockingPaneTitleWidget* m_titleWidget;
    QGridLayout*            m_clientLayout;

    QWidget*           m_headerWidget;
    DockingToolButton* m_closeButton;
    DockingToolButton* m_pinButton;
    QPoint             m_initialPos;
    FlyoutPosition     m_pos;
    int                m_size;
    bool               m_isActive;
    bool               m_dragMode;
    bool               m_resizeMode;
};

#endif // DOCKINGPANEFLYOUTWIDGET_H
