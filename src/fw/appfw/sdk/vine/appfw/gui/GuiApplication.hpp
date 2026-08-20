#pragma once

#include <vine/appfw/Application.hpp>

#include <vine/Signal.hpp>

V_APPFWGUI_NS_BEGIN

/**
 * @brief Application theme.
 */
enum class Theme
{
    Light, ///< Light theme.
    Dark   ///< Dark theme.
};

class V_APPFW_API GuiApplication : public Application {
    V_OBJECT_META_DECL
  public:
    GuiApplication(int argc, char** argv);
    ~GuiApplication() override;

  public:
    virtual void init() override;

  public:
    virtual int run() override;

  public:
    /**
     * @brief Set and apply the theme immediately.
     *
     * In follow-system mode the next system theme change takes precedence.
     */
    void setTheme(Theme theme);

    /**
     * @brief Get the currently effective theme.
     */
    Theme theme() const;

    /**
     * @brief Enable or disable following the system theme.
     * @param follow True to follow the system theme.
     */
    void setFollowSystemTheme(bool follow);

    /**
     * @brief Whether the system theme is being followed.
     */
    bool followSystemTheme() const;

  public:
    /**
     * @brief Emitted whenever the effective theme changes.
     *
     * Handlers are invoked on the GUI thread only; the parameter is the
     * newly effective theme.
     */
    Signal<Theme> themeChanged;

  private:
    void applyTheme(Theme theme);
};

V_APPFWGUI_NS_END
