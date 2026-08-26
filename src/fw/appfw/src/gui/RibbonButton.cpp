#include <vine/appfw/gui/RibbonButton.hpp>

#include <SARibbon.h>
#include <QAction>
#include <QIcon>
#include <QSize>
#include <QToolButton>
#include <algorithm>

#include <vine/appfw/gui/RibbonAction.hpp>

#include "UIElementData.hpp"

V_APPFWGUI_NS_BEGIN

namespace
{
Qt::ToolButtonStyle toQtStyle(RibbonButtonStyle s)
{
    switch (s) {
    case RibbonButtonStyle::IconOnly: return Qt::ToolButtonIconOnly;
    case RibbonButtonStyle::TextOnly: return Qt::ToolButtonTextOnly;
    case RibbonButtonStyle::TextBesideIcon: return Qt::ToolButtonTextBesideIcon;
    case RibbonButtonStyle::TextUnderIcon: return Qt::ToolButtonTextUnderIcon;
    }

    return Qt::ToolButtonIconOnly;
}

RibbonButtonStyle fromQtStyle(Qt::ToolButtonStyle s)
{
    switch (s) {
    case Qt::ToolButtonTextOnly: return RibbonButtonStyle::TextOnly;
    case Qt::ToolButtonTextBesideIcon: return RibbonButtonStyle::TextBesideIcon;
    case Qt::ToolButtonTextUnderIcon: return RibbonButtonStyle::TextUnderIcon;
    default: break;
    }

    return RibbonButtonStyle::IconOnly;
}
} // namespace

V_OBJECT_META_IMPL(RibbonButton, Control)

struct RibbonButton::Data : public UIElementData {
    void* user = nullptr;
    RibbonItemSize buttonSize = RibbonItemSize::Small;
    /// 下拉菜单条目（有序）：item 为 nullptr 表示分隔线。
    struct Entry {
        RibbonAction* item = nullptr;
    };
    std::vector<Entry>    entries;    // 有序条目（项/分隔线）
    std::vector<QAction*> separators; // 本按钮创建的分隔线 QAction（重建时释放）
    SARibbonMenu*         menu = nullptr;
    EventArgs             clickedArgs;
};

RibbonButton::RibbonButton()
  : Control(new Data(), new SARibbonToolButton(static_cast<QWidget*>(nullptr)))
{
    auto* btn = impl<SARibbonToolButton>();

    if (btn) {
        // Bridge the Qt clicked signal to the framework's `clicked` event.
        QObject::connect(btn, &QToolButton::clicked, [this](bool) { clicked.trigger(*this, dptr()->clickedArgs); });
    }
}

RibbonButton::~RibbonButton()
{
    // d is deleted by UIElement
}

void RibbonButton::text(const String& t)
{
    auto* btn = impl<SARibbonToolButton>();
    if (!btn)
        return;
    auto utf16 = t.toUtf16();
    btn->setText(QString::fromStdU16String(utf16));
}

String RibbonButton::text() const
{
    auto* btn = impl<SARibbonToolButton>();
    if (!btn)
        return {};
    auto qs = btn->text();
    return String::fromUtf16((const char16_t*)qs.utf16(), qs.size());
}

void RibbonButton::icon(const Icon& ic)
{
    auto* btn = impl<SARibbonToolButton>();
    if (!btn)
        return;
    btn->setIcon(ic.value());
}

Icon RibbonButton::icon() const
{
    auto* btn = impl<SARibbonToolButton>();
    if (!btn)
        return {};
    return Icon(btn->icon());
}

void RibbonButton::checkable(bool on)
{
    auto* btn = impl<SARibbonToolButton>();
    if (!btn)
        return;
    btn->setCheckable(on);
}

bool RibbonButton::checkable() const
{
    auto* btn = impl<SARibbonToolButton>();
    return btn && btn->isCheckable();
}

void RibbonButton::checked(bool on)
{
    auto* btn = impl<SARibbonToolButton>();
    if (!btn)
        return;
    btn->setChecked(on);
}

bool RibbonButton::checked() const
{
    auto* btn = impl<SARibbonToolButton>();
    return btn && btn->isChecked();
}

void RibbonButton::buttonSize(RibbonItemSize s)
{
    dptr()->buttonSize = s;

    auto* btn = impl<SARibbonToolButton>();
    if (!btn)
        return;

    // SARibbon 按钮类型只有 Large/Small 两档：Medium 走 SmallButton，
    // 中等占位由面板行比例（RowProportion::Medium）决定。
    btn->setButtonType(s == RibbonItemSize::Large ? SARibbonToolButton::LargeButton : SARibbonToolButton::SmallButton);
}

RibbonItemSize RibbonButton::buttonSize() const
{
    return dptr()->buttonSize;
}

void RibbonButton::style(RibbonButtonStyle s)
{
    auto* btn = impl<SARibbonToolButton>();
    if (!btn)
        return;
    btn->setToolButtonStyle(toQtStyle(s));
}

RibbonButtonStyle RibbonButton::style() const
{
    auto* btn = impl<SARibbonToolButton>();
    if (!btn)
        return RibbonButtonStyle::IconOnly;
    return fromQtStyle(btn->toolButtonStyle());
}

void RibbonButton::largeIconSize(const Size& s)
{
    auto* btn = impl<SARibbonToolButton>();
    if (!btn)
        return;
    btn->setLargeIconSize(QSize(s.x, s.y));
}

Size RibbonButton::largeIconSize() const
{
    auto* btn = impl<SARibbonToolButton>();
    if (!btn)
        return {};
    const QSize qs = btn->largeIconSize();
    return Size(qs.width(), qs.height());
}

