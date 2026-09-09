#include <vine/appfw/gui/MessageBox.hpp>

#include <QCoreApplication>
#include <QMessageBox>
#include <QThread>

#include <functional>

#include <vine/appfw/gui/UIElementData.hpp>

#include "Convert.hpp"

V_APPFWGUI_NS_BEGIN

namespace
{

/**
 * @brief Maps a framework icon to the Qt message box icon.
 *
 * @param icon Framework icon to map.
 * @return The equivalent Qt message box icon.
 */
QMessageBox::Icon toQtIcon(MessageBox::Icon icon)
{
    switch (icon) {
    case MessageBox::Icon::Information: return QMessageBox::Information;
    case MessageBox::Icon::Warning: return QMessageBox::Warning;
    case MessageBox::Icon::Critical: return QMessageBox::Critical;
    case MessageBox::Icon::Question: return QMessageBox::Question;
    case MessageBox::Icon::None: break;
    }
    return QMessageBox::NoIcon;
}

/**
 * @brief Maps a framework button preset to the Qt standard buttons.
 *
 * @param buttons Framework button preset to map.
 * @return The equivalent Qt standard buttons.
 */
QMessageBox::StandardButtons toQtButtons(MessageBox::Button buttons)
{
    switch (buttons) {
    case MessageBox::Button::Ok: return QMessageBox::Ok;
    case MessageBox::Button::Cancel: return QMessageBox::Cancel;
    case MessageBox::Button::OkCancel: return QMessageBox::Ok | QMessageBox::Cancel;
    case MessageBox::Button::Yes: return QMessageBox::Yes;
    case MessageBox::Button::No: return QMessageBox::No;
    case MessageBox::Button::YesNo: return QMessageBox::Yes | QMessageBox::No;
    case MessageBox::Button::YesNoCancel:
        return QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel;
    }
    return QMessageBox::NoButton;
}

/**
 * @brief Maps a single framework button to the Qt default button.
 *
 * Preset combinations are not valid default buttons and map to NoButton.
 *
 * @param button Framework button to map.
 * @return The equivalent Qt standard button, or NoButton for combinations.
 */
QMessageBox::StandardButton toQtDefaultButton(MessageBox::Button button)
{
    switch (button) {
    case MessageBox::Button::Ok: return QMessageBox::Ok;
    case MessageBox::Button::Cancel: return QMessageBox::Cancel;
    case MessageBox::Button::Yes: return QMessageBox::Yes;
    case MessageBox::Button::No: return QMessageBox::No;
    default: break;
    }
    return QMessageBox::NoButton;
}

/**
 * @brief Maps a clicked Qt standard button back to the framework button.
 *
 * A box dismissed without clicking a button (or an unknown result) maps to
 * Cancel so callers never mistake it for an affirmative answer.
 *
 * @param qbutton Qt standard button that was clicked.
 * @return The equivalent framework button.
 */
MessageBox::Button toVineButton(QMessageBox::StandardButton qbutton)
{
    switch (qbutton) {
    case QMessageBox::Ok: return MessageBox::Button::Ok;
    case QMessageBox::Cancel: return MessageBox::Button::Cancel;
    case QMessageBox::Yes: return MessageBox::Button::Yes;
    case QMessageBox::No: return MessageBox::Button::No;
    default: break;
    }
    return MessageBox::Button::Cancel;
}

/**
 * @brief Runs a function on the GUI thread and blocks until it has finished.
 *
 * When the caller is already on the GUI thread the function runs directly;
 * otherwise it is queued to the GUI thread and the caller blocks until the
 * function has been handled.
 *
 * @param fn Function to run on the GUI thread.
 * @return The value produced by the function; Cancel when no application
 * exists.
 */
MessageBox::Button runOnGuiThread(const std::function<MessageBox::Button()>& fn)
{
    QCoreApplication* app = QCoreApplication::instance();
    if (!app) {
        return MessageBox::Button::Cancel;
    }
    if (QThread::currentThread() == app->thread()) {
        return fn();
    }
    MessageBox::Button result = MessageBox::Button::Cancel;
    QMetaObject::invokeMethod(app, [&result, &fn] { result = fn(); }, Qt::BlockingQueuedConnection);
    return result;
}

/**
 * @brief Builds and runs a message box on the GUI thread.
 *
 * @param parent        Parent widget; may be null.
 * @param title         Window title of the box.
 * @param text          Message text shown in the box.
 * @param icon          Icon displayed in the box.
 * @param buttons       Buttons offered by the box.
 * @param default_button Default button; only applied when has_default is true.
 * @param has_default   Whether an explicit default button was requested.
 * @return The button the user clicked.
 */
MessageBox::Button runBox(QWidget* parent, const String& title, const String& text,
                          MessageBox::Icon icon, MessageBox::Button buttons,
                          MessageBox::Button default_button, bool has_default)
{
    return runOnGuiThread([parent, title, text, icon, buttons, default_button, has_default] {
        MessageBox box(parent);
        box.setWindowTitle(title);
        box.setText(text);
        box.setIcon(icon);
        box.setButtons(buttons);
        if (has_default) {
            box.setDefaultButton(default_button);
        }
        return box.exec();
    });
}

} // namespace

