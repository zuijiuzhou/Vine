#include <appfw/gui/RibbonGroup.h>
#include <SARibbon.h>
#include <core/Exception.h>

VI_APPFWGUI_NS_BEGIN

VI_OBJECT_META_IMPL(RibbonGroup, Control)

struct RibbonGroup::Data
{
};

namespace
{
    using itype = SARibbonPannel;
}

RibbonGroup::RibbonGroup()
    : Control(new SARibbonPannel()), d(new Data())
{
}

RibbonGroup::~RibbonGroup(){
    delete d;
}

String RibbonGroup::title() const
{
    auto w = impl<itype>();
    String s(w->pannelName().toStdU32String().data());
    return s;
}

RibbonGroup *RibbonGroup::title(const String &ti)
{
    auto w = impl<itype>();
    w->setPannelName(QString::fromUcs4(ti.data()));
    return this;
}

VI_APPFWGUI_NS_END