#pragma once

#include "Control.hpp"
#include "Icon.hpp"
#include "Gui.hpp"

#include <vector>

V_APPFWGUI_NS_BEGIN

class RibbonAction;

/**
 * \brief 功能区按钮（包装 SARibbonToolButton）。
 *
 * 支持文本、图标、常用 impl 方法封装，以及下拉列表：一旦加入
 * RibbonAction，会自动构建 QMenu 并挂到按钮上（InstantPopup）。
 *
 * \note 本头文件不包含、不暴露 Qt 类型（图标经 Icon 桥接）。
 * 下拉所有权规则：item 的 QAction 加入菜单后由菜单持有（item 置为不持有
 * impl）；删除 item 前请先 removeDropDownItem()。分隔线用 addSeparator()
 * 插入，不计入 dropDownItemCount()/dropDownItemAt()；如需按位置移除
 * 项或分隔线，用 dropDownEntryCount()/removeDropDownEntryAt()（统一计数）。
 */
class V_APPFW_API RibbonButton : public Control {
    V_OBJECT_META_DECL

  public:
    RibbonButton();
    virtual ~RibbonButton();

  public:
    // ---- 文本 ----
    /// 设置按钮文本。
    void text(const String& t);
    /// 获取按钮文本。
    String text() const;

    // ---- 图标 ----
    /// 设置按钮图标（经 Icon 桥接，不暴露 Qt）。
    void icon(const Icon& ic);
    /// 获取按钮图标。
    Icon icon() const;

    // ---- 常用 impl 方法封装（文本/图标/通用属性见 Control）----
    /// 设置按钮是否可选中（切换态）。
    void checkable(bool on);
    bool checkable() const;
    /// 设置按钮是否处于选中态（需先 checkable(true)）。
    void checked(bool on);
    bool checked() const;
    /**
     * \brief 设置按钮尺寸：大 / 中 / 小。
     * \note Medium 由面板行占比体现，按钮本身按小按钮渲染（SARibbon 仅有两档）。
     */
    void buttonSize(RibbonItemSize s);
    RibbonItemSize buttonSize() const;

    // ---- 显示样式 / 图标尺寸 ----
    /**
     * \brief 设置图标与文字的排布（含只显示图标）。
     * \note 大按钮默认即"图标在上、文字在下"，style() 主要对小按钮/特殊排布生效。
     */
    void style(RibbonButtonStyle s);
    RibbonButtonStyle style() const;
    /// 设置大按钮图标尺寸（配合 buttonSize(Large) 使用）。
    void largeIconSize(const Size& s);
    Size largeIconSize() const;
    /// 设置小按钮图标尺寸。
    void smallIconSize(const Size& s);
    Size smallIconSize() const;
    /**
     * \brief 本按钮文字是否自动换行（大按钮常用）。
     * \note 会被 RibbonGroup::wordWrap（面板级批量）覆盖；组级只影响当时
     * 已存在的按钮，新加入的按钮不继承组级设置。
     */
    void wordWrap(bool on);
    bool wordWrap() const;
    /**
     * \brief 本按钮文字是否显示在图标右侧。
     * \note 会被 RibbonGroup::iconRightText（面板级批量）覆盖。
     */
    void iconRightText(bool on);
    bool iconRightText() const;

    // ---- 下拉列表 ----
    /// 追加一个下拉项（自动构建 QMenu 并挂到按钮上，InstantPopup）。
    void addDropDownItem(RibbonAction* item);
    /// 移除一个下拉项（交还其 QAction 所有权）。
    void removeDropDownItem(RibbonAction* item);
    /// 清空所有下拉项（含分隔线）。
    void clearDropDownItems();
    /// 在下拉菜单末尾插入分隔线（不算作下拉项）。
    void addSeparator();
    /// 下拉项数量（不含分隔线）。
    size_t dropDownItemCount() const;
    /// 按下拉项索引获取项（不含分隔线；越界返回 nullptr）。
    RibbonAction* dropDownItemAt(size_t i) const;
    /// 全条目数（含分隔线），配合 removeDropDownEntryAt 使用。
    size_t dropDownEntryCount() const;
    /// 按全条目索引移除（项/分隔线统一计数，0-based；越界忽略）。
    void removeDropDownEntryAt(size_t i);

    // ---- 自定义数据 ----
    /// 绑定用户自定义数据指针（框架不管理其生命周期）。
    void setData(void* dptr);
    void* data() const;

  public:
    // ---- 事件（公开数据成员） ----
    /// 按钮被点击时触发。
    Event<RibbonButton, EventArgs> clicked;

  private:
    void rebuildMenu();

    struct Data;
    Data*       dptr();
    const Data* dptr() const;
};

V_APPFWGUI_NS_END
