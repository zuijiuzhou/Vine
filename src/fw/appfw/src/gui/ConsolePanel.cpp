#include <vine/appfw/gui/ConsolePanel.hpp>

#include <functional>

#include "CommandCompleter.hpp"
#include "CommandHistory.hpp"

#include <vine/appfw/gui/UIElementData.hpp>

#include <QColor>
#include <QFont>
#include <QKeyEvent>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QVBoxLayout>
#include <QWidget>

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

QColor toQColor(const Color& c)
{
    return QColor(c.r, c.g, c.b, c.a);
}

/**
 * @brief Single-line input routing Up/Down/Tab/Esc to the panel.
 */
class InputLine : public QLineEdit
{
  public:
    using QLineEdit::QLineEdit;

    std::function<void()> onEscape;
    std::function<void()> onHistoryUp;
    std::function<void()> onHistoryDown;
    std::function<void()> onTab;

  protected:
    void keyPressEvent(QKeyEvent* e) override
    {
        switch (e->key())
        {
        case Qt::Key_Escape:
            if (onEscape)
            {
                onEscape();
            }
            return;
        case Qt::Key_Up:
            if (onHistoryUp)
            {
                onHistoryUp();
            }
            return;
        case Qt::Key_Down:
            if (onHistoryDown)
            {
                onHistoryDown();
            }
            return;
        case Qt::Key_Tab:
            if (onTab)
            {
                onTab();
            }
            return;
        default:
            break;
        }
        QLineEdit::keyPressEvent(e);
    }
};

} // namespace

V_OBJECT_META_IMPL(ConsolePanel, Control)

struct ConsolePanel::Impl : public UIElementData
{
    /// Owning panel, used by the input handlers to trigger signals.
    ConsolePanel*   panel  = nullptr;
    QPlainTextEdit* output = nullptr;
    InputLine*      input  = nullptr;

    ConsoleTheme theme = ConsoleTheme::dark();

    CommandHistory   history;
    CommandCompleter completer;

    std::vector<String> matches;
    String              completionBase;
    int                 matchIndex = -1;

    /**
     * @brief Resolves the display color for a message type from the theme.
     *
     * @param type Message type to resolve.
     * @return The color used to render that message type.
     */
    QColor colorFor(ConsoleMessageType type) const;

    void appendFormatted(ConsoleMessageType type, const String& text);
    void onEscape();
    void onReturnPressed();
    void onHistoryUp();
    void onHistoryDown();
    void onTab();
};

ConsolePanel::ConsolePanel(QWidget* parent)
  : Control(new Impl(), new QWidget(parent))
{
    auto* data = dptr();
    auto* root = impl<QWidget>();

    data->panel = this;

    auto* layout = new QVBoxLayout(root);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    data->output = new QPlainTextEdit(root);
    data->output->setReadOnly(true);

    data->input = new InputLine(root);

    layout->addWidget(data->output);
    layout->addWidget(data->input);

    QObject::connect(data->input, &QLineEdit::returnPressed, root, [data] { data->onReturnPressed(); });
    data->input->onEscape      = [data] { data->onEscape(); };
    data->input->onHistoryUp   = [data] { data->onHistoryUp(); };
    data->input->onHistoryDown = [data] { data->onHistoryDown(); };
    data->input->onTab         = [data] { data->onTab(); };
}

ConsolePanel::~ConsolePanel() = default;

void ConsolePanel::append(ConsoleMessageType type, const String& text)
{
    dptr()->appendFormatted(type, text);
}

void ConsolePanel::clear()
{
    dptr()->output->clear();
}

void ConsolePanel::beginInput(const String& prompt)
{
    auto* data = dptr();
    data->appendFormatted(ConsoleMessageType::Prompt, prompt);
    data->input->clear();
    data->input->setFocus();
}

void ConsolePanel::clearInput()
{
    dptr()->input->clear();
}

void ConsolePanel::setCommandNames(const std::vector<String>& names)
{
    auto* data = dptr();
    data->completer.setCommands(names);
    data->matches.clear();
    data->completionBase.clear();
    data->matchIndex = -1;
}

void ConsolePanel::setTheme(const ConsoleTheme& theme)
{
    dptr()->theme = theme;
}

const ConsoleTheme& ConsolePanel::theme() const
{
    return dptr()->theme;
}

QColor ConsolePanel::Impl::colorFor(ConsoleMessageType type) const
{
    switch (type)
    {
    case ConsoleMessageType::Command:
        return toQColor(theme.command);
    case ConsoleMessageType::Prompt:
        return toQColor(theme.prompt);
    case ConsoleMessageType::Warning:
        return toQColor(theme.warning);
    case ConsoleMessageType::Error:
        return toQColor(theme.error);
    default:
        return toQColor(theme.normal);
    }
}

void ConsolePanel::Impl::appendFormatted(ConsoleMessageType type, const String& text)
{
    QTextCharFormat fmt;
    fmt.setForeground(colorFor(type));
    if (type == ConsoleMessageType::Command || type == ConsoleMessageType::Prompt)
    {
        fmt.setFontWeight(QFont::Bold);
    }

    QTextCursor cursor(output->document());
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(toQString(text) + QStringLiteral("\n"), fmt);
    output->setTextCursor(cursor);
    output->ensureCursorVisible();
}

void ConsolePanel::Impl::onEscape()
{
    panel->escapePressed.trigger();
}

void ConsolePanel::Impl::onReturnPressed()
{
    const String text = fromQString(input->text());
    input->clear();
    history.add(text);
    panel->lineEntered.trigger(text);
}

void ConsolePanel::Impl::onHistoryUp()
{
    const String current = fromQString(input->text());
    const String cmd     = history.previous(current);
    if (!cmd.empty())
    {
        input->setText(toQString(cmd));
    }
}

void ConsolePanel::Impl::onHistoryDown()
{
    const String cmd = history.next();
    input->setText(toQString(cmd));
}

void ConsolePanel::Impl::onTab()
{
    const String current = fromQString(input->text());

    if (matches.empty() || !current.startsWith(completionBase))
    {
        completionBase = current;
        matches        = completer.complete(current);
        matchIndex     = -1;
    }

    if (matches.empty())
    {
        return;
    }

    matchIndex = (matchIndex + 1) % static_cast<int>(matches.size());
    input->setText(toQString(matches[static_cast<size_t>(matchIndex)]));
}

inline auto ConsolePanel::dptr() -> Impl*
{
    return static_cast<Impl*>(UIElement::d);
}

inline auto ConsolePanel::dptr() const -> const Impl*
{
    return static_cast<const Impl*>(UIElement::d);
}

V_APPFWGUI_NS_END
