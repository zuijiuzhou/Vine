#include <vine/appfw/gui/RibbonDropDownItem.hpp>

#include <QAction>
#include "UIElementData.hpp"

V_APPFWGUI_NS_BEGIN

V_OBJECT_META_IMPL(RibbonDropDownItem, UIElement)

struct RibbonDropDownItem::Data : public UIElementData {
    void* user = nullptr;
};

inline auto RibbonDropDownItem::dptr() -> Data* { return static_cast<Data*>(UIElement::d); }
inline auto RibbonDropDownItem::dptr() const -> const Data* { return static_cast<const Data*>(UIElement::d); }

RibbonDropDownItem::RibbonDropDownItem()
    : UIElement(new Data(), new QAction(nullptr))
{
}

RibbonDropDownItem::~RibbonDropDownItem()
{
    // d is deleted by UIElement
}

void RibbonDropDownItem::text(const String& t)
{
    auto* act = impl<QAction>();
    if (!act) return;
    auto utf16 = t.toUtf16();
    act->setText(QString::fromStdU16String(utf16));
}

String RibbonDropDownItem::text() const
{
    auto* act = impl<QAction>();
    if (!act) return {};
    auto qs = act->text();
    return String::fromUtf16((const char16_t*)qs.utf16(), qs.size());
}

void RibbonDropDownItem::setData(void* data)
{
    dptr()->user = data;
}

void* RibbonDropDownItem::data() const
{
    return dptr()->user;
}

V_APPFWGUI_NS_END
