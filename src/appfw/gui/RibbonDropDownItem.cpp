#include <appfw/gui/RibbonDropDownItem.h>
#include <QAction>

VI_APPFWGUI_NS_BEGIN

VI_OBJECT_META_IMPL(RibbonDropDownItem, UIElement);

struct RibbonDropDownItem::Data
{
};

namespace
{
    using itype = QAction;
}

RibbonDropDownItem::RibbonDropDownItem()
    : UIElement(new QAction()), d(new Data())
{
}

RibbonDropDownItem::~RibbonDropDownItem()
{
    delete d;
}

String RibbonDropDownItem::text() const
{
    auto ac = impl<itype>();
    return String(ac->text().toStdU32String().data());
}

RibbonDropDownItem *RibbonDropDownItem::text(const String &txt)
{
    auto ac = impl<itype>();
    ac->setText(QString::fromStdU32String(txt.data()));
    return this;
}

void *RibbonDropDownItem::data() const
{
    auto ac = impl<itype>();
    return ac->data().data();
}

RibbonDropDownItem *RibbonDropDownItem::data(void *v)
{
    auto ac = impl<itype>();
    ac->setData(QVariant::fromValue(v));
    return this;
}

VI_APPFWGUI_NS_END
