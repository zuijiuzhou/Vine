#include <vine/appfw/gui/RibbonGroup.hpp>

#include <SARibbon.h>
#include <QToolButton>
#include <vine/appfw/gui/RibbonAction.hpp>
#include <vine/appfw/gui/RibbonButton.hpp>
#include <vine/appfw/gui/Control.hpp>

#include "UIElementData.hpp"

V_APPFWGUI_NS_BEGIN

V_OBJECT_META_IMPL(RibbonGroup, Control)

namespace
{

/// 条目尺寸 -> 面板行占比
SARibbonPanelItem::RowProportion rowProportionFor(RibbonItemSize s)
{
    switch (s) {
    case RibbonItemSize::Large:  return SARibbonPanelItem::Large;
    case RibbonItemSize::Medium: return SARibbonPanelItem::Medium;
    default:                     return SARibbonPanelItem::Small;
    }
}

SARibbonPanel::PanelLayoutMode toSarPanelLayoutMode(RibbonPanelLayoutMode m)
{
    switch (m) {
    case RibbonPanelLayoutMode::TwoRow:    return SARibbonPanel::TwoRowMode;
    case RibbonPanelLayoutMode::SingleRow: return SARibbonPanel::SingleRowMode;
    case RibbonPanelLayoutMode::ThreeRow:  break;
    }

    return SARibbonPanel::ThreeRowMode;
}

RibbonPanelLayoutMode fromSarPanelLayoutMode(SARibbonPanel::PanelLayoutMode m)
{
    switch (m) {
    case SARibbonPanel::TwoRowMode:    return RibbonPanelLayoutMode::TwoRow;
    case SARibbonPanel::SingleRowMode: return RibbonPanelLayoutMode::SingleRow;
    default:                           break;
    }

    return RibbonPanelLayoutMode::ThreeRow;
}

} // namespace

struct RibbonGroup::Data : public UIElementData {
    String                  title;
    bool                    word_wrap = false;
    RibbonAction*           option_item = nullptr;
    QMetaObject::Connection option_conn;   // 选项按钮 QAction 销毁回调
};

inline auto RibbonGroup::dptr() -> Data* { return static_cast<Data*>(UIElement::d); }
inline auto RibbonGroup::dptr() const -> const Data* { return static_cast<const Data*>(UIElement::d); }

RibbonGroup::RibbonGroup()
    : Control(new Data(), new SARibbonPanel())
{
}

RibbonGroup::~RibbonGroup()
{
    // 断开选项按钮 QAction 的销毁回调，避免面板销毁后回调访问已释放对象
    QObject::disconnect(dptr()->option_conn);
    // d is deleted by UIElement
}

