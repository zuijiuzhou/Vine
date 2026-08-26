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

#ifndef DOCKINGTOOLBUTTON_H
#define DOCKINGTOOLBUTTON_H

#include <QPushButton>

/**
 * \brief 标题栏上的关闭/固定小按钮（位图 + 调色板着色）。
 *
 * 按状态（激活/非激活、关闭/固定/取消固定）从资源加载对应 PNG，非激活位图会被重着色为调色板 WindowText 色以适配深色主题；悬停时绘制半透明白色高亮块。
 *
 * \note 继承 QPushButton 但完全重写绘制，仅复用其点击/悬停语义；16px 最大宽度，命中区域较小。
 */
class DockingToolButton : public QPushButton {
    Q_OBJECT

  public:
    /// 按钮形态（图标 + 激活态组合）。
    enum ButtonType
    {
        closeButtonActive,   // 关闭图标，激活态（白色）。
        closeButtonInactive, // 关闭图标，非激活态（着色）。
        pinButtonActive,     // 固定图标，激活态。
        pinButtonInactive,   // 固定图标，非激活态。
        unpinButtonActive,   // 取消固定图标（旋转 90°）。
        unpinButtonInactive  // 取消固定图标，非激活态。
    };

  public:
    explicit DockingToolButton(ButtonType type, QWidget* parent = nullptr);
    ~DockingToolButton() override;

  public:
    /**
     * \brief 切换按钮形态（激活/非激活、关闭/固定）。
     */
    void setButton(ButtonType type);

  protected:
    void paintEvent(QPaintEvent* event) override;
    void changeEvent(QEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;

  private:
    ButtonType m_buttonType;
    bool       m_highlight; ///< 悬停高亮。
};

#endif // DOCKINGTOOLBUTTON_H
