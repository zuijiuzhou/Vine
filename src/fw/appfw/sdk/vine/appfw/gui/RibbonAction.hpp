#pragma once

#include "UIElement.hpp"
#include "Icon.hpp"

V_APPFWGUI_NS_BEGIN

/**
 * \brief 功能区动作（包装 QAction）。
 *
 * 一条可执行的命令/动作：可挂到 RibbonButton 的下拉菜单、RibbonBar 的应用菜单，
 * 或作为 RibbonGroup 的选项按钮。
 * \note 本头文件不包含、不暴露 Qt 类型（图标经 Icon 桥接）；
 * 加入菜单后其 QAction 归菜单持有（见 RibbonButton 所有权说明）。
 */
class V_APPFW_API RibbonAction : public UIElement {
    V_OBJECT_META_DECL

  public:
    RibbonAction();
    virtual ~RibbonAction();

  public:
    // ---- 文本 ----
    /// 设置动作文本。
    void text(const String& t);
    /// 获取动作文本。
    String text() const;

    // ---- 图标 ----
    /// 设置动作图标（经 Icon 桥接，不暴露 Qt）。
    void icon(const Icon& ic);
    /// 获取动作图标。
    Icon icon() const;

    // ---- 常用 impl 方法封装 ----
    /// 设置悬停提示文本。
    void tooltip(const String& t);
    String tooltip() const;
    /// 设置动作是否可用。
    void enabled(bool on);
    bool enabled() const;
    /// 设置动作是否可选中（切换态）。
    void checkable(bool on);
    bool checkable() const;
    /// 设置动作是否处于选中态（需先 checkable(true)）。
    void checked(bool on);
    bool checked() const;

  public:
    // ---- 自定义数据 ----
    /// 绑定用户自定义数据指针（框架不管理其生命周期）。
    void setData(void* dptr);
    void* data() const;

  private:
    struct Data;
    Data*       dptr();
    const Data* dptr() const;
};

V_APPFWGUI_NS_END
