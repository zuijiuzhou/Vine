#include <vine/appfw/gui/ConfigWindow.hpp>

#include <any>
#include <vector>

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
#include <QTabWidget>
#include <QVBoxLayout>

#include <vine/appfw/ConfigCategory.hpp>
#include <vine/appfw/ConfigGroup.hpp>

#include <vine/appfw/gui/UIElementData.hpp>

#include "Convert.hpp"

V_APPFWGUI_NS_BEGIN

namespace
{

// Tab title: label > name > "General".
String categoryTitle(const ConfigCategory* cat)
{
    if (!cat->label().empty())
        return cat->label();
    if (!cat->name().empty())
        return cat->name();
    return String(u8"通用");
}

// Group title: label > name.
String groupTitle(const ConfigGroup* grp)
{
    if (!grp->label().empty())
        return grp->label();
    return grp->name();
}

// Creates the editor widget for the item type and wires live write-back to ConfigManager.
QWidget* makeEditorWidget(ConfigManager* config, const ConfigItem& item)
{
    const String key = item.key();
    switch (item.type()) {
    case ConfigItemType::String:
    {
        auto* e = new QLineEdit();
        if (item.readOnly())
            e->setReadOnly(true);
        QObject::connect(e, &QLineEdit::textChanged, [config, key](const QString& t) { config->setString(key, Convert::fromQString(t)); });
        return e;
    }
    case ConfigItemType::Bool:
    {
        auto* e = new QCheckBox();
        QObject::connect(e, &QCheckBox::toggled, [config, key](bool on) { config->setBool(key, on); });
        return e;
    }
    case ConfigItemType::Int:
    {
        auto* e = new QSpinBox();
        if (item.hasRange())
            e->setRange(item.minInt(), item.maxInt());
        else
            e->setRange(0, 1000000);
        e->setSingleStep(item.step() >= 1.0 ? static_cast<int>(item.step()) : 1);
        QObject::connect(e, QOverload<int>::of(&QSpinBox::valueChanged), [config, key](int v) { config->setInt(key, v); });
        return e;
    }
    case ConfigItemType::Double:
    {
        auto* e = new QDoubleSpinBox();
        if (item.hasRange())
            e->setRange(item.minDouble(), item.maxDouble());
        else
            e->setRange(0.0, 1000000.0);
        e->setSingleStep(item.step());
        e->setDecimals(6);
        QObject::connect(e, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [config, key](double v) { config->setDouble(key, v); });
        return e;
    }
    case ConfigItemType::Choice:
    {
        auto*      e       = new QComboBox();
        const auto choices = item.choices();
        for (const auto& c : choices) e->addItem(Convert::toQString(c.description));
        QObject::connect(e, QOverload<int>::of(&QComboBox::currentIndexChanged), [config, key, choices](int idx) {
            if (idx < 0 || static_cast<size_t>(idx) >= choices.size())
                return;
            const auto& v = choices[static_cast<size_t>(idx)].value;
            if (const auto* p = std::any_cast<int>(&v))
                config->setInt(key, *p);
            else if (const auto* p = std::any_cast<double>(&v))
                config->setDouble(key, *p);
            else {
                const String* sp = std::any_cast<String>(&v);
                config->setString(key, sp != nullptr ? *sp : String());
            }
        });
        return e;
    }
    }
    return nullptr;
}

} // namespace

V_OBJECT_META_IMPL(ConfigWindow, Window)

struct ConfigWindow::Impl : public UIElementData {
    ConfigRegistry*       registry = nullptr;
    ConfigManager*        config   = nullptr;
    std::vector<QWidget*> editors; // Parallel to the tree traversal order
};

ConfigWindow::ConfigWindow(ConfigRegistry* registry, ConfigManager* config)
  : Window(new Impl(), new QDialog())
{
    auto* data     = dptr();
    data->registry = registry;
    data->config   = config;

    auto* root = impl<QDialog>();
    auto* tabs = new QTabWidget(root);
    tabs->setTabBarAutoHide(true); // Hides the tab bar when there is a single tab

    for (ConfigCategory* cat : registry->categories()) {
        auto* scroll = new QScrollArea();
        scroll->setWidgetResizable(true);
        auto* container = new QWidget();
        auto* vlay      = new QVBoxLayout(container);

        bool has_item = false;
        for (ConfigGroup* grp : cat->groups()) {
            const auto items = grp->items();
            if (items.empty())
                continue; // Skip empty groups
            auto* box = new QGroupBox(Convert::toQString(groupTitle(grp)));
            if (!grp->description().empty())
                box->setToolTip(Convert::toQString(grp->description()));
            auto* form = new QFormLayout(box);
            vlay->addWidget(box);
            for (const ConfigItem* item : items) {
                auto* editor = makeEditorWidget(config, *item);
                data->editors.push_back(editor);
                if (item->readOnly())
                    editor->setEnabled(false);
                auto* label = new QLabel(Convert::toQString(item->label()));
                if (!item->description().empty())
                    label->setToolTip(Convert::toQString(item->description()));
                form->addRow(label, editor);
                has_item = true;
            }
        }
        if (!has_item) {
            auto* tip = new QLabel(QStringLiteral("(空)"));
            vlay->addWidget(tip);
        }
        scroll->setWidget(container);
        tabs->addTab(scroll, Convert::toQString(categoryTitle(cat)));
        if (!cat->description().empty())
            tabs->setTabToolTip(tabs->count() - 1, Convert::toQString(cat->description()));
    }

    if (registry->categories().empty()) {
        auto* tip = new QLabel(QStringLiteral("(未注册配置项)"));
        tabs->addTab(tip, QStringLiteral("通用"));
    }
    auto* lay = new QVBoxLayout(root);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->addWidget(tabs);

    refresh();
}

