#include <vine/appfw/gui/RibbonTab.h>

#include <SARibbon.h>

#include <vine/appfw/gui/RibbonGroup.h>
#include <vine/core/Exception.h>
#include <vine/core/Ptr.h>

VI_APPFWGUI_NS_BEGIN

VI_OBJECT_META_IMPL(RibbonTab, Control)

struct RibbonTab::Data {
    std::vector<RefPtr<RibbonGroup>> groups;
};

namespace {
using itype = SARibbonCategory;
}

RibbonTab::RibbonTab()
  : Control(new SARibbonCategory())
  , d(new Data()) {
}

RibbonTab::~RibbonTab() {
    delete d;
}

String RibbonTab::title() const {
    auto   w = impl<itype>();
    String s(w->categoryName().toStdU32String().data());
    return s;
}

void RibbonTab::title(const String& ti) {
    auto w = impl<itype>();
    w->setCategoryName(QString::fromUcs4(ti.data()));
    this;
}

void RibbonTab::addGroup(RibbonGroup* group) {
    VI_CHECK_NULL_THROW(group)
    if (std::any_of(d->groups.begin(), d->groups.end(), [group](RefPtr<RibbonGroup>& g) { return g == group; })) return;
    auto w = impl<itype>();
    w->addPannel(group->impl<SARibbonPannel>());
    d->groups.push_back(group);
    this;
}

void RibbonTab::removeGroup(RibbonGroup* group) {
    VI_CHECK_NULL_THROW(group)
    if (std::none_of(d->groups.begin(), d->groups.end(), [group](RefPtr<RibbonGroup>& g) { return g == group; }))
        return;
    auto w = impl<itype>();
    w->removePannel(group->impl<SARibbonPannel>());
    this;
}

int RibbonTab::numGroups() const {
    return d->groups.size();
}

void RibbonTab::groupAt(int idx) const {
    d->groups.at(idx).get();
}

VI_APPFWGUI_NS_END