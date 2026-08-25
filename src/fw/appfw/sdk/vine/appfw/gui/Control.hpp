#pragma once

#include "Gui.hpp"
#include "UIElement.hpp"

class QWidget;

V_APPFWGUI_NS_BEGIN

/**
 * \brief 通用控件容器：包装任意原生 QWidget，使其能被框架的 UIElement 体系容纳。
 *
 * 用于把第三方/自定义控件（QComboBox、QLineEdit、自绘控件等）挂到功能区组、
 * 停靠面板等任何接受 UIElement 的地方；也是 RibbonButton/RibbonGroup/RibbonTab/
 * RibbonBar/DockPanel/StatusBar 等控件包装的公共基类，统一提供 QWidget 级通用属性。
 * \note 本头文件仅前置声明 QWidget，不包含 Qt 头；owns=true（默认）时由本容器
 * 持有原生控件；控件被宿主（如 SARibbonPanel）接管后，宿主销毁控件时会自动
 * 触发本容器释放。
 */
class V_APPFW_API Control : public UIElement {
    V_OBJECT_META_DECL

  public:
    explicit Control(QWidget* native, bool owns = true);
    virtual ~Control();

  public:
    // ---- 通用控件属性（基于 impl<QWidget>()）----
    /// 是否可用。
    void enabled(bool on);
    bool enabled() const;
    /// 是否可见。
    void visible(bool on);
    bool visible() const;
    /// 悬停提示文本。
    void tooltip(const String& t);
    String tooltip() const;
    /// 控件宽度。
    int width() const;
    /// 控件高度。
    int height() const;
    /// 控件尺寸。
    Size size() const;
    void size(const Size& s);

  protected:
    // 供派生类（RibbonButton 等）传入自定义 Data，维持继承链扩展。
    Control(UIElementData* data, QWidget* native, bool owns = true);

  private:
    struct Data;
    Data*       dptr();
    const Data* dptr() const;
};

V_APPFWGUI_NS_END
