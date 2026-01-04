#include <vine/appfw/gui/RibbonBar.hpp>

#include <SARibbon.h>
#include <vine/appfw/gui/MainWindow.hpp>
#include <vine/appfw/gui/RibbonDropDownItem.hpp>
#include <vine/appfw/gui/RibbonTab.hpp>
#include <vine/Exception.hpp>
#include <vine/Ptr.hpp>

VI_APPFWGUI_NS_BEGIN

VI_OBJECT_META_IMPL(RibbonBar, Widget)

struct RibbonBar::Data {
    std::vector<RefPtr<RibbonTab>> tabs;
    RefPtr<MainWindow>             wnd;
    QMenu*                         application_menu = nullptr;
};

namespace {
using itype = SARibbonBar;
}

RibbonBar::RibbonBar(MainWindow* wnd)
  : Widget(static_cast<SARibbonMainWindow*>(wnd->impl())->ribbonBar())
  , d(new Data()) {
    d->wnd              = wnd;
    d->application_menu = new QMenu();
    auto app_btn        = qobject_cast<SARibbonApplicationButton*>(impl<itype>()->applicationButton());
    app_btn->setMenu(d->application_menu);
}

RibbonBar::~RibbonBar() {
    delete d;
}

int RibbonBar::numTabs() const {
    return d->tabs.size();
}

RibbonTab* RibbonBar::tabAt(int idx) const {
    return d->tabs.at(idx).get();
}

void RibbonBar::addTab(RibbonTab* tab) {
    VI_CHECK_NULL_THROW(tab)
    if (std::any_of(d->tabs.begin(), d->tabs.end(), [tab](RefPtr<RibbonTab>& t) { return tab == t; })) return;
    auto w = impl<itype>();
    w->addCategoryPage(tab->impl<SARibbonCategory>());
}

void RibbonBar::removeTab(RibbonTab* tab) {
    VI_CHECK_NULL_THROW(tab)
    if (std::none_of(d->tabs.begin(), d->tabs.end(), [tab](RefPtr<RibbonTab>& t) { return t == tab; })) return;
    auto w = impl<itype>();
    w->removeCategory(tab->impl<SARibbonCategory>());
}

int RibbonBar::currentIndex() {
    auto w = impl<itype>();
    return w->currentIndex();
}

void RibbonBar::currentIndex(int idx) {
    auto w = impl<itype>();
    w->setCurrentIndex(idx);
}

void RibbonBar::appendApplicationMenu(RibbonDropDownItem* mi) {
    d->application_menu->addAction(mi->impl<QAction>());
}

VI_APPFWGUI_NS_END