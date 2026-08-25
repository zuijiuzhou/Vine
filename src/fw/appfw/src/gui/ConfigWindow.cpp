#include <vine/appfw/gui/ConfigWindow.hpp>

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QScrollArea>
#include <QSpinBox>
#include <QVBoxLayout>

#include <utility>
#include <vector>

#include "UIElementData.hpp"

V_APPFWGUI_NS_BEGIN

namespace
{

QString toQString(const String& s)
{
    auto u16 = s.toUtf16();
    return QString::fromStdU16String(u16);
}

String fromQString(const QString& qs)
{
    return String::fromUtf16((const char16_t*)qs.utf16(), qs.size());
}

// 按条目类型生成编辑器并连接「编辑即写回」ConfigManager。
QWidget* makeEditorWidget(ConfigManager* config, const ConfigItem& item)
{
    const String key = item.key();
    switch (item.type()) {
    case ConfigItemType::String: {
        auto* e = new QLineEdit();
        if (item.readOnly())
            e->setReadOnly(true);
        QObject::connect(e, &QLineEdit::textChanged, [config, key](const QString& t) {
            config->setString(key, fromQString(t));
        });
        return e;
    }
    case ConfigItemType::Bool: {
        auto* e = new QCheckBox();
        QObject::connect(e, &QCheckBox::toggled, [config, key](bool on) {
            config->setBool(key, on);
        });
        return e;
    }
    case ConfigItemType::Int: {
        auto* e = new QSpinBox();
        if (item.hasRange())
            e->setRange(item.minInt(), item.maxInt());
        else
            e->setRange(0, 1000000);
        e->setSingleStep(item.step() >= 1.0 ? static_cast<int>(item.step()) : 1);
        QObject::connect(e, QOverload<int>::of(&QSpinBox::valueChanged), [config, key](int v) {
            config->setInt(key, v);
        });
        return e;
    }
    case ConfigItemType::Double: {
        auto* e = new QDoubleSpinBox();
        if (item.hasRange())
            e->setRange(item.minDouble(), item.maxDouble());
        else
            e->setRange(0.0, 1000000.0);
        e->setSingleStep(item.step());
        e->setDecimals(6);
        QObject::connect(e, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [config, key](double v) {
            config->setDouble(key, v);
        });
        return e;
    }
    case ConfigItemType::Choice: {
        auto* e = new QComboBox();
        for (const auto& c : item.choices())
            e->addItem(toQString(c));
        QObject::connect(e, &QComboBox::currentTextChanged, [config, key](const QString& t) {
            config->setString(key, fromQString(t));
        });
        return e;
    }
    }
    return nullptr;
}

} // namespace

V_OBJECT_META_IMPL(ConfigWindow, Window)

struct ConfigWindow::Data : public UIElementData {
    ConfigRegistry*       registry = nullptr;
    ConfigManager*        config   = nullptr;
    std::vector<QWidget*> editors;   // 与 registry->items() 顺序一致
};

inline auto ConfigWindow::dptr() -> Data* { return static_cast<Data*>(Window::d); }
inline auto ConfigWindow::dptr() const -> const Data* { return static_cast<const Data*>(Window::d); }

ConfigWindow::ConfigWindow(ConfigRegistry* registry, ConfigManager* config)
    : Window(new Data(), new QDialog())
{
    auto* data = dptr();
    data->registry = registry;
    data->config   = config;

    auto* root   = impl<QDialog>();
    auto* scroll = new QScrollArea(root);
    scroll->setWidgetResizable(true);
    auto* container = new QWidget();
    auto* vlay      = new QVBoxLayout(container);

    // 分组容器：group -> QFormLayout（按首次出现顺序）
    std::vector<std::pair<String, QFormLayout*>> groups;
    auto groupForm = [&](const String& g) -> QFormLayout* {
        for (auto& entry : groups) {
            if (entry.first == g)
                return entry.second;
        }
        auto* box  = new QGroupBox(toQString(g));
        auto* form = new QFormLayout(box);
        vlay->addWidget(box);
        groups.emplace_back(g, form);
        return form;
    };

    for (const auto& item : registry->items()) {
        auto* form   = groupForm(item.group());
        auto* editor = makeEditorWidget(config, item);
        data->editors.push_back(editor);
        if (item.readOnly())
            editor->setEnabled(false);
        auto* label = new QLabel(toQString(item.label()));
        if (!item.description().empty())
            label->setToolTip(toQString(item.description()));
        form->addRow(label, editor);
    }

    if (registry->itemCount() == 0) {
        auto* tip = new QLabel(QStringLiteral("(未注册配置项)"));
        vlay->addWidget(tip);
    }

    scroll->setWidget(container);
    auto* lay = new QVBoxLayout(root);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->addWidget(scroll);

    refresh();
}

ConfigWindow::~ConfigWindow()
{
    // d 由 UIElement 释放
}

void ConfigWindow::refresh()
{
    auto* data        = dptr();
    const auto& items = data->registry->items();
    for (size_t i = 0; i < items.size() && i < data->editors.size(); ++i) {
        const auto& item = items[i];
        QWidget*    w    = data->editors[i];
        const String key = item.key();
        w->blockSignals(true);
        switch (item.type()) {
        case ConfigItemType::String:
            static_cast<QLineEdit*>(w)->setText(
                toQString(data->config->getString(key, item.hasDefault() ? item.defaultString() : String())));
            break;
        case ConfigItemType::Bool:
            static_cast<QCheckBox*>(w)->setChecked(
                data->config->getBool(key, item.hasDefault() && item.defaultBool()));
            break;
        case ConfigItemType::Int:
            static_cast<QSpinBox*>(w)->setValue(
                data->config->getInt(key, item.hasDefault() ? item.defaultInt() : 0));
            break;
        case ConfigItemType::Double:
            static_cast<QDoubleSpinBox*>(w)->setValue(
                data->config->getDouble(key, item.hasDefault() ? item.defaultDouble() : 0.0));
            break;
        case ConfigItemType::Choice: {
            auto*      combo = static_cast<QComboBox*>(w);
            const auto v     = data->config->getString(key);
            const int  idx   = combo->findText(toQString(v));
            combo->setCurrentIndex(idx >= 0 ? idx : 0);
            break;
        }
        }
        w->blockSignals(false);
    }
}

void ConfigWindow::reset()
{
    auto* data = dptr();
    for (const auto& item : data->registry->items()) {
        if (item.hasDefault()) {
            switch (item.type()) {
            case ConfigItemType::String:
                data->config->setString(item.key(), item.defaultString());
                break;
            case ConfigItemType::Bool:
                data->config->setBool(item.key(), item.defaultBool());
                break;
            case ConfigItemType::Int:
                data->config->setInt(item.key(), item.defaultInt());
                break;
            case ConfigItemType::Double:
                data->config->setDouble(item.key(), item.defaultDouble());
                break;
            case ConfigItemType::Choice:
                data->config->setString(item.key(), item.defaultString());
                break;
            }
        } else {
            data->config->remove(item.key());
        }
    }
    refresh();
}

ConfigRegistry* ConfigWindow::registry() const
{
    return dptr()->registry;
}

ConfigManager* ConfigWindow::config() const
{
    return dptr()->config;
}

V_APPFWGUI_NS_END
