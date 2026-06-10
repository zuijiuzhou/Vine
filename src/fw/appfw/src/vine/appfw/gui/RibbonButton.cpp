#include <vine/appfw/gui/RibbonButton.hpp>

#include <QToolButton>

V_APPFWGUI_NS_BEGIN

V_OBJECT_META_IMPL(RibbonButton, UIElement)

struct RibbonButton::Data {
    QToolButton* btn = nullptr;
    void*        user = nullptr;
};

RibbonButton::RibbonButton()
  : UIElement(new QToolButton())
  , d(new Data)
{
    d->btn = impl<QToolButton>();
}

RibbonButton::~RibbonButton()
{
    delete d;
}

void RibbonButton::text(const String& t)
{
    auto utf16 = t.toUtf16();
    d->btn->setText(QString::fromUtf16(reinterpret_cast<const ushort*>(utf16.data()), (int)utf16.size()));
}

String RibbonButton::text() const
{
    auto qs = d->btn->text();
    return String::fromUtf16((const char16_t*)qs.utf16(), qs.size());
}

void RibbonButton::setData(void* dptr)
{
    d->user = dptr;
}

void* RibbonButton::data() const
{
    return d->user;
}

V_APPFWGUI_NS_END
