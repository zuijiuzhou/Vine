#include <vine/appfw/gui/RibbonGroup.hpp>

#include <SARibbon.h>

#include <vine/Exception.hpp>

V_APPFWGUI_NS_BEGIN

V_OBJECT_META_IMPL(RibbonGroup, Widget)

struct RibbonGroup::Data {};

namespace
{

using itype = SARibbonPanel;

}

RibbonGroup::RibbonGroup()
  : Widget(new SARibbonPanel())
  , d(new Data())
{}

RibbonGroup::~RibbonGroup()
{
    delete d;
}

String RibbonGroup::title() const
{
    auto w = impl<itype>();
    return String::fromUtf16(reinterpret_cast<const char16_t*>(w->panelName().constData()));
}

void RibbonGroup::title(const String& ti)
{
    auto w = impl<itype>();
    w->setPanelName(QString::fromUtf8(reinterpret_cast<const char*>(ti.data())));
}

V_APPFWGUI_NS_END
