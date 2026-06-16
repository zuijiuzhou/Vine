#include <vine/appfw/gui/StatusBar.hpp>

#include <QStatusBar>
#include "UIElementData.hpp"

V_APPFWGUI_NS_BEGIN

V_OBJECT_META_IMPL(StatusBar, UIElement)

struct StatusBar::Data : public UIElementData {};

inline auto StatusBar::dptr() -> Data* { return static_cast<Data*>(UIElement::d); }
inline auto StatusBar::dptr() const -> const Data* { return static_cast<const Data*>(UIElement::d); }

StatusBar::StatusBar()
    : UIElement(new Data(), new QStatusBar())
{
}

StatusBar::StatusBar(UIElement* parent)
    : UIElement(new Data(), new QStatusBar())
{
    // Attach to the parent widget if provided
    if (parent) {
        auto* pw = static_cast<QWidget*>(parent->impl());
        auto* sb = impl<QStatusBar>();
        if (pw && sb)
            sb->setParent(pw);
    }
}

StatusBar::~StatusBar()
{
    // d is deleted by UIElement
}

void StatusBar::showMessage(const String& msg, int timeout_ms)
{
    auto* sb = impl<QStatusBar>();
    if (!sb) return;
    auto utf16 = msg.toUtf16();
    sb->showMessage(QString::fromUtf16(reinterpret_cast<const ushort*>(utf16.data()), (int)utf16.size()), timeout_ms);
}

V_APPFWGUI_NS_END
