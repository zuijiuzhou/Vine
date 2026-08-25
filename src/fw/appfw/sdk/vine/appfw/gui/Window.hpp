#pragma once

#include "Control.hpp"
#include "Gui.hpp"

V_APPFWGUI_NS_BEGIN

/**
 * \brief 顶层窗口基类：包装原生 QWidget（顶层窗口/对话框），提供窗口级能力。
 *
 * 继承 Control（enabled/visible/tooltip/size 等通用控件属性），并补充
 * 窗口语义：标题（windowTitle）、显示/关闭（show/close）、模态（modal）、
 * 尺寸（resize）。exec() 仅对原生为 QDialog 的窗口生效（模态运行）。
 * 供 ConfigWindow、MainWindow 等窗口类派生；派生类将内容构建到
 * impl<QWidget>() 中。
 */
class V_APPFW_API Window : public Control {
    V_OBJECT_META_DECL

  public:
    explicit Window(QWidget* native, bool owns = true);
    virtual ~Window();

  public:
    /// 窗口标题。
    void   windowTitle(const String& t);
    String windowTitle() const;
    /// 窗口模态（setWindowModality）。
    void modal(bool on);
    bool modal() const;
    /// 显示窗口（非模态）。
    void show();
    /// 关闭窗口。
    void close();
    /// 模态运行（阻塞至关闭），返回对话框结果码。
    int exec();
    /// 设置窗口尺寸（像素）。
    void resize(int w, int h);

  public:
    /// 窗口状态（最小化/最大化/正常）。
    void        windowState(WindowState state);
    WindowState windowState() const;
    /// 初始窗口位置（当前仅记录，show 时按需应用）。
    void            startupPosition(StartupPosition position);
    StartupPosition startupPosition() const;
    /// 激活窗口（置于前台）。
    void activate();
    /// 窗口是否处于激活状态。
    bool isActive() const;

  protected:
    // 派生类传入自定义 Data，维持继承链扩展。
    Window(UIElementData* data, QWidget* native, bool owns = true);

  private:
    struct Data;
    Data*       dptr();
    const Data* dptr() const;
};

V_APPFWGUI_NS_END
