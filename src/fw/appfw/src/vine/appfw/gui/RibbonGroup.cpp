#include <vine/appfw/gui/RibbonGroup.hpp>

#include <SARibbon.h>

#include <vine/Exception.hpp>

VI_APPFWGUI_NS_BEGIN

VI_OBJECT_META_IMPL(RibbonGroup, Control)

struct RibbonGroup::Data {};

namespace {
using itype = SARibbonPanel;
}

RibbonGroup::RibbonGroup()
  : Control(new SARibbonPanel())
  , d(new Data()) {
}

RibbonGroup::~RibbonGroup() {
    delete d;
}

String RibbonGroup::title() const {
    auto   w = impl<itype>();
    String s(w->panelName().toStdU32String().data());
    return s;
}

void RibbonGroup::title(const String& ti) {
    auto w = impl<itype>();
    w->setPanelName(QString::fromUcs4(ti.data()));
}

VI_APPFWGUI_NS_END