#include <vine/appfw/gui/PluginManagerDialog.hpp>

#include <QAbstractItemView>
#include <QAction>
#include <QDialog>
#include <QFileDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QListWidget>
#include <QMenu>
#include <QPushButton>
#include <QSplitter>
#include <QStackedWidget>
#include <QTableWidget>
#include <QVBoxLayout>

#include <vine/appfw/CommandManager.hpp>
#include <vine/appfw/ConfigItem.hpp>
#include <vine/appfw/Plugin.hpp>
#include <vine/appfw/gui/UIElementData.hpp>

V_APPFWGUI_NS_BEGIN

namespace
{

QString toQString(const String& s)
{
    return QString::fromStdU16String(s.toUtf16());
}

vine::String toVineString(const QString& s)
{
    const auto* u16 = s.utf16();
    return vine::String::fromUtf16(reinterpret_cast<const char16_t*>(u16), s.size());
}

QString joinStrings(const std::vector<String>& list)
{
    QString result;
    bool    first = true;
    for (const auto& s : list) {
        if (!first) {
            result += QStringLiteral(", ");
        }
        result += toQString(s);
        first = false;
    }
    return result.isEmpty() ? QStringLiteral("—") : result;
}

} // namespace

V_OBJECT_META_IMPL(PluginManagerDialog, Window)

struct PluginManagerDialog::Impl : public UIElementData {
    vine::appfw::PluginManager* manager       = nullptr;
    QListWidget*                list          = nullptr;
    QStackedWidget*             stack         = nullptr;
    QLabel*                     nameLabel     = nullptr;
    QLabel*                     versionLabel  = nullptr;
    QLabel*                     vendorLabel   = nullptr;
    QLabel*                     depLabel      = nullptr;
    QLabel*                     pathLabel     = nullptr;
    QLabel*                     descLabel     = nullptr;
    QTableWidget*               cmdTable      = nullptr;
    QTableWidget*               cfgTable      = nullptr;
};

