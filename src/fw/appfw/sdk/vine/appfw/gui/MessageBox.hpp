#pragma once

#include <vine/appfw/gui/Window.hpp>

V_APPFWGUI_NS_BEGIN

/**
 * @brief Modal message box dialog (information / warning / critical /
 * question).
 *
 * Window-based wrapper around QMessageBox: titles, texts and other content use
 * framework types, and the buttons are chosen from the framework's own Button
 * presets instead of Qt enums. A box can be built and shown directly through
 * the instance API, or shown through the synchronous static helpers:
 *
 * @code
 * MessageBox box(parent);
 * box.setWindowTitle(u8"删除");
 * box.setText(u8"确定删除该对象？");
 * box.setIcon(MessageBox::Icon::Question);
 * box.setButtons(MessageBox::Button::YesNoCancel);
 * if (box.exec() == MessageBox::Button::Yes) {
 *     // ...
 * }
 *
 * MessageBox::warning(this, u8"名称无效", u8"名称不能为空。");
 * @endcode
 *
 * The native box must be created and shown on the GUI thread. The static
 * helpers may be called from any thread; they dispatch to the GUI thread and
 * block until the box has been handled.
 */
class V_APPFW_API MessageBox : public Window {
    V_OBJECT_META_DECL;

  public:
    /// Buttons shown in the box: a single button or a preset combination.
    enum class Button {
        Ok,          ///< OK.
        Cancel,      ///< Cancel.
        OkCancel,    ///< OK and Cancel.
        Yes,         ///< Yes.
        No,          ///< No.
        YesNo,       ///< Yes and No.
        YesNoCancel, ///< Yes, No and Cancel.
    };

    /// Icon displayed in the box.
    enum class Icon {
        None,        ///< No icon.
        Information, ///< Information icon.
        Warning,     ///< Warning icon.
        Critical,    ///< Critical icon.
        Question,    ///< Question icon.
    };

  public:
    /**
     * @brief Constructs an empty message box.
     *
     * Configure the box with the setters before showing it with exec() or
     * show().
     *
     * @param parent Parent widget; may be null for a top-level box.
     */
    explicit MessageBox(QWidget* parent = nullptr);
    ~MessageBox() override;

  public:
    /// Message text shown in the box.
    void   setText(const String& text);
    String text() const;
    /// Icon displayed in the box.
    void setIcon(Icon icon);
    Icon icon() const;
    /// Which buttons the box offers (a single preset).
    void   setButtons(Button buttons);
    Button buttons() const;
    /// Single button activated by pressing Enter.
    void setDefaultButton(Button button);
    /// Whether an explicit default button has been set.
    bool hasDefaultButton() const;

  public:
    /**
     * @brief Runs the box modally and reports the clicked button.
     *
     * @return The button the user clicked; a box dismissed without a button is
     * reported as Cancel.
     */
    Button exec();

  public:
    /**
     * @brief Shows an information box.
     *
     * @param parent  Parent widget; may be null for a top-level box.
     * @param title   Window title; an empty title falls back to a default.
     * @param text    Message text (line breaks allowed).
     * @param buttons Buttons to show; defaults to Ok.
     * @return The button the user clicked.
     */
    static Button information(QWidget* parent, const String& title, const String& text,
                              Button buttons = Button::Ok);

    /**
     * @brief Shows a warning box.
     *
     * @param parent  Parent widget; may be null for a top-level box.
     * @param title   Window title; an empty title falls back to a default.
     * @param text    Warning text (line breaks allowed).
     * @param buttons Buttons to show; defaults to Ok.
     * @return The button the user clicked.
     */
    static Button warning(QWidget* parent, const String& title, const String& text,
                          Button buttons = Button::Ok);

    /**
     * @brief Shows a critical error box.
     *
     * @param parent  Parent widget; may be null for a top-level box.
     * @param title   Window title; an empty title falls back to a default.
     * @param text    Error text (line breaks allowed).
     * @param buttons Buttons to show; defaults to Ok.
     * @return The button the user clicked.
     */
    static Button critical(QWidget* parent, const String& title, const String& text,
                           Button buttons = Button::Ok);

    /**
     * @brief Shows a question box.
     *
     * @param parent         Parent widget; may be null for a top-level box.
     * @param title          Window title; an empty title falls back to a default.
     * @param text           Question text (line breaks allowed).
     * @param buttons        Buttons to show; defaults to YesNo.
     * @param default_button Button activated by pressing Enter; defaults to No.
     * @return The button the user clicked.
     */
    static Button question(QWidget* parent, const String& title, const String& text,
                           Button buttons = Button::YesNo,
                           Button default_button = Button::No);

  private:
    void apply();

    struct Impl;
    Impl*       dptr();
    const Impl* dptr() const;
};

V_APPFWGUI_NS_END
