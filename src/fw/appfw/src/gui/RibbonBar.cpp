#include <vine/appfw/gui/RibbonBar.hpp>

#include <SARibbon.h>
#include <vine/Exception.hpp>
#include <vine/Ptr.hpp>
#include <vine/appfw/gui/MainWindow.hpp>
#include <vine/appfw/gui/RibbonDropDownItem.hpp>
#include <vine/appfw/gui/RibbonTab.hpp>

#include "UIElementData.hpp"

V_APPFWGUI_NS_BEGIN

V_OBJECT_META_IMPL(RibbonBar, UIElement)

struct RibbonBar::Data : public UIElementData {
    std::vector<RibbonTab*> tabs;
    MainWindow*             wnd;
    QMenu*                  application_menu = nullptr;

    ~Data()
    {
        delete application_menu;
    }
};

namespace
{

using itype = SARibbonBar;

}

inline auto RibbonBar::dptr() -> Data* { return static_cast<Data*>(UIElement::d); }
inline auto RibbonBar::dptr() const -> const Data* { return static_cast<const Data*>(UIElement::d); }

RibbonBar::RibbonBar(MainWindow* wnd)
    : UIElement(new RibbonBar::Data(), static_cast<SARibbonMainWindow*>(wnd->impl())->ribbonBar())
{
    dptr()->owns_impl = false;             // SARibbonBar is owned by SARibbonMainWindow
    dptr()->wnd       = wnd;
    dptr()->application_menu = new QMenu();
    auto app_btn = qobject_cast<SARibbonApplicationButton*>(impl<itype>()->applicationButton());
    app_btn->setMenu(dptr()->application_menu);
}

RibbonBar::~RibbonBar()
{
    // d is deleted by UIElement::~UIElement(), do NOT delete here
}

int RibbonBar::numTabs() const
{ return (int)dptr()->tabs.size(); }

RibbonTab* RibbonBar::tabAt(int idx) const
{ return dptr()->tabs.at(idx); }

void RibbonBar::addTab(RibbonTab* tab)
{
    V_CHECK_NULL_THROW(tab)
    if (std::any_of(dptr()->tabs.begin(), dptr()->tabs.end(), [tab](RibbonTab* t) { return tab == t; }))
        return;
    auto w = impl<itype>();
    w->addCategoryPage(tab->impl<SARibbonCategory>());
    dptr()->tabs.push_back(tab);
}

void RibbonBar::removeTab(RibbonTab* tab)
{
    V_CHECK_NULL_THROW(tab)
    if (std::none_of(dptr()->tabs.begin(), dptr()->tabs.end(), [tab](RibbonTab* t) { return t == tab; }))
        return;
    auto w = impl<itype>();
    w->removeCategory(tab->impl<SARibbonCategory>());
    dptr()->tabs.erase(std::remove(dptr()->tabs.begin(), dptr()->tabs.end(), tab), dptr()->tabs.end());
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
{ dptr()->application_menu->addAction(mi->impl<QAction>()); }

V_APPFWGUI_NS_END
