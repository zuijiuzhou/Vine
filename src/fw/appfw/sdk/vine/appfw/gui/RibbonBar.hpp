#pragma once

#include "Control.hpp"
#include "Icon.hpp"
#include "Gui.hpp"

V_APPFWGUI_NS_BEGIN

class RibbonTab;
class RibbonAction;
class MainWindow;

/**
 * \brief 功能区条（包装 SARibbonBar）：管理标签页与应用菜单。
 *
 * \note 本头文件不包含、不暴露 Qt 类型。
 */
class V_APPFW_API RibbonBar : public Control {
    V_OBJECT_META_DECL

    friend class MainWindow;

  public:
    RibbonBar(MainWindow* wnd);
    virtual ~RibbonBar();

  public:
    // ---- 标签页 ----
    int        numTabs() const;
    RibbonTab* tabAt(int idx) const;
    void       addTab(RibbonTab* tab);
    void       removeTab(RibbonTab* tab);
    int        currentIndex() const;
    void       currentIndex(int idx);

    // ---- 应用按钮 / 菜单 ----
    /// 追加一个应用菜单项（左上角 File 按钮）。
    void appendApplicationMenu(RibbonAction* mi);
    /// 应用按钮是否可见。
    void applicationButtonVisible(bool on);
    bool applicationButtonVisible() const;
    /// 应用按钮图标。
    void applicationIcon(const Icon& ic);
    Icon applicationIcon() const;
    /// 应用按钮文字。
    void applicationText(const String& t);
    String applicationText() const;

  public:
    // ---- 标题栏快捷访问栏（Quick Access Bar）----
    /// 追加一个快捷动作（标题栏左侧；其 QAction 归快捷栏持有）。
    void addQuickAccessItem(RibbonAction* item);
    /// 在快捷访问栏插入分隔线。
    void addQuickAccessSeparator();
    /// 快捷访问栏是否可见。
    void quickAccessVisible(bool on);
    bool quickAccessVisible() const;

  public:
    // ---- 全局风格 ----
    /// 设置 Ribbon 全局风格（行数 × 宽松/紧凑）。
    void ribbonStyle(RibbonStyle s);
    RibbonStyle ribbonStyle() const;
    /// 折叠/展开 Ribbon（最小化模式）。
    void minimumMode(bool on);
    bool minimumMode() const;
    /// 全局显示/隐藏所有面板标题。
    void panelTitleVisible(bool on);
    bool panelTitleVisible() const;
    /**
     * \brief 全局批量：按钮文字自动换行。
     * \note 仅对 RibbonButton（SARibbonToolButton）生效；addControl 添加的
     * 普通控件不受影响。级联到所有面板/按钮，会覆盖组级/按钮级 wordWrap。
     */
    void wordWrap(bool on);
    bool wordWrap() const;
    /**
     * \brief 全局批量：文字放图标右侧。
     * \note 仅对 RibbonButton（SARibbonToolButton）生效；addControl 添加的
     * 普通控件不受影响。级联到所有面板/按钮，会覆盖组级/按钮级 iconRightText。
     */
    void iconRightText(bool on);
    bool iconRightText() const;

  private:
    struct Data;
    Data*       dptr();
    const Data* dptr() const;
};

V_APPFWGUI_NS_END