void RibbonButton::smallIconSize(const Size& s)
{
    auto* btn = impl<SARibbonToolButton>();
    if (!btn)
        return;
    btn->setSmallIconSize(QSize(s.x, s.y));
}

Size RibbonButton::smallIconSize() const
{
    auto* btn = impl<SARibbonToolButton>();
    if (!btn)
        return {};
    const QSize qs = btn->smallIconSize();
    return Size(qs.width(), qs.height());
}

void RibbonButton::wordWrap(bool on)
{
    auto* btn = impl<SARibbonToolButton>();
    if (!btn)
        return;
    btn->setEnableWordWrap(on);
}

bool RibbonButton::wordWrap() const
{
    auto* btn = impl<SARibbonToolButton>();
    return btn && btn->isEnableWordWrap();
}

void RibbonButton::iconRightText(bool on)
{
    auto* btn = impl<SARibbonToolButton>();
    if (!btn)
        return;
    btn->setEnableIconRightText(on);
}

bool RibbonButton::iconRightText() const
{
    auto* btn = impl<SARibbonToolButton>();
    return btn && btn->isEnableIconRightText();
}

void RibbonButton::addDropDownItem(RibbonAction* item)
{
    if (!item)
        return;

    auto& entries = dptr()->entries;

    for (const auto& e : entries) {
        if (e.item == item) {
            return;
        } // 已存在则忽略
    }

    entries.push_back({ item });

    rebuildMenu();
}

void RibbonButton::removeDropDownItem(RibbonAction* item)
{
    if (!item)
        return;

    auto& entries = dptr()->entries;
    auto  it      = std::find_if(entries.begin(), entries.end(), [item](const Data::Entry& e) { return e.item == item; });

    if (it == entries.end()) {
        return;
    }

    entries.erase(it);

    // 把 item 的 QAction 所有权交还给 item（挂菜单期间由菜单持有）；
    // rebuildMenu() 会把它从菜单摘除。
    item->setOwnsImpl(true);

    rebuildMenu();
}

void RibbonButton::clearDropDownItems()
{
    auto& entries = dptr()->entries;

    if (entries.empty()) {
        return;
    }

    // 把每个 item 的 QAction 所有权交还后再清空。
    for (const auto& e : entries) {
        if (e.item) {
            e.item->setOwnsImpl(true);
        }
    }

    entries.clear();

    rebuildMenu();
}

void RibbonButton::addSeparator()
{
    dptr()->entries.push_back({ nullptr });

    rebuildMenu();
}

size_t RibbonButton::dropDownItemCount() const
{
    // 只统计真实项，分隔线不计入
    size_t n = 0;
    for (const auto& e : dptr()->entries) {
        if (e.item) {
            ++n;
        }
    }
    return n;
}

RibbonAction* RibbonButton::dropDownItemAt(size_t i) const
{
    const auto& entries = dptr()->entries;

    size_t idx = 0;
    for (const auto& e : entries) {
        if (!e.item) {
            continue;
        }
        if (idx == i) {
            return e.item;
        }
        ++idx;
    }

    return nullptr;
}

size_t RibbonButton::dropDownEntryCount() const
{
    // 全条目数：真实项 + 分隔线
    return dptr()->entries.size();
}

void RibbonButton::removeDropDownEntryAt(size_t i)
{
    auto& entries = dptr()->entries;

    if (i >= entries.size()) {
        return;
    } // 越界安全

    auto it = entries.begin() + i;

    // 移除真实项时交还 QAction 所有权；分隔线无需处理（由本按钮持有）
    if (it->item) {
        it->item->setOwnsImpl(true);
    }

    entries.erase(it);

    rebuildMenu();
}

void RibbonButton::setData(void* data)
{
    dptr()->user = data;
}

void* RibbonButton::data() const
{
    return dptr()->user;
}

void RibbonButton::rebuildMenu()
{
    auto* btn = impl<SARibbonToolButton>();
    if (!btn) {
        return;
    }

    auto& entries = dptr()->entries;

    // 释放上次构建的分隔线 QAction（由本按钮创建，需要自行管理）。
    // 它们虽是菜单的子对象，但重建后不再出现在菜单 action 列表里。
    for (QAction* s : dptr()->separators) {
        delete s;
    }
    dptr()->separators.clear();

    if (entries.empty()) {
        if (dptr()->menu) {
            btn->setMenu(nullptr);
            btn->setPopupMode(QToolButton::DelayedPopup);
        }

        return;
    }

    if (!dptr()->menu) {
        dptr()->menu = new SARibbonMenu(btn);
    }

    // 摘除当前所有 action 后按条目顺序重建。绝不使用 QMenu::clear()：
    // 它会删除 item 的 QAction（item 的 UIElement 也引用它）。
    for (QAction* a : dptr()->menu->actions()) {
        dptr()->menu->removeAction(a);
    }

    for (const auto& e : entries) {
        if (!e.item) {
            dptr()->separators.push_back(dptr()->menu->addSeparator());
            continue;
        }
        QAction* act = e.item->impl<QAction>();
        if (!act) {
            continue;
        }
        e.item->setOwnsImpl(false); // the menu now owns the action
        dptr()->menu->addAction(act);
    }

    btn->setMenu(dptr()->menu);
    btn->setPopupMode(QToolButton::InstantPopup);
}

inline auto RibbonButton::dptr() -> Data*
{
    return static_cast<Data*>(UIElement::d);
}

inline auto RibbonButton::dptr() const -> const Data*
{
    return static_cast<const Data*>(UIElement::d);
}

V_APPFWGUI_NS_END