ConfigWindow::~ConfigWindow()
{
    // d is released by UIElement
}

void ConfigWindow::refresh()
{
    auto*  data = dptr();
    size_t idx  = 0;
    for (ConfigCategory* cat : data->registry->categories()) {
        for (ConfigGroup* grp : cat->groups()) {
            for (const ConfigItem* item : grp->items()) {
                if (idx >= data->editors.size())
                    return;
                QWidget*     w   = data->editors[idx++];
                const String key = item->key();
                w->blockSignals(true);
                switch (item->type()) {
                case ConfigItemType::String:
                    static_cast<QLineEdit*>(w)->setText(Convert::toQString(data->config->getString(key, item->hasDefault() ? item->defaultString() : String())));
                    break;
                case ConfigItemType::Bool: static_cast<QCheckBox*>(w)->setChecked(data->config->getBool(key, item->hasDefault() && item->defaultBool())); break;
                case ConfigItemType::Int: static_cast<QSpinBox*>(w)->setValue(data->config->getInt(key, item->hasDefault() ? item->defaultInt() : 0)); break;
                case ConfigItemType::Double:
                    static_cast<QDoubleSpinBox*>(w)->setValue(data->config->getDouble(key, item->hasDefault() ? item->defaultDouble() : 0.0));
                    break;
                case ConfigItemType::Choice:
                {
                    auto*       combo = static_cast<QComboBox*>(w);
                    const auto& cs    = item->choices();
                    int         idx   = -1;
                    for (size_t i = 0; i < cs.size(); ++i) {
                        const auto& v = cs[i].value;
                        if (const auto* p = std::any_cast<int>(&v)) {
                            if (data->config->getInt(key, *p) == *p) {
                                idx = static_cast<int>(i);
                                break;
                            }
                        }
                        else if (const auto* p = std::any_cast<double>(&v)) {
                            if (data->config->getDouble(key, *p) == *p) {
                                idx = static_cast<int>(i);
                                break;
                            }
                        }
                        else if (const auto* p = std::any_cast<String>(&v)) {
                            if (data->config->getString(key, *p) == *p) {
                                idx = static_cast<int>(i);
                                break;
                            }
                        }
                    }
                    combo->setCurrentIndex(idx >= 0 ? idx : 0);
                    break;
                }
                }
                w->blockSignals(false);
            }
        }
    }
}

void ConfigWindow::reset()
{
    auto* data = dptr();
    for (ConfigCategory* cat : data->registry->categories()) {
        for (ConfigGroup* grp : cat->groups()) {
            for (const ConfigItem* item : grp->items()) {
                if (item->hasDefault()) {
                    switch (item->type()) {
                    case ConfigItemType::String: data->config->setString(item->key(), item->defaultString()); break;
                    case ConfigItemType::Bool: data->config->setBool(item->key(), item->defaultBool()); break;
                    case ConfigItemType::Int: data->config->setInt(item->key(), item->defaultInt()); break;
                    case ConfigItemType::Double: data->config->setDouble(item->key(), item->defaultDouble()); break;
                    case ConfigItemType::Choice:
                        switch (item->defaultType()) {
                        case ConfigItemType::Int: data->config->setInt(item->key(), item->defaultInt()); break;
                        case ConfigItemType::Double: data->config->setDouble(item->key(), item->defaultDouble()); break;
                        default: data->config->setString(item->key(), item->defaultString()); break;
                        }
                        break;
                    }
                }
                else {
                    data->config->remove(item->key());
                }
            }
        }
    }
    refresh();
}

raw_ptr<ConfigRegistry> ConfigWindow::registry() const
{
    return dptr()->registry;
}

raw_ptr<ConfigManager> ConfigWindow::config() const
{
    return dptr()->config;
}

inline auto ConfigWindow::dptr() -> Impl*
{
    return static_cast<Impl*>(Window::d);
}

inline auto ConfigWindow::dptr() const -> const Impl*
{
    return static_cast<const Impl*>(Window::d);
}

V_APPFWGUI_NS_END
