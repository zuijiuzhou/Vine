#include <vine/appfw/gui/MainWindow.hpp>

#include <QApplication>
#include <QDockWidget>
#include <QGuiApplication>
#include <QPalette>
#include <QSize>
#include <QStyleHints>
#include <QTimer>
#include <SARibbon.h>

// 旧方案（nativeEvent 监听 Windows 消息）需要的头文件，暂时禁用
#if 0
#    ifdef Q_OS_WIN
#        ifndef WIN32_LEAN_AND_MEAN
#            define WIN32_LEAN_AND_MEAN
#        endif
#        ifndef NOMINMAX
#            define NOMINMAX
#        endif
#        include <windows.h>
#        include <dwmapi.h>
#    endif
#endif

#include <vine/Ptr.hpp>
#include <vine/appfw/gui/DockPanel.hpp>
#include <vine/appfw/gui/DockPanelManager.hpp>
#include <vine/appfw/gui/Gui.hpp>
#include <vine/appfw/gui/RibbonBar.hpp>
#include <vine/appfw/gui/StatusBar.hpp>

#include "MainWindowImpl.hpp"
#include "UIElementData.hpp"

V_APPFWGUI_NS_BEGIN

V_OBJECT_META_IMPL(MainWindow, UIElement)

struct MainWindow::Data : public UIElementData {
    RibbonBar*        ribbon_bar = nullptr;
    StatusBar*        status_bar = nullptr;
    StartupPosition   startup_posi;
    WindowState       wnd_state;
    DockPanelManager* dock_panel_mgr = nullptr;
};

namespace
{

using itype = MainWindowImpl;

} // namespace

MainWindowImpl::MainWindowImpl(QWidget* parent)
    : SARibbonMainWindow(parent)
{
    // 跟随系统亮/暗模式切换（Qt 内部处理 Windows 主题变化消息）
    QObject::connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged,
                     this, [this](Qt::ColorScheme) { scheduleThemeUpdate(); });

    // SARibbon applies themes through QSS; applying a theme inside the constructor
    // does not take full effect, so defer it to the end of the event queue.
    scheduleThemeUpdate();
}

void MainWindowImpl::applyWindowsTheme()
{
    const bool dark = SA::isOperatingSystemInDarkMode();
    if (theme_applied_ && dark == dark_)
        return;
    dark_          = dark;
    theme_applied_ = true;

    // 自定义黑白主题：基于内置主题，再把 Ribbon 背景覆盖为窗口背景色，与 dock pane 一致
    applyCustomTheme(dark ? SARibbonTheme::RibbonThemeDark
                          : SARibbonTheme::RibbonThemeOffice2013);
}

void MainWindowImpl::applyCustomTheme(SARibbonTheme theme)
{
    setRibbonTheme(theme);

    // 内置模板用 {{white}} 画背景，与 dock pane 的窗口背景色不一致；
    // 追加覆盖这些控件的背景色，使 Ribbon 与 dock pane 保持一致
    const QColor bg = QApplication::palette().color(QPalette::Window);
    setStyleSheet(styleSheet() + QStringLiteral(
        "SARibbonBar { background-color: %1; }\n"
        "SARibbonCategory { background-color: %1; }\n"
        "SARibbonPanel { background-color: %1; }\n"
        "SARibbonToolButton { background-color: %1; }\n"
        "SARibbonTabBar::tab:selected, SARibbonTabBar::tab:hover:!selected {"
        " background: %1; border-bottom: 1px solid %1; }\n").arg(bg.name()));
}

void MainWindowImpl::scheduleThemeUpdate()
{
    if (update_pending_)
        return;
    update_pending_ = true;
    QTimer::singleShot(0, this, [this] {
        update_pending_ = false;
        applyWindowsTheme();
    });
}

