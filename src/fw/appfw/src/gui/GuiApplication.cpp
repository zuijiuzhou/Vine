#include <vine/appfw/gui/GuiApplication.hpp>

#include <QApplication>
#include <QGuiApplication>
#include <QPalette>
#include <QStyleHints>

#if defined(Q_OS_WIN) && QT_VERSION < QT_VERSION_CHECK(6, 5, 0)
#    include <QSettings>
#endif

#include "GuiApplicationData.hpp"

#if defined(Q_OS_WIN) && QT_VERSION < QT_VERSION_CHECK(6, 5, 0)
namespace
{

// Qt < 6.5 does not read the Windows colour scheme, so query it directly.
bool isSystemDarkMode(void)
{
    QSettings settings(QStringLiteral("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize"), QSettings::NativeFormat);

    return settings.value(QStringLiteral("AppsUseLightTheme"), 1).toInt() == 0;
}

} // namespace
#endif

#if defined(Q_OS_LINUX)
namespace
{

// DockingPanes moves top-level windows and reads the global cursor position,
// neither of which the Wayland platform plugin supports. WSLg exposes both
// DISPLAY and WAYLAND_DISPLAY and this Qt build prefers Wayland, so force
// X11/XWayland when running under WSL unless the user chose a platform.
void selectX11UnderWslg(void)
{
    if (qEnvironmentVariableIsSet("QT_QPA_PLATFORM")) {
        return;
    }

    if (qEnvironmentVariableIsSet("WSL_DISTRO_NAME") && qEnvironmentVariableIsSet("DISPLAY")) {
        qputenv("QT_QPA_PLATFORM", "xcb");
    }
}

} // namespace
#endif

V_APPFWGUI_NS_BEGIN

V_OBJECT_META_IMPL(GuiApplication, Application)

namespace
{

// Classic Fusion dark palette (matches the Qt >= 6.5 Fusion dark palette).
QPalette createDarkPalette(void)
{
    QPalette pal;
    pal.setColor(QPalette::Window, QColor(53, 53, 53));
    pal.setColor(QPalette::WindowText, Qt::white);
    pal.setColor(QPalette::Base, QColor(25, 25, 25));
    pal.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    pal.setColor(QPalette::ToolTipBase, QColor(53, 53, 53));
    pal.setColor(QPalette::ToolTipText, Qt::white);
    pal.setColor(QPalette::Text, Qt::white);
    pal.setColor(QPalette::Button, QColor(53, 53, 53));
    pal.setColor(QPalette::ButtonText, Qt::white);
    pal.setColor(QPalette::BrightText, Qt::red);
    pal.setColor(QPalette::Link, QColor(42, 130, 218));
    pal.setColor(QPalette::Highlight, QColor(42, 130, 218));
    pal.setColor(QPalette::HighlightedText, Qt::black);
    return pal;
}

// Classic Fusion dark palette (matches the Qt >= 6.5 Fusion dark palette).
QPalette createLightPalette(void)
{
    // 注意: Qt >= 6.5 的 Fusion 标准调色板会跟随系统亮/暗配色,
    // 不能直接用 style()->standardPalette() 当浅色板, 必须显式定义
    QPalette pal;
    pal.setColor(QPalette::Window, QColor(239, 239, 239));
    pal.setColor(QPalette::WindowText, Qt::black);
    pal.setColor(QPalette::Base, Qt::white);
    pal.setColor(QPalette::AlternateBase, QColor(247, 247, 247));
    pal.setColor(QPalette::ToolTipBase, QColor(255, 255, 220));
    pal.setColor(QPalette::ToolTipText, Qt::black);
    pal.setColor(QPalette::Text, Qt::black);
    pal.setColor(QPalette::Button, QColor(239, 239, 239));
    pal.setColor(QPalette::ButtonText, Qt::black);
    pal.setColor(QPalette::BrightText, Qt::red);
    pal.setColor(QPalette::Link, QColor(0, 0, 255));
    pal.setColor(QPalette::Highlight, QColor(42, 130, 218));
    pal.setColor(QPalette::HighlightedText, Qt::white);
    pal.setColor(QPalette::Mid, QColor(160, 160, 160));
    return pal;
}

// 将“系统当前主题”解析为 Theme 枚举值
Theme resolveSystemTheme(void)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    return QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark ? Theme::Dark : Theme::Light;
#elif defined(Q_OS_WIN)
    return isSystemDarkMode() ? Theme::Dark : Theme::Light;
#else
    return Theme::Light;
#endif
}

} // namespace

GuiApplication::GuiApplication(int argc, char** argv)
  : Application(new GuiApplicationData(), argc, argv)
{}

GuiApplication::~GuiApplication()
{
    // d is deleted by Application::~Application()
}

void GuiApplication::init()
{
    auto* d = static_cast<GuiApplicationData*>(dptr());
    if (d->app != nullptr) {
        return;
    }

#if defined(Q_OS_LINUX)
    selectX11UnderWslg();
#endif

    // QCoreApplication keeps a reference to argc; the data it refers to
    // must stay valid for the whole application lifetime, so pass the
    // stored member instead of a local copy.
    d->app = new QApplication(d->argc, d->argv);

#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    // 跟随系统时监听系统主题变化并重新解析
    QObject::connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged, d->app, [this, d](Qt::ColorScheme) {
        if (d->follow_system) {
            setTheme(resolveSystemTheme());
        }
    });
#endif

    // 应用初始主题（跟随系统或固定）
    if (d->follow_system) {
        d->theme = resolveSystemTheme();
    }
    applyTheme(d->theme);
}

void GuiApplication::setTheme(Theme theme)
{
    auto* d = static_cast<GuiApplicationData*>(dptr());
    if (d->theme == theme) {
        return;
    }
    d->theme = theme;
    applyTheme(theme);
    themeChanged.trigger(theme);
}

Theme GuiApplication::theme() const
{
    const auto* d = static_cast<const GuiApplicationData*>(dptr());
    return d->theme;
}

void GuiApplication::setFollowSystemTheme(bool follow)
{
    auto* d = static_cast<GuiApplicationData*>(dptr());
    if (d->follow_system == follow) {
        return;
    }
    d->follow_system = follow;
    if (follow) {
        setTheme(resolveSystemTheme());
    }
}

bool GuiApplication::followSystemTheme() const
{
    const auto* d = static_cast<const GuiApplicationData*>(dptr());
    return d->follow_system;
}

void GuiApplication::applyTheme(Theme theme)
{
    const auto* d = static_cast<GuiApplicationData*>(dptr());
    if (d->app == nullptr) {
        return;
    }

    if (theme == Theme::Dark) {
        d->app->setPalette(createDarkPalette());
    }
    else {
        d->app->setPalette(createLightPalette());
    }
}

int GuiApplication::run()
{
    const auto* d = static_cast<GuiApplicationData*>(dptr());
    return d->app->exec();
}

V_APPFWGUI_NS_END
