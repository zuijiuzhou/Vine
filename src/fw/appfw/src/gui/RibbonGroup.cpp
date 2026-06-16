#include <vine/appfw/gui/RibbonGroup.hpp>

#include <SARibbon.h>
#include <QToolButton>
#include <vine/appfw/gui/RibbonButton.hpp>

#include "UIElementData.hpp"

V_APPFWGUI_NS_BEGIN

V_OBJECT_META_IMPL(RibbonGroup, UIElement)

struct RibbonGroup::Data : public UIElementData {
    String title;
};

inline auto RibbonGroup::dptr() -> Data* { return static_cast<Data*>(UIElement::d); }
inline auto RibbonGroup::dptr() const -> const Data* { return static_cast<const Data*>(UIElement::d); }

RibbonGroup::RibbonGroup()
    : UIElement(new Data(), new SARibbonPanel())
{
}

RibbonGroup::~RibbonGroup()
{
    // d is deleted by UIElement
}

void RibbonGroup::title(const String& t)
{
    dptr()->title = t;
    auto* pnl = impl<SARibbonPanel>();
    if (pnl) {
        auto utf16 = t.toUtf16();
        pnl->setPanelName(QString::fromUtf16(reinterpret_cast<const ushort*>(utf16.data()), (int)utf16.size()));
    }
}

String RibbonGroup::title() const
{
    return dptr()->title;
}

void RibbonGroup::addButton(RibbonButton* b)
{
    if (!b) return;
    auto w = b->impl<QWidget>();
    auto* pnl = impl<SARibbonPanel>();
    if (w && pnl)
        pnl->addWidget(w, SARibbonPanelItem::Small);
}

void RibbonGroup::removeButton(RibbonButton* b)
{
    if (!b) return;
    auto* pnl = impl<SARibbonPanel>();
    if (!pnl) return;
    auto* w = b->impl<QWidget>();
    if (w) {
        auto* action = w->findChild<QAction*>(QString(), Qt::FindDirectChildrenOnly);
        if (!action) {
            auto* tb = qobject_cast<QToolButton*>(w);
            if (tb)
                action = tb->defaultAction();
        }
        if (action)
            pnl->removeAction(action);
        w->setParent(nullptr);
        w->deleteLater();
    }
}

V_APPFWGUI_NS_END