// 旧方案：通过 nativeEvent 监听 Windows 主题变化消息，暂时禁用
#if 0
bool MainWindowImpl::nativeEvent(const QByteArray& eventType, void* message, qintptr* result)
{
#ifdef Q_OS_WIN
    if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG") {
        const MSG* msg = static_cast<const MSG*>(message);
        if (msg != nullptr) {
            switch (msg->message) {
            case WM_SETTINGCHANGE:                // light/dark mode or accent color changed
            case WM_THEMECHANGED:                 // system theme changed
            case WM_DWMCOLORIZATIONCOLORCHANGED:  // accent color changed
                scheduleThemeUpdate();
                break;
            default:
                break;
            }
        }
    }
#endif
    return SARibbonMainWindow::nativeEvent(eventType, message, result);
}
#endif

inline auto MainWindow::dptr() -> Data*
{ return static_cast<Data*>(UIElement::d); }

inline auto MainWindow::dptr() const -> const Data*
{ return static_cast<const Data*>(UIElement::d); }

MainWindow::MainWindow()
  : UIElement(new Data(), new MainWindowImpl(nullptr))
{
    dptr()->ribbon_bar     = new RibbonBar(this);
    dptr()->status_bar     = new StatusBar(this);
    dptr()->dock_panel_mgr = new DockPanelManager();
    dptr()->dock_panel_mgr->setWindow(this);

    impl<itype>()->setWindowTitle("Vine");
    impl<itype>()->setMinimumSize(QSize(800, 600));
    impl<itype>()->setCentralWidget(
        static_cast<QWidget*>(dptr()->dock_panel_mgr->root()->impl()));
    impl<itype>()->setStatusBar(dptr()->status_bar->impl<QStatusBar>());

    dptr()->status_bar->setOwnsImpl(true); // QMainWindow takes ownership of status bar

    auto ribbon = impl<itype>()->ribbonBar();
    ribbon->setContentsMargins(5, 0, 5, 0);
    ribbon->applicationButton()->setText("File");
}

MainWindow::~MainWindow()
{
    delete dptr()->dock_panel_mgr;
    delete dptr()->ribbon_bar;
    delete dptr()->status_bar;
    // d is deleted by UIElement
}

void MainWindow::startupPosition(StartupPosition position)
{ dptr()->startup_posi = position; }

StartupPosition MainWindow::startupPosition() const
{ return dptr()->startup_posi; }

void MainWindow::windowState(WindowState state)
{
    dptr()->wnd_state = state;
    Qt::WindowState qstate;
    if (state == WindowState::Minimized)
        qstate = Qt::WindowState::WindowMinimized;
    else if (state == WindowState::Maximized)
        qstate = Qt::WindowState::WindowMaximized;
    else
        qstate = Qt::WindowState::WindowNoState;
    impl<itype>()->setWindowState(qstate);
}

WindowState MainWindow::windowState() const
{
    WindowState state;
    auto        qstate = impl<itype>()->windowState();
    if (qstate & Qt::WindowState::WindowFullScreen)
        state = WindowState::Maximized;
    else if (qstate & Qt::WindowState::WindowMaximized)
        state = WindowState::Maximized;
    else if (qstate & Qt::WindowState::WindowMinimized)
        state = WindowState::Minimized;
    else
        state = WindowState::Normal;
    return state;
}

void MainWindow::activate()
{ impl<itype>()->activateWindow(); }

void MainWindow::setEnabled()
{ impl<itype>()->setEnabled(true); }

void MainWindow::setDisabled()
{ impl<itype>()->setEnabled(false); }

bool MainWindow::isActive() const
{ return impl<itype>()->isActiveWindow(); }

bool MainWindow::isEnabled() const
{ return impl<itype>()->isEnabled(); }

void MainWindow::show()
{ impl<itype>()->show(); }

void MainWindow::close()
{ impl<itype>()->close(); }

RibbonBar* MainWindow::ribbonBar() const
{ return dptr()->ribbon_bar; }

StatusBar* MainWindow::statusBar() const
{ return dptr()->status_bar; }

DockPanelManager* MainWindow::dockPanelManager() const
{ return dptr()->dock_panel_mgr; }

V_APPFWGUI_NS_END