void RibbonGroup::title(const String& t)
{
    dptr()->title = t;
    auto* pnl = impl<SARibbonPanel>();
    if (pnl) {
        auto utf16 = t.toUtf16();
        pnl->setPanelName(QString::fromStdU16String(utf16));
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
        pnl->addWidget(w, rowProportionFor(b->buttonSize()));
}

void RibbonGroup::addControl(Control* w, RibbonItemSize size)
{
    if (!w) return;
    auto wgt = w->impl<QWidget>();
    auto* pnl = impl<SARibbonPanel>();
    if (wgt && pnl)
        pnl->addWidget(wgt, rowProportionFor(size));
}

void RibbonGroup::removeControl(Control* w)
{
    if (!w) return;
    auto* pnl = impl<SARibbonPanel>();
    if (!pnl) return;
    auto* wgt = w->impl<QWidget>();
    if (!wgt) return;

    auto* action = wgt->findChild<QAction*>(QString(), Qt::FindDirectChildrenOnly);
    if (!action) {
        if (auto* tb = qobject_cast<QToolButton*>(wgt))
            action = tb->defaultAction();
    }
    if (action)
        pnl->removeAction(action);

    wgt->setParent(nullptr);
    wgt->deleteLater();
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

void RibbonGroup::addSeparator()
{
    auto* pnl = impl<SARibbonPanel>();
    if (pnl)
        pnl->addSeparator();
}

void RibbonGroup::layoutMode(RibbonPanelLayoutMode m)
{
    auto* pnl = impl<SARibbonPanel>();
    if (pnl)
        pnl->setPanelLayoutMode(toSarPanelLayoutMode(m));
}

RibbonPanelLayoutMode RibbonGroup::layoutMode() const
{
    auto* pnl = impl<SARibbonPanel>();
    if (!pnl)
        return RibbonPanelLayoutMode::ThreeRow;
    return fromSarPanelLayoutMode(pnl->panelLayoutMode());
}

void RibbonGroup::expanding(bool on)
{
    auto* pnl = impl<SARibbonPanel>();
    if (pnl)
        pnl->setExpanding(on);
}

bool RibbonGroup::expanding() const
{
    auto* pnl = impl<SARibbonPanel>();
    return pnl && pnl->isExpanding();
}

void RibbonGroup::canCustomize(bool on)
{
    auto* pnl = impl<SARibbonPanel>();
    if (pnl)
        pnl->setCanCustomize(on);
}

bool RibbonGroup::canCustomize() const
{
    auto* pnl = impl<SARibbonPanel>();
    return pnl && pnl->isCanCustomize();
}

void RibbonGroup::largeIconSize(const Size& s)
{
    auto* pnl = impl<SARibbonPanel>();
    if (pnl)
        pnl->setLargeIconSize(QSize(s.x, s.y));
}

Size RibbonGroup::largeIconSize() const
{
    auto* pnl = impl<SARibbonPanel>();
    if (!pnl)
        return {};
    const QSize qs = pnl->largeIconSize();
    return Size(qs.width(), qs.height());
}

void RibbonGroup::smallIconSize(const Size& s)
{
    auto* pnl = impl<SARibbonPanel>();
    if (pnl)
        pnl->setSmallIconSize(QSize(s.x, s.y));
}

Size RibbonGroup::smallIconSize() const
{
    auto* pnl = impl<SARibbonPanel>();
    if (!pnl)
        return {};
    const QSize qs = pnl->smallIconSize();
    return Size(qs.width(), qs.height());
}

void RibbonGroup::iconRightText(bool on)
{
    auto* pnl = impl<SARibbonPanel>();
    if (pnl)
        pnl->setEnableIconRightText(on);
}

bool RibbonGroup::iconRightText() const
{
    auto* pnl = impl<SARibbonPanel>();
    return pnl && pnl->isEnableIconRightText();
}

void RibbonGroup::wordWrap(bool on)
{
    // SARibbonPanel::setEnableWordWrap 是 protected（仅供 SARibbonBar/Category
    // 同步），面板层无法直接调用；改为遍历按钮逐个设置。
    dptr()->word_wrap = on;

    auto* pnl = impl<SARibbonPanel>();
    if (!pnl)
        return;

    const auto btns = pnl->ribbonToolButtons();
    for (auto* b : btns)
        b->setEnableWordWrap(on);
}

bool RibbonGroup::wordWrap() const
{
    return dptr()->word_wrap;
}

void RibbonGroup::setOptionAction(RibbonAction* item)
{
    // 断开旧回调，避免重复连接
    if (dptr()->option_conn) {
        QObject::disconnect(dptr()->option_conn);
        dptr()->option_conn = {};
    }
    dptr()->option_item = item;

    auto* pnl = impl<SARibbonPanel>();
    if (!pnl)
        return;

    QAction* act = item ? item->impl<QAction>() : nullptr;
    pnl->setOptionAction(act);

    // item 被销毁时同步清掉面板的选项按钮，避免悬垂指针
    if (act) {
        dptr()->option_conn = QObject::connect(act, &QObject::destroyed, [this, pnl](QObject*) {
            if (dptr()->option_item) {
                dptr()->option_item = nullptr;
                if (pnl)
                    pnl->setOptionAction(nullptr);
            }
        });
    }
}

RibbonAction* RibbonGroup::optionAction() const
{
    return dptr()->option_item;
}

V_APPFWGUI_NS_END
