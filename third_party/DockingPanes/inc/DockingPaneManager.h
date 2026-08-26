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

#ifndef DOCKINGPANEMANAGER_H
#define DOCKINGPANEMANAGER_H

#include <QWidget>

class QBoxLayout;
class QDomDocument;
class QDomNode;
class DockingPaneBase;
class DockingPaneSplitterContainer;
class DockingPaneManagerPrivate;
class DockingFrameStickers;
class DockingTargetWidget;
class DockingPaneTitleWidget;
class DockingPaneFlyoutWidget;
class DockAutoHideButton;

/**
 * \brief 停靠系统的核心管理器。
 *
 * 持有整棵停靠树（根节点 m_rootPane）、中央客户区（DockingPaneClient）、四个自动隐藏按钮条、拖放指示器（DockingFrameStickers / DockingTargetWidget）以及当前
 * flyout。 负责窗格的创建、停靠、关闭、隐藏/显示、浮动、自动隐藏，以及布局的保存与恢复。宿主通过 widget() 取得顶层控件嵌入主窗口。
 *
 * \note 注意事项：
 *  - widget() 未调用前四个自动隐藏条未创建，此时调用 saveLayout() / updateAutohideButton() / removePinnedButton() 会解引用空指针。
 *  - closePane() 对“已提交的 Tab 子窗格”（state()==Tabbed）会路由到所属DockingPaneTabbedContainer::closePane() 完成标签移除；
 *    applyLayout() 恢复期间（m_closingAll）不路由，避免触发 close 回调与过早拆组。
 *  - createPane() 使用重复 id 会直接覆盖 m_dockingPaneMap 中的旧条目。
 *  - 本类不删除停靠树（m_dockingWidget / m_thisWidget / m_clientPane），它们归宿主（如 setCentralWidget）所有，析构时仅清理内部指示器与 flyout。
 */
class DockingPaneManager : public QObject {
    Q_OBJECT

  public:
    /**
     * \brief 停靠方位。
     *
     * dockTab 表示并入某个窗格的标签组（或与之合并成新标签组）。
     */
    enum DockPosition
    {
        dockLeft,
        dockRight,
        dockTop,
        dockBottom,
        dockFloat,
        dockTab
    };

  public:
    /**
     * \brief 构造管理器。
     *
     * 仅创建内部结构（根/客户窗格/指示器），不创建自动隐藏条
     * （见 widget()）。
     */
    DockingPaneManager();

    /**
     * \brief 析构。释放内部指示器与当前 flyout；停靠树归宿主所有。
     */
    ~DockingPaneManager() override;

  public:
    /**
     * \brief 返回容纳停靠系统的顶层 QWidget（懒创建）。
     *
     * 首次调用会创建四个自动隐藏按钮条并安装事件过滤器。
     * \return 可嵌入主窗口（如 QMainWindow::setCentralWidget）的控件。
     */
    QWidget* widget();

    /**
     * \brief 设置中央客户区控件。
     * \param widget 客户区内容（不能为 nullptr）。
     * \return 客户端窗格（DockingPaneClient）。
     */
    DockingPaneBase* setClientWidget(QWidget* widget);

    /**
     * \brief 创建一个窗格并按方位停靠（或浮动）。
     * \param id           唯一标识（重复会覆盖 map）。
     * \param title        标题。
     * \param widget       客户区内容。
     * \param initialSize  初始尺寸。
     * \param dockPosition 停靠方位；dockFloat 时直接浮动显示。
     * \param neighbourPane 邻居窗格（dockTab / 相对停靠时使用）。
     * \return 新建的 DockingPaneContainer。
     */
    DockingPaneBase* createPane(const QString&                   id,
                                const QString&                   title,
                                QWidget*                         widget,
                                QSize                            initialSize,
                                DockingPaneManager::DockPosition dockPosition,
                                DockingPaneBase*                 neighbourPane = nullptr);

    /**
     * \brief 按 id 关闭窗格。
     * \see closePane(DockingPaneBase*)
     */
    void closePane(const QString& id);

    /**
     * \brief 关闭窗格：隐藏并置为 Hidden，从停靠树摘除。
     *
     * \note 对 Tab 子窗格会路由到所属 tabbed 容器（见类说明）；
     * 只关闭、不删除对象，也不从 m_dockingPaneMap 移除。
     */
    void closePane(DockingPaneBase* dockingPane);

    /**
     * \brief 隐藏窗格并转为自动隐藏（Pinned）：创建边缘按钮。
     * \note 需要窗格位于分割器内；浮动窗格为静默 no-op。
     */
    void hidePane(DockingPaneBase* dockingPane);

    /**
     * \brief 按状态恢复显示窗格。
     * \note Hidden → 重新浮动；Pinned → 打开 flyout；Docked/Floating → 激活。
     */
    void showPane(DockingPaneBase* dockingPane);

    /**
     * \brief 从管理器记账移除并调度删除窗格对象（deleteLater）。
     */
    void deletePane(DockingPaneBase* pane);

