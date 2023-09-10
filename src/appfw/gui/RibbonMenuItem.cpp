#include <appfw/gui/RibbonMenuItem.h>
#include <QAction>

VI_APPFWGUI_NS_BEGIN

VI_OBJECT_META_IMPL(RibbonMenuItem, UIElement);

struct RibbonMenuItem::Data
{
};

namespace
{
    using itype = QAction;
}

RibbonMenuItem::RibbonMenuItem()
    : UIElement(new QAction()), d(new Data())
{
}

RibbonMenuItem::~RibbonMenuItem()
{
    delete d;
}

String RibbonMenuItem::text() const
{
    auto ac = impl<itype>();
    return String(ac->text().toStdU32String().data());
}

RibbonMenuItem *RibbonMenuItem::text(const String &txt)
{
    auto ac = impl<itype>();
    ac->setText(QString::fromStdU32String(txt.data()));
    return this;
}

void *RibbonMenuItem::data() const
{
    auto ac = impl<itype>();
    return ac->data().data();
}

RibbonMenuItem *RibbonMenuItem::data(void *v)
{
    auto ac = impl<itype>();
    ac->setData(QVariant::fromValue(v));
    return this;
}

VI_APPFWGUI_NS_END
