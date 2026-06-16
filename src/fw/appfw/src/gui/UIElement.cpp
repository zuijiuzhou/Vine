#include <vine/appfw/gui/UIElement.hpp>

#include <QAction>

#include "UIElementData.hpp"


V_APPFWGUI_NS_BEGIN

V_OBJECT_META_IMPL(UIElement, Object)

namespace
{} // namespace

UIElement::UIElement(UIElementData* data, QObject* impl)
  : d(data)
{
    d->impl    = impl;

    d->impl_destroyed_connection = QObject::connect(impl, &QObject::destroyed, [this, data](QObject* obj) {
        data->impl         = nullptr;   // prevent dtor from double-deleting
        data->impl_deleted = true;
        if (data->owns_impl) {
            delete this;                // self-destruct when impl goes away
        }
    });
}

UIElement::UIElement(UIObject* impl)
    : d(new UIElementData())
{
        d->impl = impl;
        UIElementData* dptr = d;
        d->impl_destroyed_connection = QObject::connect(impl, &QObject::destroyed, [this, dptr](QObject* obj) {
                dptr->impl         = nullptr;   // prevent dtor from double-deleting
                dptr->impl_deleted = true;
                if (dptr->owns_impl) {
                    delete this;                // self-destruct when impl goes away
                }
        });
}

UIElement::~UIElement()
{
    if (d->impl) {
        QObject::disconnect(d->impl_destroyed_connection);
        if (d->owns_impl) {
            delete d->impl;
        }
    }
    delete d;
}

String UIElement::getName() const
{ return d->name; }

void UIElement::setName(const String& name)
{ d->name = name; }

QObject* UIElement::impl() const
{ return d->impl; }

void UIElement::setOwnsImpl(bool owns)
{
    d->owns_impl = owns;
}

V_APPFWGUI_NS_END
