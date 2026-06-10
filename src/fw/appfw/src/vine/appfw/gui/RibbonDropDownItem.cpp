#include <vine/appfw/gui/RibbonDropDownItem.hpp>

#include <QAction>

V_APPFWGUI_NS_BEGIN

V_OBJECT_META_IMPL(RibbonDropDownItem, UIElement)

struct RibbonDropDownItem::Data {
    QAction* act = nullptr;
    void*    user = nullptr;
};

RibbonDropDownItem::RibbonDropDownItem()
  : UIElement(new QAction(nullptr))
  , d(new Data)
{
    d->act = impl<QAction>();
}

RibbonDropDownItem::~RibbonDropDownItem()
{
    delete d;
}

void RibbonDropDownItem::text(const String& t)
{
    auto utf16 = t.toUtf16();
    d->act->setText(QString::fromUtf16(reinterpret_cast<const ushort*>(utf16.data()), (int)utf16.size()));
}

String RibbonDropDownItem::text() const
{
    auto qs = d->act->text();
    return String::fromUtf16((const char16_t*)qs.utf16(), qs.size());
}

void RibbonDropDownItem::setData(void* dptr)
{
    d->user = dptr;
}

void* RibbonDropDownItem::data() const
{
    return d->user;
}

V_APPFWGUI_NS_END