V_OBJECT_META_IMPL(MessageBox, Window)

struct MessageBox::Impl : public UIElementData {
    String text;
    Icon   icon            = Icon::None;
    Button buttons         = Button::Ok;
    bool   has_default_btn = false;
    Button default_button  = Button::No;
};

MessageBox::MessageBox(QWidget* parent)
  : Window(new Impl(), new QMessageBox(parent))
{
    apply();
}

MessageBox::~MessageBox()
{
    // d is released by UIElement.
}

void MessageBox::setText(const String& text)
{
    dptr()->text = text;
    apply();
}

String MessageBox::text() const
{
    return dptr()->text;
}

void MessageBox::setIcon(Icon icon)
{
    dptr()->icon = icon;
    apply();
}

MessageBox::Icon MessageBox::icon() const
{
    return dptr()->icon;
}

void MessageBox::setButtons(Button buttons)
{
    dptr()->buttons = buttons;
    apply();
}

MessageBox::Button MessageBox::buttons() const
{
    return dptr()->buttons;
}

void MessageBox::setDefaultButton(Button button)
{
    auto* data      = dptr();
    data->has_default_btn = true;
    data->default_button  = button;
    apply();
}

bool MessageBox::hasDefaultButton() const
{
    return dptr()->has_default_btn;
}

MessageBox::Button MessageBox::exec()
{
    apply();
    auto* native = impl<QMessageBox>();
    return toVineButton(static_cast<QMessageBox::StandardButton>(native->exec()));
}

void MessageBox::apply()
{
    auto*  data   = dptr();
    auto*  native = impl<QMessageBox>();
    native->setText(Convert::toQString(data->text));
    native->setIcon(toQtIcon(data->icon));
    native->setStandardButtons(toQtButtons(data->buttons));
    native->setDefaultButton(data->has_default_btn ? toQtDefaultButton(data->default_button) : QMessageBox::NoButton);
}

MessageBox::Button MessageBox::information(QWidget* parent, const String& title, const String& text,
                                           Button buttons)
{
    return runBox(parent, title.empty() ? String(u8"提示") : title, text,
                  Icon::Information, buttons, Button::No, false);
}

MessageBox::Button MessageBox::warning(QWidget* parent, const String& title, const String& text,
                                       Button buttons)
{
    return runBox(parent, title.empty() ? String(u8"警告") : title, text,
                  Icon::Warning, buttons, Button::No, false);
}

MessageBox::Button MessageBox::critical(QWidget* parent, const String& title, const String& text,
                                        Button buttons)
{
    return runBox(parent, title.empty() ? String(u8"错误") : title, text,
                  Icon::Critical, buttons, Button::No, false);
}

MessageBox::Button MessageBox::question(QWidget* parent, const String& title, const String& text,
                                        Button buttons, Button default_button)
{
    return runBox(parent, title.empty() ? String(u8"请确认") : title, text,
                  Icon::Question, buttons, default_button, true);
}

inline auto MessageBox::dptr() -> Impl*
{
    return static_cast<Impl*>(UIElement::d);
}

inline auto MessageBox::dptr() const -> const Impl*
{
    return static_cast<const Impl*>(UIElement::d);
}

V_APPFWGUI_NS_END
