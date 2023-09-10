#include <appfw/gui/UIElement.h>

#include <QAction>

VI_APPFWGUI_NS_BEGIN

VI_OBJECT_META_IMPL(UIElement, Object)

struct UIElement::Data
{
    String name;
    QObject *impl;
    QMetaObject::Connection impl_destroyed_connection;
};

namespace
{
    void ImplDestroyed(QObject *obj)
    {
    }
}

UIElement::UIElement(QObject *impl)
    : d(new Data())
{
    d->impl = impl;
    Data *dptr = d;
    d->impl_destroyed_connection = QObject::connect(
        impl, &QObject::destroyed, [this, dptr](QObject *obj)
        { 
            dptr->impl = nullptr; 
            delete this; 
        }
    );
}

UIElement::~UIElement()
{
    if (d->impl)
    {
        QObject::disconnect(d->impl_destroyed_connection);
        delete d->impl;
    }
    delete d;
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

QObject *UIElement::impl() const
{
    return d->impl;
}

VI_APPFWGUI_NS_END