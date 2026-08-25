#pragma once

#include "Control.hpp"
#include "Gui.hpp"

V_APPFWGUI_NS_BEGIN

class RibbonButton;
class RibbonAction;
class Control;

/**
 * \brief 功能区组（包装 SARibbonPanel）：一组按钮/控件的容器。
 *
 * 支持标题、按钮增删、分隔线、布局模式（三行/两行/单行）、面板级批量
 * 图标尺寸与样式，以及右上角选项按钮。
 *
 * \note 本头文件不包含、不暴露 Qt 类型。面板级标题高度、间距等全局属性由
 * SARibbonBar 统一管理，此处不封装。
 */
class V_APPFW_API RibbonGroup : public Control {
    V_OBJECT_META_DECL

  public:
    RibbonGroup();
    virtual ~RibbonGroup();

  public:
    // ---- 标题 ----
    /// 设置组标题。
    void title(const String& t);
    /// 获取组标题。
    String title() const;

  public:
    // ---- 按钮 / 控件 ----
    /// 添加一个按钮（按按钮大小自动选行占比：大占整行 / 中占两行 / 小占一行）。
    void addButton(RibbonButton* b);
    /// 移除一个按钮（从面板摘除，底层控件延迟释放）。
    void removeButton(RibbonButton* b);
    /// 添加一个通用控件容器（任意原生控件，行占比同 addButton）。
    void addControl(Control* w, RibbonItemSize size);
    /// 移除一个通用控件容器。
    void removeControl(Control* w);
    /// 在按钮之间添加分隔线。
    void addSeparator();

  public:
    // ---- 布局 / 外观 ----
    /**
     * \brief 设置面板布局模式：三行 / 两行 / 单行。
     * \note 面板加入 RibbonBar 时，BAR 的默认布局模式会同步覆盖此前设置。
     */
    void layoutMode(RibbonPanelLayoutMode m);
    RibbonPanelLayoutMode layoutMode() const;
    /// 设置是否水平扩展（配合可扩展控件如 Gallery 使用）。
    void expanding(bool on);
    bool expanding() const;
    /// 设置是否允许用户右键自定义面板。
    void canCustomize(bool on);
    bool canCustomize() const;
    /// 面板级大按钮图标尺寸（批量统一设置）。
    void largeIconSize(const Size& s);
    Size largeIconSize() const;
    /// 面板级小按钮图标尺寸（批量统一设置）。
    void smallIconSize(const Size& s);
    Size smallIconSize() const;
    /**
     * \brief 面板级批量设置：文字是否放图标右侧。
     *
     * 会级联覆盖面板内所有按钮的 iconRightText 状态。
     * \note 仅对面板内的 RibbonButton 生效；addControl 添加的普通控件不受影响。
     * 与 RibbonButton::iconRightText 写的是同一个底层状态：组级调用会
     * 覆盖按钮级设置；之后对单个按钮的调用可微调，直到下一次组级调用。\n
     * 读取返回面板级标志（批量设置项），不反映单按钮的微调结果。
     */
    void iconRightText(bool on);
    bool iconRightText() const;
    /**
     * \brief 面板级批量设置：按钮文字是否自动换行。
     *
     * \note 仅对面板内的 RibbonButton 生效；addControl 添加的普通控件不受影响。
     * SARibbonPanel::setEnableWordWrap 是 protected（仅内部同步用），
     * 因此这里遍历面板内**已有**按钮逐个设置；**之后新加入的按钮不会继承**。
     * 组级调用会覆盖按钮级 wordWrap；读取返回本次设置（存于 Data）。
     */
    void wordWrap(bool on);
    bool wordWrap() const;

  public:
    // ---- 选项按钮（右上角）----
    /** 设置选项按钮；传 nullptr 清除。
     *  \note 面板不持有 QAction 所有权；传入的 item 被销毁时会自动清除按钮。 */
    void setOptionAction(RibbonAction* item);
    RibbonAction* optionAction() const;

  private:
    struct Data;
    Data*       dptr();
    const Data* dptr() const;
};

V_APPFWGUI_NS_END
