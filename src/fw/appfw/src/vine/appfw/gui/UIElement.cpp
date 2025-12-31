#include <vine/appfw/gui/UIElement.hpp>

#include <QAction>

VI_APPFWGUI_NS_BEGIN

VI_OBJECT_META_IMPL(UIElement, RefObject)

struct UIElement::Data {
    String                  name;
    QObject*                impl         = nullptr;
    bool                    impl_deleted = false;
    QMetaObject::Connection impl_destroyed_connection;
};

namespace {
void ImplDestroyed(QObject* obj) {
}
} // namespace

UIElement::UIElement(QObject* impl)
  : d(new Data()) {
    d->impl                      = impl;
    Data* dptr                   = d;
    d->impl_destroyed_connection = QObject::connect(impl, &QObject::destroyed, [this, dptr](QObject* obj) {
        dptr->impl         = nullptr;
        dptr->impl_deleted = true;
    });
}

UIElement::~UIElement() {
    if (d->impl) {
        QObject::disconnect(d->impl_destroyed_connection);
        delete d->impl;
    }
    delete d;
}

String UIElement::name() const {
    return d->name;
}

void UIElement::name(const String& name) {
    d->name = name;
}

QObject* UIElement::impl() const {
    return d->impl;
}

VI_APPFWGUI_NS_END