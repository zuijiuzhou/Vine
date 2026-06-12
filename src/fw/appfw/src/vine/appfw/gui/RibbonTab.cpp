#include <vine/appfw/gui/RibbonTab.hpp>

#include <SARibbon.h>
#include <vine/appfw/gui/RibbonGroup.hpp>
#include <vine/appfw/gui/UIElementData.hpp>

V_APPFWGUI_NS_BEGIN

V_OBJECT_META_IMPL(RibbonTab, UIElement)

struct RibbonTab::Data : public UIElementData {
    String title;
};

inline auto RibbonTab::dptr() -> Data* { return static_cast<Data*>(UIElement::d); }
inline auto RibbonTab::dptr() const -> const Data* { return static_cast<const Data*>(UIElement::d); }

RibbonTab::RibbonTab()
    : UIElement(new Data(), new SARibbonCategory(QString()))
{
}

RibbonTab::~RibbonTab()
{
    // d is deleted by UIElement
}

void RibbonTab::title(const String& t)
{
    dptr()->title = t;
    auto* cat = impl<SARibbonCategory>();
    if (cat) {
        auto utf16 = t.toUtf16();
        cat->setCategoryName(QString::fromUtf16(reinterpret_cast<const ushort*>(utf16.data()), (int)utf16.size()));
    }
}

String RibbonTab::title() const
{
    return dptr()->title;
}

void RibbonTab::addGroup(RibbonGroup* g)
{
    if (!g) return;
    auto p = g->impl<SARibbonPanel>();
    auto* cat = impl<SARibbonCategory>();
    if (p && cat)
        cat->addPanel(p);
}

void RibbonTab::removeGroup(RibbonGroup* g)
{
    if (!g) return;
    auto p = g->impl<SARibbonPanel>();
    auto* cat = impl<SARibbonCategory>();
    if (p && cat)
        cat->removePanel(p);
}

V_APPFWGUI_NS_END
