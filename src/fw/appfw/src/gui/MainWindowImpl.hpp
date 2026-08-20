#pragma once

#include <SARibbon.h>

#include <vine/Signal.hpp>
#include <vine/appfw/gui/GuiApplication.hpp>

V_APPFWGUI_NS_BEGIN

class MainWindowImpl : public SARibbonMainWindow {
  public:
    explicit MainWindowImpl(QWidget* parent = nullptr);
    ~MainWindowImpl() override;

  public:
    void applyAppTheme();

  private:
    Signal<Theme>::HandlerId theme_handler_id_{};
};

V_APPFWGUI_NS_END
