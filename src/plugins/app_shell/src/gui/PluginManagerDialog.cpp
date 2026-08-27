#include "PluginManagerDialog.hpp"

#include <QDialog>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

#include <vine/appfw/Plugin.hpp>
#include <vine/appfw/gui/UIElementData.hpp>

V_APPFWGUI_NS_BEGIN

namespace
{

QString toQString(const String& s)
{
    return QString::fromStdU16String(s.toUtf16());
}

} // namespace

V_OBJECT_META_IMPL(PluginManagerDialog, Window)

struct PluginManagerDialog::Data : public UIElementData {
    vine::appfw::PluginManager* manager = nullptr;
    QListWidget*                list    = nullptr;
};

PluginManagerDialog::PluginManagerDialog(vine::appfw::PluginManager* manager)
  : Window(new Data(), new QDialog())
{
    auto* data    = dptr();
    data->manager = manager;

    auto* root = impl<QDialog>();
    root->setWindowTitle(QStringLiteral("插件管理器"));

    auto* lay = new QVBoxLayout(root);
    data->list = new QListWidget(root);
    lay->addWidget(data->list);

    auto* loadBtn    = new QPushButton(QStringLiteral("加载插件…"), root);
    auto* refreshBtn = new QPushButton(QStringLiteral("刷新"), root);
    auto* closeBtn   = new QPushButton(QStringLiteral("关闭"), root);
    auto* btnLay     = new QHBoxLayout();
    btnLay->addWidget(loadBtn);
    btnLay->addWidget(refreshBtn);
    btnLay->addStretch();
    btnLay->addWidget(closeBtn);
    lay->addLayout(btnLay);

    QObject::connect(loadBtn, &QPushButton::clicked, root, [this] {
        auto* data = dptr();
        if (!data->manager) {
            return;
        }
        const QString file = QFileDialog::getOpenFileName(
            nullptr, QStringLiteral("选择插件库"), QString(), QStringLiteral("插件库 (*.dll *.so *.dylib)"));
        if (file.isEmpty()) {
            return;
        }
        const auto* u16 = file.utf16();
        data->manager->load(vine::String::fromUtf16(reinterpret_cast<const char16_t*>(u16), file.size()));
        refresh();
    });
    QObject::connect(refreshBtn, &QPushButton::clicked, root, [this] { refresh(); });
    QObject::connect(closeBtn, &QPushButton::clicked, root, [root] { root->close(); });

    refresh();
}

PluginManagerDialog::~PluginManagerDialog()
{
    // d is released by UIElement
}

void PluginManagerDialog::refresh()
{
    auto* data = dptr();
    data->list->clear();
    if (!data->manager) {
        return;
    }
    for (const auto& name : data->manager->names()) {
        auto*      plugin = data->manager->plugin(name);
        const auto info   = plugin ? plugin->info() : vine::appfw::PluginInfo{};
        const auto path   = data->manager->libraryPath(name);

        QString text = toQString(name);
        if (!info.version.empty()) {
            text += QStringLiteral("  v") + toQString(info.version);
        }
        if (!path.empty()) {
            text += QStringLiteral("\n  ") + QString::fromStdString(path.string());
        }
        data->list->addItem(text);
    }
}

inline auto PluginManagerDialog::dptr() -> Data*
{
    return static_cast<Data*>(UIElement::d);
}

inline auto PluginManagerDialog::dptr() const -> const Data*
{
    return static_cast<const Data*>(UIElement::d);
}

V_APPFWGUI_NS_END
