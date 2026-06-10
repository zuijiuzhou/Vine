#include <vine/appfw/gui/UIElement.hpp>

#include <QAction>

#include <vine/appfw/gui/UIElementData.hpp>


V_APPFWGUI_NS_BEGIN

V_OBJECT_META_IMPL(UIElement, Object)

namespace
{} // namespace

UIElement::UIElement(UIElementData* data, QObject* impl)
  : d(data)
{
    d->impl    = impl;

    d->impl_destroyed_connection = QObject::connect(impl, &QObject::destroyed, [this, data](QObject* obj) {
        data->impl         = nullptr;
        data->impl_deleted = true;
    });
}

UIElement::UIElement(UIObject* impl)
    : d(new UIElementData())
{
        d->impl = impl;
        UIElementData* dptr = d;
        d->impl_destroyed_connection = QObject::connect(impl, &QObject::destroyed, [this, dptr](QObject* obj) {
                dptr->impl         = nullptr;
                dptr->impl_deleted = true;
        });
}

UIElement::~UIElement()
{
    if (d->impl) {
        QObject::disconnect(d->impl_destroyed_connection);
        delete d->impl;
    }
    delete d;
}

String UIElement::getName() const
{ return d->name; }

void UIElement::setName(const String& name)
{ d->name = name; }

QObject* UIElement::impl() const
{ return d->impl; }

V_APPFWGUI_NS_END
