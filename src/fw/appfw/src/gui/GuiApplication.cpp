#include <vine/appfw/gui/GuiApplication.hpp>

#include <QApplication>
#include <QPalette>

#if defined(Q_OS_WIN) && QT_VERSION < QT_VERSION_CHECK(6, 5, 0)
#include <QSettings>
#endif

#include "GuiApplicationData.hpp"

#if defined(Q_OS_WIN) && QT_VERSION < QT_VERSION_CHECK(6, 5, 0)
namespace
{

// Qt < 6.5 does not read the Windows colour scheme, so query it directly.
bool isSystemDarkMode(void)
{
    QSettings settings(QStringLiteral("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize"),
                       QSettings::NativeFormat);

    return settings.value(QStringLiteral("AppsUseLightTheme"), 1).toInt() == 0;
}

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
    if (d->app == nullptr) {
#if defined(Q_OS_LINUX)
        selectX11UnderWslg();
#endif

        // QCoreApplication keeps a reference to argc; the data it refers to
        // must stay valid for the whole application lifetime, so pass the
        // stored member instead of a local copy.
        d->app = new QApplication(d->argc, d->argv);

#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
        // Leave the colour scheme untouched: Qt reads the OS colour scheme
        // (e.g. Windows 11 dark mode) and its styles apply it automatically.
#elif defined(Q_OS_WIN)
        if (isSystemDarkMode()) {
            QApplication::setStyle(QStringLiteral("Fusion"));
            QApplication::setPalette(createDarkPalette());
        }
#endif
    }
}

int GuiApplication::run()
{
    auto* d = static_cast<GuiApplicationData*>(dptr());
    return d->app->exec();
}

V_APPFWGUI_NS_END
