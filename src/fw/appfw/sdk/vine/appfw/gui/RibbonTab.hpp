#pragma once

#include "Control.hpp"
#include "Gui.hpp"

V_APPFWGUI_NS_BEGIN

class RibbonGroup;

/**
 * \brief 功能区标签页（包装 SARibbonCategory）：一组面板（RibbonGroup）的容器。
 *
 * \note 本头文件不包含、不暴露 Qt 类型。
 */
class V_APPFW_API RibbonTab : public Control {
    V_OBJECT_META_DECL

  public:
    RibbonTab();
    virtual ~RibbonTab();

  public:
    // ---- 标题 ----
    void title(const String& t);
    String title() const;

  public:
    // ---- 组 ----
    void addGroup(RibbonGroup* g);
    void removeGroup(RibbonGroup* g);
    /// 组数量。
    int numGroups() const;
    /// 按索引获取组（越界返回 nullptr）。
    RibbonGroup* groupAt(int i) const;

  public:
    // ---- 面板外观（作用于本页所有面板）----
    /// 设置本页所有面板的布局模式（三行/两行/单行）。
    void panelLayoutMode(RibbonPanelLayoutMode m);
    RibbonPanelLayoutMode panelLayoutMode() const;
    /// 显示/隐藏本页所有面板标题。
    void panelTitleVisible(bool on);
    bool panelTitleVisible() const;
    /// 本页所有面板的按钮间距。
    void panelSpacing(int n);
    int panelSpacing() const;

  private:
    struct Data;
    Data*       dptr();
    const Data* dptr() const;
};

V_APPFWGUI_NS_END
