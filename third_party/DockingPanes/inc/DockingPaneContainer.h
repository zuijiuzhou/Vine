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
#include "DockingPaneFlyoutWidget.h"
#include <QPointer>

class QGridLayout;

class DockingPaneFlyoutWidget;
class DockingPaneGlow;
class DockingPaneManager;
class DockingPaneTitleWidget;
class DockingToolButton;

/**
 * \brief 单个停靠窗格：标题栏 + 关闭/固定按钮 + 客户区。
 *
 * 是停靠树中的基本叶子节点。可处于停靠、浮动、自动隐藏、Tab 四种形态（见 DockingPaneBase::State）。
 * 浮动时为 Qt::ToolTip 无边框置顶窗口，带边缘缩放光晕（DockingPaneGlow）。
 *
 * \note 注意事项：
 *  - 默认构造 DockingPaneContainer(QWidget*) 仅面向派生类（DockingPaneTabbedContainer）使用，成员未初始化，直接调用其方法会崩溃。
 *  - 作为 Tab 子窗格时，只有 clientWidget 被放进标签组，容器对象本身不reparent；
 *    关闭/移动必须经由 DockingPaneManager::closePane() 路由或 DockingPaneTabbedContainer::closePane()，不要直接操作。
 *  - m_flyoutWidget 为 QPointer，flyout 被管理器删除后自动置空。
 *  - 拖动标题栏超过 5px 会转为浮动（floatPane），期间窗口重建会丢失鼠标抓取，需要 reacquireGrab()/takeGrab() 重新抓取。
 */
class DockingPaneContainer : public DockingPaneBase {
    Q_OBJECT

  public:
    /**
     * \brief 自动隐藏弹出方向（flyout 从哪条边弹出）。
     */
    enum FlyoutPosition
    {
        Left,
        Right,
        Top,
        Bottom
    };

    friend class DockingPaneManager;

  public:
    /**
     * \brief 完整构造（含标题、id、父窗口、客户区）。
     */
    explicit DockingPaneContainer(const QString& title, const QString& id, QWidget* parent = nullptr, QWidget* clientWidget = nullptr);

    /**
     * \brief 默认构造，仅供派生类初始化自身成员后使用。
     * \note 本构造不创建标题栏/按钮/客户区，直接使用会解引用空指针。
     */
    explicit DockingPaneContainer(QWidget* parent = nullptr);
    ~DockingPaneContainer() override;

  public:
    /**
     * \brief 转为浮动窗格（QRect 版本，参数当前被忽略，仅用于状态切换）。
     */
    void floatPane(QRect rect);

    /**
     * \brief 从当前位置偏移 pos 转为浮动窗格。
     *
     * 先经 closePane() 从停靠树摘除，再按记录的全局位置浮动。
     * \note 需要 dockingManager() 与 mainWindow() 有效（否则为无父顶层窗口）。
     */
    void floatPane(QPoint pos);

    /**
     * \brief 打开自动隐藏 flyout。
     * \param hasFocus 是否立即获得焦点（否则 1s 后进入自动隐藏超时）。
     * \param parent
     * \param pos
     * \param pane     要弹出的子窗格（Tab 容器按标签选择）。
     * \return 新建的 flyout 控件。
     */
    virtual DockingPaneFlyoutWidget* openFlyout(bool hasFocus, QWidget* parent, FlyoutPosition pos, DockingPaneContainer* pane);

    /**
     * \brief 设置状态；非 Floating 时显示固定按钮并释放光晕。
     */
    void setState(State state) override;

    /**
     * \brief 子窗格数量（单窗格恒为 1；Tab 容器返回标签数）。
     */
    virtual int getPaneCount();

    /**
     * \brief 取第 index 个子窗格（单窗格返回自身）。
     */
    virtual DockingPaneContainer* getPane(int index);

    /**
     * \brief 当前客户区控件。
     */
    QWidget* clientWidget();

