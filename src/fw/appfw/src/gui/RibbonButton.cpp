#include <vine/appfw/gui/RibbonButton.hpp>

#include <SARibbon.h>
#include "UIElementData.hpp"

V_APPFWGUI_NS_BEGIN

V_OBJECT_META_IMPL(RibbonButton, UIElement)

struct RibbonButton::Data : public UIElementData {
    void* user = nullptr;
};

inline auto RibbonButton::dptr() -> Data* { return static_cast<Data*>(UIElement::d); }
inline auto RibbonButton::dptr() const -> const Data* { return static_cast<const Data*>(UIElement::d); }

RibbonButton::RibbonButton()
    : UIElement(new Data(), new SARibbonToolButton(static_cast<QWidget*>(nullptr)))
{
}

RibbonButton::~RibbonButton()
{
    // d is deleted by UIElement
}

void RibbonButton::text(const String& t)
{
    auto* btn = impl<SARibbonToolButton>();
    if (!btn) return;
    auto utf16 = t.toUtf16();
    btn->setText(QString::fromUtf16(reinterpret_cast<const ushort*>(utf16.data()), (int)utf16.size()));
}

String RibbonButton::text() const
{
    auto* btn = impl<SARibbonToolButton>();
    if (!btn) return {};
    auto qs = btn->text();
    return String::fromUtf16((const char16_t*)qs.utf16(), qs.size());
}

void RibbonButton::setData(void* data)
{
    dptr()->user = data;
}

void* RibbonButton::data() const
{
    return dptr()->user;
}

V_APPFWGUI_NS_END