PluginManagerDialog::PluginManagerDialog(vine::appfw::PluginManager* manager)
  : Window(new Impl(), new QDialog())
{
    auto* data    = dptr();
    data->manager = manager;

    auto* root = impl<QDialog>();
    root->setWindowTitle(QStringLiteral("插件管理器"));

    auto* outer = new QVBoxLayout(root);

    auto* splitter = new QSplitter(Qt::Horizontal, root);
    outer->addWidget(splitter, 1);

    // Left: plugin list.
    data->list = new QListWidget(splitter);
    data->list->setSelectionMode(QAbstractItemView::SingleSelection);
    data->list->setContextMenuPolicy(Qt::CustomContextMenu);
    splitter->addWidget(data->list);

    // Right: placeholder + detail page.
    data->stack = new QStackedWidget(splitter);

    auto* placeholder = new QLabel(QStringLiteral("请选择左侧插件查看详情"), data->stack);
    placeholder->setAlignment(Qt::AlignCenter);
    placeholder->setStyleSheet(QStringLiteral("color: gray;"));
    data->stack->addWidget(placeholder);

    auto* detail     = new QWidget(data->stack);
    auto* detailLay  = new QVBoxLayout(detail);
    detailLay->setContentsMargins(6, 6, 6, 6);

    auto* form = new QFormLayout();
    data->nameLabel    = new QLabel(detail);
    data->versionLabel = new QLabel(detail);
    data->vendorLabel  = new QLabel(detail);
    data->depLabel     = new QLabel(detail);
    data->pathLabel    = new QLabel(detail);
    data->descLabel    = new QLabel(detail);
    data->nameLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    data->versionLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    data->vendorLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    data->depLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    data->pathLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    data->pathLabel->setWordWrap(true);
    data->descLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    data->descLabel->setWordWrap(true);

    form->addRow(QStringLiteral("名称"), data->nameLabel);
    form->addRow(QStringLiteral("版本"), data->versionLabel);
    form->addRow(QStringLiteral("厂商"), data->vendorLabel);
    form->addRow(QStringLiteral("依赖"), data->depLabel);
    form->addRow(QStringLiteral("库路径"), data->pathLabel);
    form->addRow(QStringLiteral("描述"), data->descLabel);
    detailLay->addLayout(form);

    // Commands reported by the plugin.
    data->cmdTable = new QTableWidget(0, 3, detail);
    data->cmdTable->setHorizontalHeaderLabels({ QStringLiteral("命令"), QStringLiteral("分组"), QStringLiteral("描述") });
    data->cmdTable->horizontalHeader()->setStretchLastSection(true);
    data->cmdTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    data->cmdTable->setSelectionMode(QAbstractItemView::SingleSelection);
    data->cmdTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    auto* cmdBox = new QGroupBox(QStringLiteral("命令"), detail);
    auto* cmdLay = new QVBoxLayout(cmdBox);
    cmdLay->setContentsMargins(6, 4, 6, 4);
    cmdLay->addWidget(data->cmdTable);
    detailLay->addWidget(cmdBox, 1);

    // Config items reported by the plugin.
    data->cfgTable = new QTableWidget(0, 3, detail);
    data->cfgTable->setHorizontalHeaderLabels({ QStringLiteral("键"), QStringLiteral("标签"), QStringLiteral("描述") });
    data->cfgTable->horizontalHeader()->setStretchLastSection(true);
    data->cfgTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    data->cfgTable->setSelectionMode(QAbstractItemView::SingleSelection);
    data->cfgTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    auto* cfgBox = new QGroupBox(QStringLiteral("配置"), detail);
    auto* cfgLay = new QVBoxLayout(cfgBox);
    cfgLay->setContentsMargins(6, 4, 6, 4);
    cfgLay->addWidget(data->cfgTable);
    detailLay->addWidget(cfgBox, 1);

    data->stack->addWidget(detail);
    splitter->addWidget(data->stack);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes({ 220, 520 });

    auto* loadBtn    = new QPushButton(QStringLiteral("加载插件…"), root);
    auto* refreshBtn = new QPushButton(QStringLiteral("刷新"), root);
    auto* closeBtn   = new QPushButton(QStringLiteral("关闭"), root);
    auto* btnLay     = new QHBoxLayout();
    btnLay->addWidget(loadBtn);
    btnLay->addWidget(refreshBtn);
    btnLay->addStretch();
    btnLay->addWidget(closeBtn);
    outer->addLayout(btnLay);

    QObject::connect(loadBtn, &QPushButton::clicked, root, [this] { loadPlugin(); });
    QObject::connect(refreshBtn, &QPushButton::clicked, root, [this] { refresh(); });
    QObject::connect(closeBtn, &QPushButton::clicked, root, [root] { root->close(); });

    QObject::connect(data->list, &QListWidget::currentItemChanged, root, [this](QListWidgetItem* item, QListWidgetItem*) {
        showDetail(item ? toVineString(item->data(Qt::UserRole).toString()) : vine::String{});
    });

    // Right-click menu: view a single plugin's detail / load / refresh.
    QObject::connect(data->list, &QListWidget::customContextMenuRequested, root, [this](const QPoint& pos) {
        auto* data = dptr();
        auto* item = data->list->itemAt(pos);

        QMenu menu(data->list);
        QAction* viewAction     = menu.addAction(QStringLiteral("查看详情"));
        menu.addSeparator();
        QAction* loadAction     = menu.addAction(QStringLiteral("加载插件…"));
        QAction* refreshAction  = menu.addAction(QStringLiteral("刷新"));
        QAction* chosen = menu.exec(data->list->viewport()->mapToGlobal(pos));

        if (chosen == viewAction) {
            if (item) {
                data->list->setCurrentItem(item);
            }
            showDetail(item ? toVineString(item->data(Qt::UserRole).toString()) : vine::String{});
        } else if (chosen == loadAction) {
            loadPlugin();
        } else if (chosen == refreshAction) {
            refresh();
        }
    });

    refresh();
}

