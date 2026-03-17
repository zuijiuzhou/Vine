#include <vine/appfw/gui/RibbonDropDownItem.hpp>

#include <QAction>

V_APPFWGUI_NS_BEGIN

V_OBJECT_META_IMPL(RibbonDropDownItem, UIElement);

struct RibbonDropDownItem::Data {};

namespace
{

using itype = QAction;

}

RibbonDropDownItem::RibbonDropDownItem()
  : UIElement(new QAction())
  , d(new Data())
{}

RibbonDropDownItem::~RibbonDropDownItem()
{
    delete d;
}

String RibbonDropDownItem::text() const
{
    auto ac = impl<itype>();
    return String::fromUtf16(reinterpret_cast<const char16_t*>(ac->text().constData()));
}

void RibbonDropDownItem::text(const String& txt)
{
    auto ac = impl<itype>();
    ac->setText(QString::fromUtf8(reinterpret_cast<const char*>(txt.data())));
}

void* RibbonDropDownItem::data() const
{
    auto ac = impl<itype>();
    return ac->data().data();
}

void RibbonDropDownItem::data(void* v)
{
    auto ac = impl<itype>();
    ac->setData(QVariant::fromValue(v));
}

V_APPFWGUI_NS_END
