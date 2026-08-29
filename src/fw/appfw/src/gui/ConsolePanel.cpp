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

struct ConsolePanel::Data : public UIElementData
{
    QPlainTextEdit* output = nullptr;
    InputLine*      input  = nullptr;

    ConsoleTheme theme = ConsoleTheme::dark();

    CommandHistory   history;
    CommandCompleter completer;

    std::vector<String> matches;
    String              completionBase;
    int                 matchIndex = -1;
};

ConsolePanel::ConsolePanel(QWidget* parent)
  : Control(new Data(), new QWidget(parent))
{
    auto* data = dptr();
    auto* root = impl<QWidget>();

    auto* layout = new QVBoxLayout(root);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    data->output = new QPlainTextEdit(root);
    data->output->setReadOnly(true);

    data->input = new InputLine(root);

    layout->addWidget(data->output);
    layout->addWidget(data->input);

    QObject::connect(data->input, &QLineEdit::returnPressed, root, [this] { onReturnPressed(); });
    data->input->onEscape      = [this] { escapePressed.trigger(); };
    data->input->onHistoryUp   = [this] { onHistoryUp(); };
    data->input->onHistoryDown = [this] { onHistoryDown(); };
    data->input->onTab         = [this] { onTab(); };
}

ConsolePanel::~ConsolePanel() = default;

void ConsolePanel::append(ConsoleMessageType type, const String& text)
{
    appendFormatted(type, text);
}

void ConsolePanel::clear()
{
    dptr()->output->clear();
}

void ConsolePanel::beginInput(const String& prompt)
{
    auto* data = dptr();
    appendFormatted(ConsoleMessageType::Prompt, prompt);
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

QColor ConsolePanel::colorFor(ConsoleMessageType type) const
{
    const auto* data = dptr();
    switch (type)
    {
    case ConsoleMessageType::Command:
        return toQColor(data->theme.command);
    case ConsoleMessageType::Prompt:
        return toQColor(data->theme.prompt);
    case ConsoleMessageType::Warning:
        return toQColor(data->theme.warning);
    case ConsoleMessageType::Error:
        return toQColor(data->theme.error);
    default:
        return toQColor(data->theme.normal);
    }
}

void ConsolePanel::appendFormatted(ConsoleMessageType type, const String& text)
{
    auto* output = dptr()->output;

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

void ConsolePanel::onReturnPressed()
{
    auto*       data = dptr();
    const String text = fromQString(data->input->text());
    data->input->clear();
    data->history.add(text);
    lineEntered.trigger(text);
}

void ConsolePanel::onHistoryUp()
{
    auto*       data    = dptr();
    const String current = fromQString(data->input->text());
    const String cmd     = data->history.previous(current);
    if (!cmd.empty())
    {
        data->input->setText(toQString(cmd));
    }
}

void ConsolePanel::onHistoryDown()
{
    auto*       data = dptr();
    const String cmd  = data->history.next();
    data->input->setText(toQString(cmd));
}

void ConsolePanel::onTab()
{
    auto*       data    = dptr();
    const String current = fromQString(data->input->text());

    if (data->matches.empty() || !current.startsWith(data->completionBase))
    {
        data->completionBase = current;
        data->matches        = data->completer.complete(current);
        data->matchIndex     = -1;
    }

    if (data->matches.empty())
    {
        return;
    }

    data->matchIndex = (data->matchIndex + 1) % static_cast<int>(data->matches.size());
    data->input->setText(toQString(data->matches[static_cast<size_t>(data->matchIndex)]));
}

inline auto ConsolePanel::dptr() -> Data*
{
    return static_cast<Data*>(UIElement::d);
}

inline auto ConsolePanel::dptr() const -> const Data*
{
    return static_cast<const Data*>(UIElement::d);
}

V_APPFWGUI_NS_END
