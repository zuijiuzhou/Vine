#include <appfw/gui/RibbonBar.h>

#include <SARibbon.h>
#include <core/Exception.h>
#include <appfw/gui/RibbonTab.h>
#include <appfw/gui/MainWindow.h>
#include <appfw/gui/RibbonDropDownItem.h>

VI_APPFWGUI_NS_BEGIN

VI_OBJECT_META_IMPL(RibbonBar, Control)

struct RibbonBar::Data
{
    std::vector<RefPtr<RibbonTab>> tabs;
    RefPtr<MainWindow> wnd;
    QMenu *application_menu = nullptr;
};

namespace
{
    using itype = SARibbonBar;
}

RibbonBar::RibbonBar(MainWindow *wnd)
    : Control(static_cast<SARibbonMainWindow *>(wnd->impl())->ribbonBar()), d(new Data())
{
    d->wnd = wnd;
    d->application_menu = new QMenu();
    auto app_btn = qobject_cast<SARibbonApplicationButton *>(impl<itype>()->applicationButton());
    app_btn->setMenu(d->application_menu);
}

RibbonBar::~RibbonBar()
{
    delete d;
}

Int32 RibbonBar::numTabs() const
{
    return d->tabs.size();
}

RibbonTab *RibbonBar::tabAt(Int32 idx) const
{
    return d->tabs.at(idx).get();
}

RibbonBar *RibbonBar::addTab(RibbonTab *tab)
{
    VI_CHECK_NULL(tab)
    if (std::any_of(d->tabs.begin(), d->tabs.end(), [tab](RefPtr<RibbonTab>& t)
                    { return  tab == t; }))
        return this;
    auto w = impl<itype>();
    w->addCategoryPage(tab->impl<SARibbonCategory>());
    return this;
}

RibbonBar *RibbonBar::removeTab(RibbonTab *tab)
{
    VI_CHECK_NULL(tab)
    if (std::none_of(d->tabs.begin(), d->tabs.end(), [tab](RefPtr<RibbonTab>& t)
                     { return t == tab; }))
        return this;
    auto w = impl<itype>();
    w->removeCategory(tab->impl<SARibbonCategory>());
    return this;
}

Int32 RibbonBar::currentIndex()
{
    auto w = impl<itype>();
    return w->currentIndex();
}

RibbonBar *RibbonBar::currentIndex(Int32 idx)
{
    auto w = impl<itype>();
    w->setCurrentIndex(idx);
    return this;
}

void RibbonBar::appendApplicationMenu(RibbonDropDownItem *mi)
{
    d->application_menu->addAction(mi->impl<QAction>());
}

VI_APPFWGUI_NS_END