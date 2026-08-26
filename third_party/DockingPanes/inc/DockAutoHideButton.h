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

#ifndef DOCKAUTOHIDEBUTTON_H
#define DOCKAUTOHIDEBUTTON_H

#include <QPushButton>

#include "DockingPaneBase.h"
#include "DockingPaneContainer.h"

class QStyleOptionButton;
class QTimer;

/**
 * \brief 自动隐藏条上的按钮（显示窗格名称，可横/竖排）。
 *
 * 位于主窗口边缘的自动隐藏条上，指向一个被固定（Pinned）的窗格，点击（clicked）会经 DockingPaneManager 打开对应 flyout。
 */
class DockAutoHideButton : public QPushButton {
    Q_OBJECT

  public:
    /**
     * \brief 按钮所在边缘（决定文字排布方向与 swap）。
     */
    enum Position
    {
        Left,
        Right,
        Top,
        Bottom
    };

  public:
    explicit DockAutoHideButton(Position pos, QWidget* parent = nullptr);
    explicit DockAutoHideButton(const QString& text, QWidget* parent = nullptr);
    DockAutoHideButton(const QIcon& icon, const QString& text, QWidget* parent = nullptr);

  public:
    /**
     * \brief 文字排布方向（Horizontal/Vertical）。
     */
    Qt::Orientation orientation() const;
    /**
     * \brief 设置文字排布方向（Horizontal/Vertical）。
     */
    void setOrientation(Qt::Orientation orientation);

    bool mirrored() const;
    void setMirrored(bool mirrored);

    /**
     * \brief 切换文字/色条绘制方向（边缘内侧/外侧）。
     */
    void swapDirection(bool state);

    QSize sizeHint() const override;

    /**
     * \brief 按钮所在边缘。
     */
    Position position();

    /**
     * \brief 设置按钮指向的容器与子窗格。
     */
    void setPane(DockingPaneContainer* container, DockingPaneBase* pane);

    /**
     * \brief 指向的子窗格。
     */
    DockingPaneBase* pane();

    /**
     * \brief 指向的容器（含自动隐藏按钮）。
     */
    DockingPaneContainer* container();

  protected:
    void paintEvent(QPaintEvent* event) override;
    void changeEvent(QEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

  Q_SIGNALS:
    void openFlyout();

  private:
    void init();
    /*
     * @brief 获取当前对象的StyleOption
     */
    QStyleOptionButton* getStyleOption() const;
    /*
     * @brief 悬停计时到点
     */
    void onTimerElapsed();

  private:
    Qt::Orientation       m_orientation;
    bool                  m_mirrored;
    bool                  m_hovered;
    DockingPaneBase*      m_dockingPane;   // 指向的子窗格（原始指针）。
    DockingPaneContainer* m_paneContainer; //  指向的容器（原始指针）。

    bool     m_swapDirection;
    Position m_pos;
    QTimer*  m_hoverTimer;
};

#endif // DOCKAUTOHIDEBUTTON_H