    /**
     * \brief 取消固定（unpin）：恢复显示并移除自动隐藏按钮。
     */
    void unpinPane(DockingPaneBase* pane);

    /**
     * \brief 关闭已固定的（Pinned）窗格并关闭其 flyout。
     */
    void closePinnedPane(DockingPaneBase* pane);

    /**
     * \brief 打开某个自动隐藏按钮对应的 flyout（自动隐藏弹出窗格）。
     * \note 同一 flyout 已打开且指向同一窗格时直接返回。
     */
    void openFlyout(DockAutoHideButton* button);

    /**
     * \brief 用 newPane 替换停靠树中的 oldPane（如折叠 Tab 组为单窗格）。
     */
    void replacePane(DockingPaneBase* oldPane, DockingPaneBase* newPane);

    /**
     * \brief 停靠操作后更新自动隐藏按钮所指向的容器/窗格。
     */
    void updateAutohideButton(DockingPaneBase* oldContainer, DockingPaneBase* oldPane, DockingPaneBase* newContainer, DockingPaneBase* newPane);

    /**
     * \brief 核心停靠逻辑。
     *
     * dockTab 时并入 neighbourPane 的标签组或新建标签组；其余方位创建
     * 新分割器并插入停靠树。
     * \note 对已是 Tab 的窗格会先经 closePane 从旧组摘除再入新组。
     * \return 停靠完成后的容器。
     */
    DockingPaneBase* dockPane(DockingPaneBase* newPane, DockingPaneManager::DockPosition dockPosition, DockingPaneBase* neighbourPane);

    /**
     * \brief 序列化当前布局（停靠树 + 固定 + 浮动）为 XML。
     * \note 需要先调用 widget() 以创建自动隐藏条，否则崩溃。
     */
    QString saveLayout(const QString& id);

    /**
     * \brief 从 XML 恢复布局。
     * \return 成功与否；失败时回退到仅含客户区。
     */
    bool applyLayout(const QString& layout);

    /**
     * \brief 设置主窗口；浮动窗格将以其为父窗口。
     */
    void setMainWindow(QWidget* window);

    /**
     * \brief 返回主窗口。
     */
    QWidget* mainWindow();

    /**
     * \brief 调试：向 qDebug 输出所有窗格及其状态。
     */
    void dumpPaneList();

    /**
     * \brief 按索引取窗格。
     */
    DockingPaneBase* pane(int index) const;

    /**
     * \brief 窗格总数。
     */
    int paneCount() const;

    /**
     * \brief 查询窗格当前所处停靠方位（动态计算）。
     *
     * Docked → 相对客户区的方位；Tabbed → 所属标签组的方位；
     * Floating/Pinned/Hidden → dockFloat。
     * \note 供宿主实时查询（如 dockArea）；不要用于决定停靠目标。
     */
    DockPosition dockPositionOf(DockingPaneBase* pane);

  public:
    /**
     * \brief 拖动浮动窗格时更新停靠指示器（由容器拖动回调调用）。
     */
    void floatingPaneMoved(DockingPaneBase* pane, QPoint cursorPos);

    /**
     * \brief 拖动结束：用松手位置重新命中检测并停靠。
     */
    void floatingPaneEndMove(DockingPaneBase* pane, QPoint cursorPos);

    /**
     * \brief 拖动开始：初始化指示器。
     */
    void floatingPaneStartMove(DockingPaneBase* pane, QPoint cursorPos);

    /**
     * \brief 移除指定容器的自动隐藏按钮（dockingPane 为空时移除全部）。
     */
    void removePinnedButton(DockingPaneBase* dockingPaneContainer, DockingPaneBase* dockingPane = nullptr);

  private:
    friend class DockingPaneContainer;
    friend class DockingPaneTabbedContainer;

    DockPosition getClientPaneDirection(DockingPaneBase* dockingPane);
    bool         containsPane(QWidget* widget, QWidget* child);
    void         saveFloatingState(QDomNode* parentNode);

    DockingPaneBase* restoreLayout(const QDomNode& node);
    void             savePinnedState(QDomNode* parentNode, QBoxLayout* layout);
    void             restorePinnedPanes(QDomNode* node);
    void             reparentPane(DockingPaneSplitterContainer* previousParentSplitter, DockingPaneBase* dockingPane);
    void             restoreFloatingPanes(QDomNode* node);

  public:
    bool eventFilter(QObject* obj, QEvent* event) override;

  private:
    void             setWidget(QWidget* widget);
    DockingPaneBase* getDockingParent(QWidget* widget);

    void updateAllSplitters(DockingPaneSplitterContainer* parentSplitter = nullptr, bool* containsClient = nullptr);
    bool updateFloatingPane(DockingPaneBase* currentPane, QPoint cursorPos);

  private Q_SLOTS:
    void onAutoDockButtonClicked();
    void onFlyoutFocusLost();

  protected:
    DockingPaneManagerPrivate* const d_ptr;

  private:
    Q_DECLARE_PRIVATE(DockingPaneManager)
};

#endif // DOCKINGPANEMANAGER_H
