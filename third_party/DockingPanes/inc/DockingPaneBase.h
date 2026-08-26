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

#ifndef DOCKINGPANEBASE_H
#define DOCKINGPANEBASE_H

#include <QWidget>

class QDomNode;

class DockingPaneManager;

/**
 * \brief 停靠系统所有节点的抽象基类。
 *
 * 停靠树中的每个元素（单窗格 DockingPaneContainer、标签组 DockingPaneTabbedContainer、分割器 DockingPaneSplitterContainer、中央客户区
 * DockingPaneClient）都继承自本类。 提供统一的身份（id/name）、状态（State）与宿主数据（userData）。
 *
 * \note 已知设计缺陷：setState() 只是给 m_state 赋值，不附带任何副作用；
 * 真正的状态转换（隐藏/显示/固定/浮动等）必须由具体子类或
 * DockingPaneManager 的对应操作完成，调用方不要依赖 setState 产生界面变化。
 */
class DockingPaneBase : public QWidget {
    Q_OBJECT

  public:
    /**
     * \brief 窗格生命周期状态。
     *
     * Hidden  已关闭/隐藏（仍可能留在树中）；
     * Docked  停靠在树中；
     * Floating 浮动独立窗口；
     * Pinned  自动隐藏（边缘有按钮）；
     * Tabbed  是某个 DockingPaneTabbedContainer 的标签页。
     */
    enum State
    {
        Hidden,
        Docked,
        Floating,
        Pinned,
        Tabbed
    };

    friend class DockingPaneManager;

  public:
    explicit DockingPaneBase(QWidget* parent = nullptr);
    ~DockingPaneBase() override;

  public:
    /**
     * \brief 显示名称（窗格标题 / Tab 文本来源）。
     */
    const QString& name();

    /**
     * \brief 唯一标识（用于布局恢复与查找）。
     */
    const QString& id();

    /**
     * \brief 返回所属的 DockingPaneManager。
     * \note 未加入管理器时为 nullptr。
     */
    DockingPaneManager* dockingManager();

    /**
     * \brief 设置宿主自定义数据（如宿主 UIElement 包装对象指针）。
     * \note 原始 void*，调用方负责生命周期；删除所指对象前应清空。
     */
    void setUserData(void* data)
    {
        m_userData = data;
    }

    void* userData() const
    {
        return m_userData;
    }

    /**
     * \brief 当前状态。 \see State
     */
    virtual State state();

    /**
     * \brief 设置状态，仅内部使用。
     * \note 仅赋值，不附带界面副作用（见类说明）。
     * \note 仅供派生类与 DockingPaneManager（friend）使用，宿主请通过管理器的高层操作（dockPane/closePane/hidePane 等）改变状态。
     */
    virtual void setState(State state);

    /**
     * \brief 将本节点及其子节点写入父 XML 节点（布局保存）。
     * \param parentNode
     * \param includeGeometry 是否包含几何信息（浮动窗格用）。
     */
    virtual void saveLayout(QDomNode* parentNode, bool includeGeometry = false);

  protected:
    virtual void setName(const QString& name);
    virtual void setId(const QString& id);

  protected:
    bool                m_isClient;           ///< 是否中央客户区。
    DockingPaneManager* m_dockingManager;     ///< 所属管理器。
    State               m_state;              ///< 当前状态。
    QString             m_name;               ///< 显示名称。
    QString             m_id;                 ///< 唯一标识。
    void*               m_userData = nullptr; ///< 宿主自定义数据。
};

#endif // DOCKINGPANEBASE_H
