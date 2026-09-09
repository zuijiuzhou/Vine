#include <vine/appfw/gui/CommandManagerDialog.hpp>

#include <QAbstractItemView>
#include <QDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

#include <vine/appfw/gui/UIElementData.hpp>

#include "Convert.hpp"

V_APPFWGUI_NS_BEGIN

V_OBJECT_META_IMPL(CommandManagerDialog, Window)

struct CommandManagerDialog::Impl : public UIElementData {
    vine::appfw::CommandManager* manager = nullptr;
    QLineEdit*                   filter  = nullptr;
    QTableWidget*                table   = nullptr;
};

CommandManagerDialog::CommandManagerDialog(vine::appfw::CommandManager* manager)
  : Window(new Impl(), new QDialog())
{
    auto* data    = dptr();
    data->manager = manager;

    auto* root = impl<QDialog>();
    root->setWindowTitle(QStringLiteral("命令管理器"));

    auto* lay = new QVBoxLayout(root);

    data->filter = new QLineEdit(root);
    data->filter->setPlaceholderText(QStringLiteral("筛选名称 / 分组 / 描述 / 别名…"));
    lay->addWidget(data->filter);

    data->table = new QTableWidget(0, 4, root);
    data->table->setHorizontalHeaderLabels({ QStringLiteral("名称"), QStringLiteral("分组"), QStringLiteral("描述"), QStringLiteral("别名") });
    data->table->horizontalHeader()->setStretchLastSection(true);
    data->table->setSelectionBehavior(QAbstractItemView::SelectRows);
    data->table->setSelectionMode(QAbstractItemView::SingleSelection);
    data->table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    lay->addWidget(data->table);

    auto* refreshBtn = new QPushButton(QStringLiteral("刷新"), root);
    auto* removeBtn  = new QPushButton(QStringLiteral("卸载命令"), root);
    auto* closeBtn   = new QPushButton(QStringLiteral("关闭"), root);
    auto* btnLay     = new QHBoxLayout();
    btnLay->addWidget(refreshBtn);
    btnLay->addWidget(removeBtn);
    btnLay->addStretch();
    btnLay->addWidget(closeBtn);
    lay->addLayout(btnLay);

    QObject::connect(data->filter, &QLineEdit::textChanged, root, [this] { applyFilter(); });
    QObject::connect(refreshBtn, &QPushButton::clicked, root, [this] { refresh(); });
    QObject::connect(removeBtn, &QPushButton::clicked, root, [this] { unregisterSelected(); });
    QObject::connect(closeBtn, &QPushButton::clicked, root, [root] { root->close(); });

    refresh();
}

CommandManagerDialog::~CommandManagerDialog()
{
    // d is released by UIElement
}

void CommandManagerDialog::refresh()
{
    auto* data = dptr();
    data->table->setRowCount(0);
    if (!data->manager) {
        return;
    }

    for (const auto& info : data->manager->commandInfos()) {
        const int row = data->table->rowCount();
        data->table->insertRow(row);

        data->table->setItem(row, 0, new QTableWidgetItem(Convert::toQString(info.name)));
        data->table->setItem(row, 1, new QTableWidgetItem(Convert::toQString(info.group)));
        data->table->setItem(row, 2, new QTableWidgetItem(Convert::toQString(info.description)));

        QString aliases;
        bool    first = true;
        for (const auto& alias : info.aliases) {
            if (!first) {
                aliases += QStringLiteral(", ");
            }
            aliases += Convert::toQString(alias);
            first = false;
        }
        data->table->setItem(row, 3, new QTableWidgetItem(aliases));
    }

    applyFilter();
}

void CommandManagerDialog::applyFilter()
{
    auto* data      = dptr();
    const QString needle = data->filter->text().trimmed();

    for (int row = 0; row < data->table->rowCount(); ++row) {
        bool match = needle.isEmpty();
        for (int col = 0; col < data->table->columnCount() && !match; ++col) {
            if (const auto* item = data->table->item(row, col)) {
                match = item->text().contains(needle, Qt::CaseInsensitive);
            }
        }
        data->table->setRowHidden(row, !match);
    }
}

void CommandManagerDialog::unregisterSelected()
{
    auto* data = dptr();
    if (!data->manager) {
        return;
    }

    const int row = data->table->currentRow();
    if (row < 0) {
        return;
    }

    const auto* nameItem = data->table->item(row, 0);
    if (!nameItem) {
        return;
    }

    const auto* u16        = nameItem->text().utf16();
    const String name      = String::fromUtf16(reinterpret_cast<const char16_t*>(u16), nameItem->text().size());
    data->manager->unregisterCommand(name);

    // Also drop aliases that resolve to the removed command.
    for (const auto& alias : data->manager->aliases()) {
        if (alias.second == name) {
            data->manager->unregisterAlias(alias.first);
        }
    }

    refresh();
}

inline auto CommandManagerDialog::dptr() -> Impl*
{
    return static_cast<Impl*>(UIElement::d);
}

inline auto CommandManagerDialog::dptr() const -> const Impl*
{
    return static_cast<const Impl*>(UIElement::d);
}

V_APPFWGUI_NS_END
