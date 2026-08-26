#include <vine/appfw/gui/RibbonAction.hpp>

#include <QAction>
#include <QIcon>
#include "UIElementData.hpp"

V_APPFWGUI_NS_BEGIN

V_OBJECT_META_IMPL(RibbonAction, UIElement)

struct RibbonAction::Data : public UIElementData {
    void* user = nullptr;
};

RibbonAction::RibbonAction()
  : UIElement(new Data(), new QAction(nullptr))
{}

RibbonAction::~RibbonAction()
{
    // d is deleted by UIElement
}

void RibbonAction::text(const String& t)
{
    auto* act = impl<QAction>();
    if (!act)
        return;
    auto utf16 = t.toUtf16();
    act->setText(QString::fromStdU16String(utf16));
}

String RibbonAction::text() const
{
    auto* act = impl<QAction>();
    if (!act)
        return {};
    auto qs = act->text();
    return String::fromUtf16((const char16_t*)qs.utf16(), qs.size());
}

void RibbonAction::icon(const Icon& ic)
{
    auto* act = impl<QAction>();
    if (!act)
        return;
    act->setIcon(ic.value());
}

Icon RibbonAction::icon() const
{
    auto* act = impl<QAction>();
    if (!act)
        return {};
    return Icon(act->icon());
}

void RibbonAction::tooltip(const String& t)
{
    auto* act = impl<QAction>();
    if (!act)
        return;
    auto utf16 = t.toUtf16();
    act->setToolTip(QString::fromStdU16String(utf16));
}

String RibbonAction::tooltip() const
{
    auto* act = impl<QAction>();
    if (!act)
        return {};
    auto qs = act->toolTip();
    return String::fromUtf16((const char16_t*)qs.utf16(), qs.size());
}

void RibbonAction::enabled(bool on)
{
    auto* act = impl<QAction>();
    if (!act)
        return;
    act->setEnabled(on);
}

bool RibbonAction::enabled() const
{
    auto* act = impl<QAction>();
    return act && act->isEnabled();
}

void RibbonAction::checkable(bool on)
{
    auto* act = impl<QAction>();
    if (!act)
        return;
    act->setCheckable(on);
}

bool RibbonAction::checkable() const
{
    auto* act = impl<QAction>();
    return act && act->isCheckable();
}

void RibbonAction::checked(bool on)
{
    auto* act = impl<QAction>();
    if (!act)
        return;
    act->setChecked(on);
}

bool RibbonAction::checked() const
{
    auto* act = impl<QAction>();
    return act && act->isChecked();
}

void RibbonAction::setData(void* data)
{
    dptr()->user = data;
}

void* RibbonAction::data() const
{
    return dptr()->user;
}

inline auto RibbonAction::dptr() -> Data*
{
    return static_cast<Data*>(UIElement::d);
}

inline auto RibbonAction::dptr() const -> const Data*
{
    return static_cast<const Data*>(UIElement::d);
}

V_APPFWGUI_NS_END