    /**
     * \brief 设置客户区控件。
     * \note widget 为 nullptr 时仅清空布局并返回（不会崩溃）。
     */
    virtual void setClientWidget(QWidget* widget);

    void saveLayout(QDomNode* parentNode, bool includeGeometry = false) override;

    /**
     * \brief flyout 尺寸（未设置时默认 100x100）。
     */
    QSize flyoutSize();

    /**
     * \brief 设置 flyout 尺寸（自动隐藏时保存窗格尺寸用）。
     */
    void setFlyoutSize(QSize flyoutSize);

    /**
     * \brief 浮动时的边缘缩放光晕对象。
     */
    DockingPaneGlow* floatingGlow();

    // --- Feature toggles ---

    /**
     * \brief 是否可关闭（仅控制标题栏关闭按钮可见性）。
     * \note 不阻止编程式 closePane()/removeDockPanel() 等其它关闭路径。
     */
    void setClosable(bool closable);
    bool isClosable() const;

    // --- Close callback ---

    /// 关闭回调：返回 false 可否决本次关闭（宿主可借此拦截）。
    using CloseCallback = bool (*)(DockingPaneContainer*);

    /**
     * \brief 设置关闭回调（宿主安装 onClosing 拦截用）。
     */
    void setCloseCallback(CloseCallback cb)
    {
        m_closeCallback = cb;
    }

    /**
     * \brief 调用关闭回调；无回调时视为允许关闭。
     */
    bool invokeCloseCallback()
    {
        return m_closeCallback ? m_closeCallback(this) : true;
    }

    /**
     * \brief flyout 拖出转浮动后 flyout 隐藏导致鼠标抓取释放，
     *        由目标 pane 标题接管以继续拖动。
     */
    void continueDrag(QPoint pos);

  protected:
    void setName(const QString& name) override;

    /// 激活态变化时更新标题/按钮外观（焦点变化触发）。
    void setActivePane(bool active);
    void paintEvent(QPaintEvent* event) override;
    void changeEvent(QEvent* event) override;

    // ---- 标题栏拖动（停靠/浮动切换）----
    virtual void onStartDragTitle(QPoint pos);
    virtual void onEndDragTitle(QPoint pos);
    virtual void onMoveDragTitle(QPoint pos);

    // ---- flyout 标题拖动（自动隐藏拖出为浮动）----
    virtual void onStartDragFlyoutTitle(QPoint pos);
    virtual void onEndDragFlyoutTitle(QPoint pos);
    virtual void onMoveDragFlyoutTitle(QPoint pos);

    /// 标题栏关闭按钮被点击（先经 close 回调，可被否决）。
    virtual void onCloseButtonClicked();
    /// flyout 的关闭按钮被点击（关闭固定窗格）。
    virtual void onCloseContainer();
    /// qApp 焦点变化：设置本窗格激活态。
    virtual void onFocusChanged(QWidget* old, QWidget* now);
    /// 固定按钮被点击：走 hidePane 进入自动隐藏。
    virtual void onPinButtonClicked();
    /// flyout 的固定按钮被点击：unpin。
    virtual void onUnpinContainer();

  protected:
    QWidget*     m_headerWidget = nullptr;
    QWidget*     m_clientWidget = nullptr;
    QGridLayout* m_clientLayout = nullptr;

    DockingToolButton* m_closeButton = nullptr;
    DockingToolButton* m_pinButton   = nullptr;

    bool   m_isActive = false;
    QPoint m_initialPos;

    DockingPaneTitleWidget* m_titleWidget            = nullptr;
    // QPointer so the container never keeps a dangling flyout pointer
    // after the manager has deleted the flyout (focus-lost path).
    QPointer<DockingPaneFlyoutWidget> m_flyoutWidget = nullptr;

    QSize m_flyoutSize;

    DockingPaneGlow* m_floatingGlow = nullptr;

    bool m_draggingFlyout = false;

    bool m_closable = true;

    CloseCallback m_closeCallback = nullptr;

  private:
    void onFClicked();
};

#endif // DOCKINGPANECONTAINER_H
