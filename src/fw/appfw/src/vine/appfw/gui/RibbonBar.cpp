#include <vine/appfw/gui/RibbonBar.hpp>

#include <SARibbon.h>
#include <vine/Exception.hpp>
#include <vine/Ptr.hpp>
#include <vine/appfw/gui/MainWindow.hpp>
#include <vine/appfw/gui/RibbonDropDownItem.hpp>
#include <vine/appfw/gui/RibbonTab.hpp>
#include <vine/appfw/gui/UIElementData.hpp>

V_APPFWGUI_NS_BEGIN

V_OBJECT_META_IMPL(RibbonBar, UIElement)

struct RibbonBar::Data : public UIElementData {
    std::vector<RibbonTab*> tabs;
    MainWindow*             wnd;
    QMenu*                  application_menu = nullptr;
};

namespace
{

using itype = SARibbonBar;

}

RibbonBar::RibbonBar(MainWindow* wnd)
    : UIElement(new RibbonBar::Data(), static_cast<SARibbonMainWindow*>(wnd->impl())->ribbonBar())
    , d(static_cast<Data*>(UIElement::d))
{
    d->wnd              = wnd;
    d->application_menu = new QMenu();
    auto app_btn        = qobject_cast<SARibbonApplicationButton*>(impl<itype>()->applicationButton());
    app_btn->setMenu(d->application_menu);
}

RibbonBar::~RibbonBar()
{ delete d; }

int RibbonBar::numTabs() const
{ return (int)d->tabs.size(); }

RibbonTab* RibbonBar::tabAt(int idx) const
{ return d->tabs.at(idx); }

void RibbonBar::addTab(RibbonTab* tab)
{
    V_CHECK_NULL_THROW(tab)
    if (std::any_of(d->tabs.begin(), d->tabs.end(), [tab](RibbonTab* t) { return tab == t; }))
        return;
    auto w = impl<itype>();
    w->addCategoryPage(tab->impl<SARibbonCategory>());
    d->tabs.push_back(tab);
}

void RibbonBar::removeTab(RibbonTab* tab)
{
    V_CHECK_NULL_THROW(tab)
    if (std::none_of(d->tabs.begin(), d->tabs.end(), [tab](RibbonTab* t) { return t == tab; }))
        return;
    auto w = impl<itype>();
    w->removeCategory(tab->impl<SARibbonCategory>());
    d->tabs.erase(std::remove(d->tabs.begin(), d->tabs.end(), tab), d->tabs.end());
}

int RibbonBar::currentIndex()
{
    auto w = impl<itype>();
    return w->currentIndex();
}

void RibbonBar::currentIndex(int idx)
{
    auto w = impl<itype>();
    w->setCurrentIndex(idx);
}

void RibbonBar::appendApplicationMenu(RibbonDropDownItem* mi)
{ d->application_menu->addAction(mi->impl<QAction>()); }

V_APPFWGUI_NS_END
