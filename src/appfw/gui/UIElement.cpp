#include <appfw/gui/UIElement.h>

VINE_APPFWGUI_NS_BEGIN

VI_OBJECT_META_IMPL(UIElement, Object)

struct UIElement::Data
{
    String name;
};

UIElement::UIElement()
    : d(new Data())
{
    
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

VINE_APPFWGUI_NS_END