PluginManagerDialog::~PluginManagerDialog()
{
    // d is released by UIElement
}

void PluginManagerDialog::loadPlugin()
{
    auto* data = dptr();
    if (!data->manager) {
        return;
    }
    const QString file = QFileDialog::getOpenFileName(
        nullptr, QStringLiteral("选择插件库"), QString(), QStringLiteral("插件库 (*.dll *.so *.dylib)"));
    if (file.isEmpty()) {
        return;
    }
    data->manager->load(toVineString(file));
    refresh();
}

void PluginManagerDialog::refresh()
{
    auto* data = dptr();

    const QString prev = data->list->currentItem() ? data->list->currentItem()->data(Qt::UserRole).toString() : QString();

    data->list->blockSignals(true);
    data->list->clear();
    data->list->blockSignals(false);

    int selectRow = -1;
    if (data->manager) {
        for (const auto& name : data->manager->names()) {
            const QString display = toQString(name);
            auto*         item    = new QListWidgetItem(data->list);
            item->setData(Qt::UserRole, display);
            if (auto* plugin = data->manager->plugin(name)) {
                const auto info = plugin->info();
                if (!info.version.empty()) {
                    item->setText(display + QStringLiteral("  v") + toQString(info.version));
                } else {
                    item->setText(display);
                }
            } else {
                item->setText(display);
            }
            if (display == prev) {
                selectRow = data->list->count() - 1;
            }
        }
    }

    if (selectRow >= 0) {
        data->list->setCurrentRow(selectRow);
    } else if (data->list->count() > 0) {
        data->list->setCurrentRow(0);
    } else {
        showDetail({});
    }
}

void PluginManagerDialog::showDetail(const vine::String& name)
{
    auto* data = dptr();
    if (name.empty() || !data->manager) {
        data->stack->setCurrentIndex(0);
        return;
    }

    auto* plugin = data->manager->plugin(name);
    const auto info = plugin ? plugin->info() : vine::appfw::PluginInfo{};
    const auto path = data->manager->libraryPath(name);

    data->nameLabel->setText(toQString(name));
    data->versionLabel->setText(toQString(info.version));
    data->vendorLabel->setText(toQString(info.vendor));
    data->depLabel->setText(joinStrings(info.dependencies));
    data->pathLabel->setText(path.empty() ? QStringLiteral("—") : QString::fromStdString(path.string()));
    data->descLabel->setText(toQString(info.description));

    // Commands reported by the plugin.
    data->cmdTable->setRowCount(0);
    for (const auto& ci : data->manager->commandInfosForPlugin(name)) {
        const int row = data->cmdTable->rowCount();
        data->cmdTable->insertRow(row);
        data->cmdTable->setItem(row, 0, new QTableWidgetItem(toQString(ci.name)));
        data->cmdTable->setItem(row, 1, new QTableWidgetItem(toQString(ci.group)));
        data->cmdTable->setItem(row, 2, new QTableWidgetItem(toQString(ci.description)));
    }

    // Config items reported by the plugin.
    data->cfgTable->setRowCount(0);
    for (const auto* item : data->manager->configItemsForPlugin(name)) {
        if (item == nullptr) {
            continue;
        }
        const int row = data->cfgTable->rowCount();
        data->cfgTable->insertRow(row);
        data->cfgTable->setItem(row, 0, new QTableWidgetItem(toQString(item->key())));
        data->cfgTable->setItem(row, 1, new QTableWidgetItem(toQString(item->label())));
        data->cfgTable->setItem(row, 2, new QTableWidgetItem(toQString(item->description())));
    }

    data->stack->setCurrentIndex(1);
}

inline auto PluginManagerDialog::dptr() -> Impl*
{
    return static_cast<Impl*>(UIElement::d);
}

inline auto PluginManagerDialog::dptr() const -> const Impl*
{
    return static_cast<const Impl*>(UIElement::d);
}

V_APPFWGUI_NS_END
