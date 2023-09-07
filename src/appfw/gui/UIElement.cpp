#include <appfw/gui/UIElement.h>

VI_APPFWGUI_NS_BEGIN

VI_OBJECT_META_IMPL(UIElement, Object)

struct UIElement::Data
{
    String name;
    void* impl;
};

UIElement::UIElement(void* impl)
    : d(new Data())
{
    d->impl = impl;
}

String UIElement::name() const
{
    return d->name;
}

UIElement *UIElement::name(const String &name)
{
    d->name = name;
    return this;
}

void* UIElement::impl() const{
    return d->impl;
}

VI_APPFWGUI_NS_END