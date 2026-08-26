#include <vine/appfw/gui/Window.hpp>

#include <QDialog>
#include <QWidget>

#include "UIElementData.hpp"

V_APPFWGUI_NS_BEGIN

namespace
{

QString toQString(const String& s)
{
    auto u16 = s.toUtf16();
    return QString::fromStdU16String(u16);
}

String fromQString(const QString& qs)
{
    return String::fromUtf16((const char16_t*)qs.utf16(), qs.size());
}

} // namespace

V_OBJECT_META_IMPL(Window, Control)

struct Window::Data : public UIElementData {};

Window::Window(QWidget* native, bool owns)
  : Control(new Data(), native, owns)
{}

Window::~Window()
{
    // d 由 UIElement 释放
}

void Window::windowTitle(const String& t)
{
    if (auto* w = impl<QWidget>())
        w->setWindowTitle(toQString(t));
}

String Window::windowTitle() const
{
    auto* w = impl<QWidget>();
    return w ? fromQString(w->windowTitle()) : String();
}

void Window::modal(bool on)
{
    if (auto* w = impl<QWidget>())
        w->setWindowModality(on ? Qt::WindowModal : Qt::NonModal);
}

bool Window::modal() const
{
    auto* w = impl<QWidget>();
    return w && w->windowModality() != Qt::NonModal;
}

void Window::show()
{
    if (auto* w = impl<QWidget>())
        w->show();
}

void Window::close()
{
    if (auto* w = impl<QWidget>())
        w->close();
}

int Window::exec()
{
    auto* w   = impl<QWidget>();
    auto* dlg = qobject_cast<QDialog*>(w);
    return dlg ? dlg->exec() : 0;
}

void Window::resize(int w, int h)
{
    if (auto* widget = impl<QWidget>())
        widget->resize(w, h);
}

void Window::windowState(WindowState state)
{
    auto* w = impl<QWidget>();
    if (!w)
        return;
    Qt::WindowState qstate;
    if (state == WindowState::Minimized)
        qstate = Qt::WindowState::WindowMinimized;
    else if (state == WindowState::Maximized)
        qstate = Qt::WindowState::WindowMaximized;
    else
        qstate = Qt::WindowState::WindowNoState;
    w->setWindowState(qstate);
}

WindowState Window::windowState() const
{
    auto*       w     = impl<QWidget>();
    WindowState state = WindowState::Normal;
    if (!w)
        return state;
    const auto qstate = w->windowState();
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

void Window::startupPosition(StartupPosition position)
{
    if (auto* w = impl<QWidget>())
        w->setProperty("vine.startupPosition", static_cast<int>(position));
}

StartupPosition Window::startupPosition() const
{
    auto* w = impl<QWidget>();
    if (!w)
        return StartupPosition::Manual;
    return static_cast<StartupPosition>(w->property("vine.startupPosition").toInt());
}

void Window::activate()
{
    if (auto* w = impl<QWidget>())
        w->activateWindow();
}

bool Window::isActive() const
{
    auto* w = impl<QWidget>();
    return w && w->isActiveWindow();
}

Window::Window(UIElementData* data, QWidget* native, bool owns)
  : Control(data, native, owns)
{}

inline auto Window::dptr() -> Data*
{
    return static_cast<Data*>(UIElement::d);
}

inline auto Window::dptr() const -> const Data*
{
    return static_cast<const Data*>(UIElement::d);
}

V_APPFWGUI_NS_END
