#pragma once

#include <SARibbon.h>

V_APPFWGUI_NS_BEGIN

class MainWindowImpl : public SARibbonMainWindow {
  public:
    explicit MainWindowImpl(QWidget* parent = nullptr);

  public:
    void applyWindowsTheme();

  protected:
    // 旧方案：通过 nativeEvent 监听 Windows 主题变化消息，暂时禁用
    // bool nativeEvent(const QByteArray& eventType, void* message, qintptr* result) override;

  private:
    void scheduleThemeUpdate();
    void applyCustomTheme(SARibbonTheme theme);

    bool dark_           = false;
    bool theme_applied_  = false;
    bool update_pending_ = false;
};

V_APPFWGUI_NS_END
